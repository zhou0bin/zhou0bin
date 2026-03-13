// --------------------------------------------------------------------------
// |                _     _              _____         _____                |
// |               |  \  | |            / ____|  /\   |  __ \               |
// |               | | \ | |  __       | (___   /  \  | |__) |              |
// |               | |\ \| | /__\|   |  \___ \ / /\ \ |  _  /               |
// |               | | \ \ ||    |   |   ___) / /__\ \| | \ \               |
// |               |_|  \_\| \__/ \_/|/|_____/________\_|  \_\              |
// |                                                                        |
// --------------------------------------------------------------------------
// COPYRIGHT
// --------------------------------------------------------------------------
//
// This software is copyright protected and proprietary to Neusoft Reach.
// Neusoft Reach grants to you only those rights as set out in the license 
// conditions.
// All other rights remain with Neusoft Reach.
// --------------------------------------------------------------------------

#include "dcm/connection/do_ip_udp_handler.h"

#include "dcm/connection/do_ip_communication.h"
#include "dcm/connection/do_ip_connection_manager.h"
#include "dcm/connection/do_ip_constants.h"

#include "common/random_number_generator.h"
#include "common/multi_byte_type.h"

#include "log.h"

#include "asf/diag/uds_transport/protocol_types.h"
using asf::diag::uds_transport::ByteVector;

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

DoIpUdpHandler::DoIpUdpHandler(const std::string& config, DoIpConnectionManager& connManager)
    : connManager_(connManager)
    , doip_udp_info_(config)
    , local_ip_()
    , entityIdentificationPtr_(nullptr)
    , vehicleIdentificationNumber_(doip::kDefaultVehicleIdentificationNumber_00)
    , vinMutex_()
    , gidStatus_()
    , gidMutex_()
    , powerMode_()
    , powerModeMutex_()
    , config_(config)
{
    entityIdentificationPtr_ = std::make_shared<EntityIdentification>();
}

void DoIpUdpHandler::Initialize()
{
    std::ifstream ifs(config_);
    if (ifs.good())
    {
        rapidjson::IStreamWrapper ist(ifs);
        rapidjson::Document doc;
        (void)doc.ParseStream(ist);

        const rapidjson::Value& network = doc["Network"];
        if (network.HasMember("local_ip") && network["local_ip"].IsString())
        {
            local_ip_ = network["local_ip"].GetString();
        }
    }
}

void DoIpUdpHandler::HandleUdpMessage(const int udp_socket,
    struct sockaddr_in& remote_addr,
    const unsigned int bytes_available,
    bool& is_socket_open)
{
    bool send_respons = true;
    uint32_t payload_length_udp = 0;

    if (udp_socket < 0) {
        is_socket_open = false;
        LOG_ERROR << __FUNCTION__ << "Invalid Udp Socket provided. ";
        return;
    } else if (bytes_available < kDoIp_HeaderTotal_length) {
        DoIpCommunication::SocketReadPayloadDiscard(udp_socket, bytes_available, is_socket_open);
        LOG_ERROR << __FUNCTION__ << "Incomplete packet pending in Udp socket. ";
        return;
    } else {}

    //DoIpPacket::PayloadLength expected_payload_length{bytes_available - kDoIp_HeaderTotal_length};
    DoIpPacket doip_message{DoIpPacket::kHost, connManager_.GetLogicalAddress()};
    DoIpPacket doip_response_message(DoIpPacket::kHost, connManager_.GetLogicalAddress());
    doip_response_message.SetProtocolVersion(kSupportedDoIpVersion);

    uint8_t buffer[kDoIp_HeaderTotal_length] = {0};
    struct sockaddr client_addr;
    socklen_t length(sizeof(client_addr));

    // Peek at the socket: DoIp head only
    const ssize_t bytes_received{recvfrom(udp_socket, &buffer, kDoIp_HeaderTotal_length, MSG_PEEK, &client_addr, &length)};
    if (bytes_received == 0) {
        is_socket_open = false;
    } else if (bytes_received < 0) {
        is_socket_open = false;
        const int current_errno{errno};
        if (DoIpCommunication::IsSocketClosed(current_errno)) {
            LOG_ERROR << __FUNCTION__ << "Socket closed. ";
        } else {
            LOG_ERROR << __FUNCTION__ << "Socket error! errno: " << current_errno;
        }
        return;
    } else {
        if ((buffer[0] + buffer[1]) == static_cast<uint8_t>(0xff)) {
            doip_message.SetProtocolVersion(buffer[0]);
        } else {
            LOG_WARN << __FUNCTION__ << "DoIP Header Version error. ";
            doip_response_message = doip_message.ConstructNack(DoIpNackCodes::kIncorrectPatternFormat);
            DoIpCommunication::SocketWrite(udp_socket, doip_response_message, is_socket_open, &remote_addr);
            DoIpCommunication::SocketReadPayloadDiscard(udp_socket, bytes_available, is_socket_open);
            return;
        }

        uint16_t payloadtype = 0;
        SetByte(payloadtype, buffer[2], 1);
        SetByte(payloadtype, buffer[3], 0);
        doip_message.SetPayloadType(payloadtype);
        //doip_message.SetPayloadType((static_cast<uint16_t>(buffer[2]))<<8 | buffer[3]);
        //payload_length_udp = ((uint32_t)buffer[4]<<24) | ((uint32_t)buffer[5]<<16) | ((uint32_t)buffer[6]<<8) | (buffer[7]);
        SetByte(payload_length_udp, buffer[4], 3);
        SetByte(payload_length_udp, buffer[5], 2);
        SetByte(payload_length_udp, buffer[6], 1);
        SetByte(payload_length_udp, buffer[7], 0);
        // DoIP-041, ISO13400-2:2012
        if (DoIpCommunication::CheckGenericDoIPHeaderVersion(doip_message) == false) {
            LOG_WARN << __FUNCTION__ << "DoIP Header Version error. ";
            doip_response_message = doip_message.ConstructNack(DoIpNackCodes::kIncorrectPatternFormat);
            DoIpCommunication::SocketWrite(udp_socket, doip_response_message, is_socket_open, &remote_addr);
            DoIpCommunication::SocketReadPayloadDiscard(udp_socket, bytes_available, is_socket_open);
            return;
        }

        // DoIP-042, ISO13400-2:2012
        if (is_socket_open) {
            const DoIpPacket::PayloadType type = doip_message.payload_type_;
            if ((DoIpPayloadType::kGenericDoIpNack == type)
             || (DoIpPayloadType::kVehicleIdentificationRequest == type)
             || (DoIpPayloadType::kVehicleIdentificationRequestWithEid == type)
             || (DoIpPayloadType::kVehicleIdentificationRequestWithVin == type)
             || (DoIpPayloadType::kDoIpEntityStatusRequest == type)
             || (DoIpPayloadType::kDiagnosticPowerModeInformationRequest == type)
             || (DoIpPayloadType::kVehicleAnnouncement == type)) {
                LOG_INFO << __FUNCTION__ << "Recv Payload Type: " << LOG_HEX(type);
            } else {
                LOG_WARN << __FUNCTION__ << "Payload Type error. ";
                doip_response_message = doip_message.ConstructNack(DoIpNackCodes::kUnknownPayloadType);
                DoIpCommunication::SocketWrite(udp_socket, doip_response_message, is_socket_open, &remote_addr);
                DoIpCommunication::SocketReadPayloadDiscard(udp_socket, bytes_available, is_socket_open);
                return;
            }
        }
    }

    // DoIP-043, ISO13400-2:2012
    const DoIpPacket::PayloadLength payload_length{payload_length_udp};
    //if (expected_payload_length > connManager_.GetMaxDataSize()) {
    if (payload_length > connManager_.GetMaxDataSize()) {
        LOG_WARN << __FUNCTION__ << "DoIP Payload Length > MaxDataSize error. ";
        doip_response_message = doip_message.ConstructNack(DoIpNackCodes::kMessageTooLarge);
        DoIpCommunication::SocketWrite(udp_socket, doip_response_message, is_socket_open, &remote_addr);
        DoIpCommunication::SocketReadPayloadDiscard(udp_socket, bytes_available, is_socket_open);
        return;
    }

    // DoIP-044, ISO13400-2:2012
    try {
        // Allocate sufficient buffer to capture the whole packet
        //doip_message.SetPayloadLength(expected_payload_length);
        doip_message.SetPayloadLength(payload_length);
    }
    catch(std::bad_alloc& e) {
        LOG_ERROR << __FUNCTION__ << "bad_alloc! ";
        doip_response_message = doip_message.ConstructNack(DoIpNackCodes::kOutOfMemory);
        DoIpCommunication::SocketWrite(udp_socket, doip_response_message, is_socket_open, &remote_addr);
        DoIpCommunication::SocketReadPayloadDiscard(udp_socket, bytes_available, is_socket_open);
        return;
    }

    try {
        DoIpCommunication::SocketRead(udp_socket, doip_message, is_socket_open, true);
    }
    catch(std::runtime_error& e) {
        LOG_ERROR << __FUNCTION__ << "runtime_error: " << e.what();
        return;
    }

    // DoIP-045, ISO13400-2:2012
    if (DoIpCommunication::CheckPayloadTypeSpecificLength(doip_message) == false) {
        LOG_WARN << __FUNCTION__ << "DoIP Payload Specific Length error. ";
        doip_response_message = doip_message.ConstructNack(DoIpNackCodes::kInvalidPayloadLength);
        DoIpCommunication::SocketWrite(udp_socket, doip_response_message, is_socket_open, &remote_addr);
        return;
    }

    if (DoIpPayloadType::kVehicleIdentificationRequest == doip_message.payload_type_) {
        BuildVehicleAnnouncementMessage(udp_socket, doip_response_message);

        const auto max_init_time = connManager_.GetMaxInitialVehicleAnnouncementTime() * 1000.0;
        if (static_cast<int>(max_init_time) != 0) {
            common::RandomNumberGenerator DoIpAnnounceWaitTime{0,static_cast<int>(max_init_time)};
            std::this_thread::sleep_for(std::chrono::milliseconds(DoIpAnnounceWaitTime.get()));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(doip::DoIpAnnounceWait()));
        }
    } else if (DoIpPayloadType::kVehicleIdentificationRequestWithEid == doip_message.payload_type_) {
        ByteVector localEid;
        GetEntityIdentification(udp_socket, localEid);

        bool eidsAreEqual{false};
        eidsAreEqual = (doip_message.payload_ == localEid);

        if (eidsAreEqual) {
            BuildVehicleAnnouncementMessage(udp_socket, doip_response_message);
        } else {
            // The Nack might have the wrong nack code, but there is no invalid argument nack code!
            doip_response_message = doip_message.ConstructNack(DoIpNackCodes::kIncorrectPatternFormat);
        }
    } else if (DoIpPayloadType::kVehicleIdentificationRequestWithVin == doip_message.payload_type_) {
        const auto localVin = GetVehicleIdentificationNumber();

        const bool vinsAreEqual = (doip_message.payload_ == localVin);

        if (vinsAreEqual) {
            BuildVehicleAnnouncementMessage(udp_socket, doip_response_message);
        } else {
            // The Nack might have the wrong nack code, but there is no invalid argument nack code!
            doip_response_message = doip_message.ConstructNack(DoIpNackCodes::kIncorrectPatternFormat);
        }
    } else if (DoIpPayloadType::kDoIpEntityStatusRequest == doip_message.payload_type_) {
        // LOG_INFO << __FUNCTION__ << "Got DoIP entity status request. ";
        BuildEntityStatusResponseMessage(doip_response_message);
    } else if (DoIpPayloadType::kDiagnosticPowerModeInformationRequest == doip_message.payload_type_) {
        // LOG_INFO << __FUNCTION__ << "Got DoIP power mode request. ";
        BuildPowerModeResponseMessage(doip_response_message);
    } else {
        send_respons = false;
    }

    if (is_socket_open && send_respons) {
        DoIpCommunication::SocketWrite(udp_socket, doip_response_message, is_socket_open, &remote_addr);
        if (is_socket_open) {
            LOG_INFO << __FUNCTION__ << "Send Payload Type: " << LOG_HEX(doip_response_message.payload_type_);
        }
    }
}

bool DoIpUdpHandler::SendVehicleAnnouncement(const int udpDiscoverySocket, struct sockaddr_in& limitedBroadcast) noexcept(false)
{
    DoIpPacket vehicleAnnouncementMessage(DoIpPacket::kHost, connManager_.GetLogicalAddress());
    vehicleAnnouncementMessage.SetProtocolVersion(kSupportedDoIpVersion);

    BuildVehicleAnnouncementMessage(udpDiscoverySocket, vehicleAnnouncementMessage);

    // DoIP 050, ISO13400-2:2012
    auto timesAnnounced = 0;
    auto socketOpen{true};
    do {
        if (timesAnnounced > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(doip::kAnnounceInterval));
        }

        if (socketOpen) {
            try {
                // As SocketWrite is a blocking write, this may be blocked longer than 500ms
                DoIpCommunication::SocketWrite(udpDiscoverySocket, vehicleAnnouncementMessage, socketOpen, &limitedBroadcast);
            } catch (...) {
                socketOpen = false;
                break;
            }
        }

        ++timesAnnounced;
    } while (timesAnnounced < doip::kAnnounceNum);

    if (!socketOpen) {
        LOG_WARN << __FUNCTION__ << "Vehicle Announcement was not sent correctly. ";
    }

    return socketOpen;
}

bool DoIpUdpHandler::GetEntityIdentification(const int givenSocket, ByteVector& eid) noexcept
{
    bool eidResult = false;
    const auto doIpEidUseMac = connManager_.GetEidUseMac();
    if (doIpEidUseMac == true) {
        if ((entityIdentificationPtr_ != nullptr) && entityIdentificationPtr_) {
            in_addr local_ip{};
            (void)inet_aton(this->local_ip_.c_str(), &local_ip);
            eidResult = entityIdentificationPtr_.get()->GetEntityIdentification(givenSocket, local_ip, eid);
        }
    } else {
        const auto doIpEid = connManager_.GetEid();
        eid.push_back(GetByte(doIpEid, 5));
        eid.push_back(GetByte(doIpEid, 4));
        eid.push_back(GetByte(doIpEid, 3));
        eid.push_back(GetByte(doIpEid, 2));
        eid.push_back(GetByte(doIpEid, 1));
        eid.push_back(GetByte(doIpEid, 0));
        eidResult = true;
    }
    return eidResult;
}

ByteVector DoIpUdpHandler::GetVehicleIdentificationNumber() noexcept(false)
{
    const std::lock_guard<std::mutex> locker_v(vinMutex_);

    QueryVehicleIdentification();

    return vehicleIdentificationNumber_;
}

GidStatus DoIpUdpHandler::GetGroupIdentification() noexcept(false)
{
    const auto doIpGid = connManager_.GetGid();
    if (doIpGid != 0) {
        const std::lock_guard<std::mutex> locker_g(gidMutex_);
        gidStatus_.GID.clear();
        gidStatus_.GID.push_back(GetByte(doIpGid, 5));
        gidStatus_.GID.push_back(GetByte(doIpGid, 4));
        gidStatus_.GID.push_back(GetByte(doIpGid, 3));
        gidStatus_.GID.push_back(GetByte(doIpGid, 2));
        gidStatus_.GID.push_back(GetByte(doIpGid, 1));
        gidStatus_.GID.push_back(GetByte(doIpGid, 0));
        gidStatus_.furtherActionReq = static_cast<uint8_t>(doip::GIDFurtherActions::kNone);
        gidStatus_.syncStatus = static_cast<uint8_t>(doip::GIDSyncStatus::kSynced);
    } else {
        QueryGroupIdentification();
        // const std::lock_guard<std::mutex> locker_g(gidMutex_);
        //gidStatus_.furtherActionReq = static_cast<uint8_t>(doip::GIDFurtherActions::kNone);
    }

    return gidStatus_;
}
asf::diag::PowerModeType DoIpUdpHandler::GetDoipPowerMode() noexcept(false)
{
    QueryDiagnosticDoipPowerMode();
    return powerMode_;
}

void DoIpUdpHandler::QueryVehicleIdentification() noexcept
{
    // Init with default values as of table 40 of ISO 13400-2:2012
    if (connManager_.GetVinInvalidPattern() == static_cast<uint8_t>(255)) {
        vehicleIdentificationNumber_ = doip::kDefaultVehicleIdentificationNumber_FF;
    } else {
        vehicleIdentificationNumber_ = doip::kDefaultVehicleIdentificationNumber_00;
    }

    ByteVector vin_result{};
    if (doip_udp_info_.GetDoIPVIN(vin_result)) {
        vehicleIdentificationNumber_ = vin_result;
    } else {
        LOG_WARN << __FUNCTION__ << "VIN could not be retrieved, leaving default value. ";
    }
}

void DoIpUdpHandler::QueryGroupIdentification() noexcept
{
    // Init with default values as of tables 20 an 21 of ISO 13400-2:2012
    std::unique_lock<std::mutex> locker_qgi(gidMutex_);
    if (connManager_.GetGidInvalidPattern() == static_cast<uint8_t>(255)) {
        gidStatus_.GID = {doip::kDefaultGroupIdentification_FF};
        //gidStatus_.furtherActionReq = static_cast<uint8_t>(doip::GIDFurtherActions::kNone);
        //gidStatus_.syncStatus = static_cast<uint8_t>(doip::GIDSyncStatus::kIncomplete);
    } else {
        gidStatus_.GID = {doip::kDefaultGroupIdentification_00};
        //gidStatus_.furtherActionReq = static_cast<uint8_t>(doip::GIDFurtherActions::kNone);
        //gidStatus_.syncStatus = static_cast<uint8_t>(doip::GIDSyncStatus::kIncomplete);
    }
    gidStatus_.furtherActionReq = static_cast<uint8_t>(doip::GIDFurtherActions::kNone);
    gidStatus_.syncStatus = static_cast<uint8_t>(doip::GIDSyncStatus::kIncomplete);
    // locker_qgi.unlock();

    // GidStatus gid_result;
    // if (doip_udp_info_.GetDoIPGID(gid_result)) {
    //     const std::unique_lock<std::mutex> locker_gidStatus(gidMutex_);
    //     gidStatus_ = gid_result;
    // } else {
    //     LOG_WARN << __FUNCTION__ << "GID could not be retrieved, leaving default value. ";
    // }

}

void DoIpUdpHandler::QueryDiagnosticDoipPowerMode() noexcept
{
    std::unique_lock<std::mutex> locker_qddpm(powerModeMutex_);
    powerMode_ = PowerModeType::kNotReady;
    // locker_qddpm.unlock();

    // const auto power_mode_result = doip_udp_info_.GetDoIPPowerMode();
    // if (power_mode_result.HasValue()) {
    //     const std::unique_lock<std::mutex> locker_powerModeStatus(powerModeMutex_);
    //     powerMode_ = power_mode_result.Value();
    // } else {
    //     LOG_WARN << __FUNCTION__ << "Power mode could not be retrieved, leaving default value. ";
    // }
}

void DoIpUdpHandler::BuildVehicleAnnouncementMessage(const int givenSocket, DoIpPacket& vehicleAnnouncementBuffer)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    auto vinBuffer = GetVehicleIdentificationNumber();
    (void)vehicleAnnouncementBuffer.payload_.insert(vehicleAnnouncementBuffer.payload_.cbegin(), vinBuffer.begin(), vinBuffer.end());
    const uint16_t logicalAddress = connManager_.GetLogicalAddress();
    vehicleAnnouncementBuffer.payload_.push_back(static_cast<uint8_t>(logicalAddress >> 8));
    vehicleAnnouncementBuffer.payload_.push_back(static_cast<uint8_t>(logicalAddress & 0xff));
    ByteVector eidBuffer;
    GetEntityIdentification(givenSocket, eidBuffer);
    (void)vehicleAnnouncementBuffer.payload_.insert(vehicleAnnouncementBuffer.payload_.cend(), eidBuffer.begin(), eidBuffer.end());
    auto gid = GetGroupIdentification();
    (void)vehicleAnnouncementBuffer.payload_.insert(vehicleAnnouncementBuffer.payload_.cend(), gid.GID.begin(), gid.GID.end());
    vehicleAnnouncementBuffer.payload_.push_back(gid.furtherActionReq);
    if (connManager_.GetVehicleIdentificationSyncStatus() == true) {
        vehicleAnnouncementBuffer.payload_.push_back(gid.syncStatus);
    }
    vehicleAnnouncementBuffer.SetPayloadLength(static_cast<uint32_t>(vehicleAnnouncementBuffer.payload_.size()));
    vehicleAnnouncementBuffer.payload_type_ = DoIpPayloadType::kVehicleAnnouncement;
    vehicleAnnouncementBuffer.Ntoh();
}

void DoIpUdpHandler::BuildEntityStatusResponseMessage(DoIpPacket& doip_response_message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto doIpNodeType = connManager_.GetNodeType();
    const auto doIpCurrentlyOpenSockets = connManager_.GetCurrentlyOpenSocketsCount();
    const auto doIpMaxTcpSockets = connManager_.GetMaxConcurrentTcpSockets();
    const auto doIpMaxDataSize = connManager_.GetMaxDataSize();
    const auto doIpEntityStatusMaxByteFieldUse = connManager_.GetEntityStatusMaxByteFieldUse();

    doip_response_message.payload_.push_back(doIpNodeType);
    doip_response_message.payload_.push_back(doIpMaxTcpSockets);
    doip_response_message.payload_.push_back(doIpCurrentlyOpenSockets);

    //Table 37 MDS, ISO13400-2:2012
    if (doIpEntityStatusMaxByteFieldUse == true) {
        doip_response_message.payload_.push_back(GetByte(doIpMaxDataSize, 3));
        doip_response_message.payload_.push_back(GetByte(doIpMaxDataSize, 2));
        doip_response_message.payload_.push_back(GetByte(doIpMaxDataSize, 1));
        doip_response_message.payload_.push_back(GetByte(doIpMaxDataSize, 0));
    }
    doip_response_message.SetPayloadLength(static_cast<uint32_t>(doip_response_message.payload_.size()));
    doip_response_message.SetPayloadType(DoIpPayloadType::kDoIpEntityStatusResponse);
    doip_response_message.Ntoh();
}

void DoIpUdpHandler::BuildPowerModeResponseMessage(DoIpPacket& doip_response_message)
{
    const auto doipPowerMode = GetDoipPowerMode();
    doip_response_message.payload_.push_back(static_cast<uint8_t>(doipPowerMode));

    doip_response_message.SetPayloadLength(static_cast<uint32_t>(doip_response_message.payload_.size()));
    doip_response_message.SetPayloadType(DoIpPayloadType::kDiagnosticPowerModeInformationResponse);
    doip_response_message.Ntoh();
}

bool DoIpUdpHandler::ShouldIgnoreThisRequest(const DoIpPacket& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const DoIpPacket::PayloadType type = message.payload_type_;
    if ((DoIpPayloadType::kGenericDoIpNack == type)
     || (DoIpPayloadType::kVehicleAnnouncement == type)
     || (DoIpPayloadType::kRoutingActivationResponse == type)
     || (DoIpPayloadType::kAliveCheckResponse == type)
     || (DoIpPayloadType::kDoIpEntityStatusResponse == type)
     || (DoIpPayloadType::kDiagnosticPowerModeInformationResponse == type)
     || (DoIpPayloadType::kDiagnosticAck == type)
     || (DoIpPayloadType::kDiagnosticNack == type))
    {
        return true;
    }
    else
    {
        return false;
    }
}

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
