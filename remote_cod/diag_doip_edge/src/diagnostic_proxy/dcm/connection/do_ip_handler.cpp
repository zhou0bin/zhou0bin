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

#include "dcm/connection/do_ip_handler.h"

#include <ifaddrs.h>

#include "dcm/connection/do_ip_communication.h"
#include "dcm/connection/do_ip_connection_manager.h"

#include "common/multi_byte_type.h"
#include "log.h"

#include "asf/diag/uds_transport/protocol_handler.h"
using asf::diag::uds_transport::UdsTransportProtocolHandler;
#include "uds_transport/protocol_types_priv.h"
using asf::diag::uds_transport::UdsTransportProtocol;

#include "asf/diag/uds_transport/uds_message.h"
using InitializationResult = asf::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult;
using TransmissionResult = asf::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult;
using TargetAddressType = asf::diag::uds_transport::UdsMessage::TargetAddressType;

namespace
{
constexpr uint16_t kUdpDiscoveryPort = 13400;
const auto kLimitedBroadcastIp = "255.255.255.255";
const auto maxCheckCount = 10;
}  // namespace

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

DoIpHandler::DoIpHandler(const std::string& config, uds_transport::UdsTransportProtocolMgr& transport_protocol_mgr)
    : UdsTransportProtocolHandler(UdsTransportProtocol::DoIp, transport_protocol_mgr)
    , threads_()
    , doip_connection_mgr_(std::make_unique<DoIpConnectionManager>(config))
    , udp_handler_(std::make_unique<DoIpUdpHandler>(config, *doip_connection_mgr_))
    , exit_requested_(false)
    , local_ip_()
    , tcp_port_(0)
    , udp_port_(0)
    , tcp_socket_(-1)
    , udp_socket_(-1)
    , multicast_ip_()
    , channel_state_notify_tbl_()
    , config_(config)
{
}

InitializationResult DoIpHandler::Initialize()
{
    // DoIpConnectionManager
    doip_connection_mgr_->Initialize();
    // DoIpUdpHandler
    udp_handler_->Initialize();
    // DoIpUdpInfo
    udp_handler_->GetDoIpUdpInfo().Initialize();
    udp_handler_->GetDoIpUdpInfo().RegisterVehicleAnnounceCallback([this]() { this->SendVehicleAnnouncementWhenSocketIsBound(); });

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
        if (network.HasMember("TcpPort") && network["TcpPort"].IsInt())
        {
            tcp_port_ = static_cast<unsigned>(network["TcpPort"].GetInt());
        }
        if (network.HasMember("UdpPort") && network["UdpPort"].IsInt())
        {
            udp_port_ = static_cast<unsigned>(network["UdpPort"].GetInt());
        }
        if (network.HasMember("ipv4MulticastIpAddress") && network["ipv4MulticastIpAddress"].IsString())
        {
            multicast_ip_ = network["ipv4MulticastIpAddress"].GetString();
        }
    }

    LOG_INFO << __FUNCTION__ << "ok! ";
    return InitializationResult::kInitializeOk;
}

void DoIpHandler::Start()
{
    // Create the sockets for communication

    // Set up addressing options
    in_addr local_ip{};
    (void)inet_aton(this->local_ip_.c_str(), &local_ip);
    const auto tcp_port = static_cast<in_port_t>(this->tcp_port_);
    const auto udp_port = static_cast<in_port_t>(this->udp_port_);

    std::string ifa_name{};

    if (local_ip_ != "0.0.0.0") {
        uint8_t check_count = 1;
        //wait network adapter up
        while (FindInterfaceIP(local_ip, ifa_name) == false) {
            LOG_WARN << __FUNCTION__ << "Start wait network adapter up. ";
            if (check_count == maxCheckCount) {
                LOG_ERROR << __FUNCTION__ << "local ip configuration error. ";
                return;
            }
            check_count++;
            (void)usleep(500*1000);
        }
    }
    LOG_ERROR << __FUNCTION__ << "local ip: " << inet_ntoa(local_ip);

    // Setup for Tcp socket
    {
        tcp_socket_ = socket(AF_INET, SOCK_STREAM, 0);

        if (tcp_socket_ < 0)
        {
            LOG_FATAL << __FUNCTION__ << "socket Error! errno: " << strerror(errno);
            throw std::runtime_error(strerror(errno));
        }

        // Set SO_REUSEADDR
        const int optval{1};
        if (setsockopt(tcp_socket_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        {
            LOG_FATAL << __FUNCTION__ << "setsockopt Error! errno: " << strerror(errno);
            throw std::runtime_error(strerror(errno));
        }

        struct sockaddr_in tcp_sockaddr;

        tcp_sockaddr.sin_family = AF_INET;
        tcp_sockaddr.sin_port = htons(tcp_port);
        tcp_sockaddr.sin_addr = local_ip;
        (void)memset(&tcp_sockaddr.sin_zero, 0, 8);

        if (bind(tcp_socket_, reinterpret_cast<sockaddr*>(&tcp_sockaddr), sizeof(struct sockaddr_in)) < 0)
        {
            LOG_FATAL << __FUNCTION__ << "bind Error! errno: " << strerror(errno);
            throw std::runtime_error(strerror(errno));
        }

        // Accept on the Tcp socket
        if (listen(tcp_socket_, kDoIp_Tcp_maxQueueLength) < 0)
        {
            LOG_FATAL << __FUNCTION__ << "listen Error! errno: " << strerror(errno);
            throw std::runtime_error(strerror(errno));
        }
    }

    // Setup for Udp port
    {
        udp_socket_ = socket(AF_INET, SOCK_DGRAM, 0);

        if (udp_socket_ < 0) {
            LOG_FATAL << __FUNCTION__ << "socket Error! errno: " << strerror(errno);
            throw std::runtime_error(strerror(errno));
        }

        struct sockaddr_in udp_sockaddr;

        udp_sockaddr.sin_family = AF_INET;
        udp_sockaddr.sin_port = htons(udp_port);
        // udp_sockaddr.sin_addr = local_ip;
        udp_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        (void)memset(&udp_sockaddr.sin_zero, 0, 8);

        if (!ifa_name.empty())
        {
            if (setsockopt(udp_socket_, SOL_SOCKET, SO_BINDTODEVICE, ifa_name.c_str(), ifa_name.length()) == 0)
            {
                LOG_INFO << __FUNCTION__ << "setsockopt SO_BINDTODEVICE: " << ifa_name << "length: " << ifa_name.length();
            }
        }

        //broadcast
        if (multicast_ip_ == "0.0.0.0")
        {
            const int opt = 1;
            if (setsockopt(udp_socket_, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) < 0)
            {
                LOG_FATAL << __FUNCTION__ << "setsockopt Error! errno: " << strerror(errno);
                throw std::runtime_error(strerror(errno));
            }
        }
        //multicast
        else
        {
            const int loop = 0;
            if (setsockopt(udp_socket_, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
            {
                LOG_FATAL << __FUNCTION__ << "setsockopt Error! errno: " << strerror(errno);
                throw std::runtime_error(strerror(errno));
            }

            struct ip_mreq mreq;
            mreq.imr_multiaddr.s_addr = inet_addr(multicast_ip_.c_str());
            mreq.imr_interface = local_ip;
            //mreq.imr_interface.s_addr = htonl(INADDR_ANY);
            if (setsockopt(udp_socket_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
            {
                LOG_FATAL << __FUNCTION__ << "setsockopt Error! errno: " << strerror(errno);
                throw std::runtime_error(strerror(errno));
            }
        }

        if (bind(udp_socket_, reinterpret_cast<sockaddr*>(&udp_sockaddr), sizeof(udp_sockaddr)) < 0)
        {
            LOG_FATAL << __FUNCTION__ << "bind Error! errno: " << strerror(errno);
            throw std::runtime_error(strerror(errno));
        }

        //SendVehicleAnnouncementWhenSocketIsBound();
    }

    /* Note: Threads are spawned at the very end so that the TcpHandler is not spawned before it is clear that the Udp
     * socket was successfully set up.
     */

    // Spawn a thread to handle Tcp messages
    (void)threads_.emplace_back(std::thread(&DoIpHandler::TcpHandler, this));
    (void)pthread_setname_np(threads_.back().native_handle(), "TcpHandler");

    // Spawn a thread to handle Udp messages
    (void)threads_.emplace_back(std::thread(&DoIpHandler::UdpHandler, this));
    (void)pthread_setname_np(threads_.back().native_handle(), "UdpHandler");

    udp_handler_->GetDoIpUdpInfo().Start();
}

void DoIpHandler::Stop()
{
    exit_requested_.store(true, std::memory_order_release);

    // Use shutdown() in addition to close() to ensure that the respective threads are unblocked.
    if (tcp_socket_ != -1) {
        (void)shutdown(tcp_socket_, SHUT_RDWR);
        (void)close(tcp_socket_);
        tcp_socket_ = -1;
    }
    if (udp_socket_ != -1) {
        (void)shutdown(udp_socket_, SHUT_RDWR);
        (void)close(udp_socket_);
        udp_socket_ = -1;
    }

    for (std::thread& thread : threads_) {
        thread.join();
    }
    threads_.clear();

    // stop udp handler
    udp_handler_->GetDoIpUdpInfo().Stop();
    // stop our internal connection manager
    doip_connection_mgr_->Stop();
    transportprotocol_manager_.HandlerStopped(UdsTransportProtocol::DoIp);
}

void DoIpHandler::TcpHandler()
{
    LOG_ERROR << __FUNCTION__ << "Starting TcpHandler (Accept). ";
    // Spawn an additional thread whenever an accept happens
    bool is_socket_open{true};  // TODO(PAASR-169): Rework is_socket_open to be replaced by exceptions
    while ((!exit_requested_.load(std::memory_order_acquire)) && is_socket_open)
    {
        struct sockaddr client_addr;
        socklen_t length(sizeof(struct sockaddr));
        LOG_ERROR << __FUNCTION__ << "Accept ";
        const int retval{accept(tcp_socket_, &client_addr, &length)};

        // we don't need to check if exit was requested, since if it was accept would have canceled the thread
        if (retval < 0) {
            is_socket_open = false;
            const int current_errno{errno};
            if (DoIpCommunication::IsSocketClosed(current_errno)) {
                LOG_ERROR << __FUNCTION__ << "Socket closed. ";
            } else {
                LOG_ERROR << __FUNCTION__ << "Socket error! errno: " << current_errno;
            }
        } else {
            in_addr local_ip{};
            (void)inet_aton(this->local_ip_.c_str(), &local_ip);
            const auto remote_ip(reinterpret_cast<sockaddr_in*>(&client_addr)->sin_addr);
            DoIpConnection::Ptr connection{doip_connection_mgr_->FindOrCreateConnection(local_ip, remote_ip)};
            if (connection == nullptr) {
                LOG_ERROR << __FUNCTION__ << "No connection found! ";
            } else {
                LOG_ERROR << __FUNCTION__ << "remote ip: " << inet_ntoa(remote_ip);
                connection->SetTransferUpperHandler([this](const ChannelID channelId, const uds_transport::ByteVector& payload)
                    {
                        this->RecvUDSMessageCallback(channelId, payload);
                    });
                connection->SetConnectionStateHandler([this](const ChannelID channelId, const bool state)
                    {
                        this->ConnectionStateChanged(channelId, state);
                    });
                connection->AddTcpSocket(retval);
            }
        }
    }
}

void DoIpHandler::UdpHandler()
{
    LOG_ERROR << __FUNCTION__ << "Starting UdpHandler (Recv). ";
    bool is_socket_open(true);  // TODO(PAASR-169): Rework is_socket_open to be replaced by exceptions
    while ((!exit_requested_.load(std::memory_order_acquire)) && is_socket_open)
    {
        // Create a buffer for holding the DoIp header
        char buffer;

        struct sockaddr client_addr;
        socklen_t length(sizeof(client_addr));

        // Peek at the socket: Extract connection information & DoIp Payload length
        const ssize_t bytes_received{recvfrom(udp_socket_, &buffer, 1, MSG_PEEK | MSG_TRUNC, &client_addr, &length)};

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
        } else if (bytes_received > std::numeric_limits<ssize_t>::max()) {
            // We would read more than a single Udp packet.
            LOG_ERROR << __FUNCTION__ << "Insufficient buffer size! ";
        } else {
            // Hand off buffer to correct connection object
            struct sockaddr_in* const client_addr_in{reinterpret_cast<sockaddr_in*>(&client_addr)};
            in_addr local_ip{};
            (void)inet_aton(this->local_ip_.c_str(), &local_ip);
            const struct ::in_addr remote_ip(client_addr_in->sin_addr);

            if (udp_handler_ == nullptr) {
                LOG_ERROR << __FUNCTION__ << "No connection found! ";
            } else {
                try {
                    udp_handler_->HandleUdpMessage(udp_socket_, *client_addr_in, static_cast<unsigned int>(bytes_received), is_socket_open);
                }
                catch(std::runtime_error& e) {
                    LOG_ERROR << __FUNCTION__ << "runtime_error: " << e.what();
                    continue;
                }
            }
        }
    }
}

void DoIpHandler::SendVehicleAnnouncementWhenSocketIsBound()
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (udp_socket_ < 0)
    {
        LOG_ERROR << __FUNCTION__ << "socket is closed! ";
        return;
    }
    in_addr local_ip{};
    (void)inet_aton(this->local_ip_.c_str(), &local_ip);
    struct sockaddr_in limitedBroadcast{};
    limitedBroadcast.sin_family = AF_INET;
    limitedBroadcast.sin_port = htons(kUdpDiscoveryPort);
    if (multicast_ip_ == "0.0.0.0")
    {
        (void)inet_aton(kLimitedBroadcastIp, &limitedBroadcast.sin_addr);
    }
    else
    {
        (void)inet_aton(multicast_ip_.c_str(), &limitedBroadcast.sin_addr);
    }
    (void)memset(&limitedBroadcast.sin_zero, 0, 8);
    const int on = 1;
    if (multicast_ip_ == "0.0.0.0")
    {
        (void)setsockopt(udp_socket_, SOL_SOCKET, SO_REUSEADDR|SO_BROADCAST, &on, sizeof(on));
    }
    else
    {
        (void)setsockopt(udp_socket_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    }

    bool vehicleAnnouncementSent = false;

    if (udp_handler_ != nullptr)
    {
        vehicleAnnouncementSent = udp_handler_->SendVehicleAnnouncement(udp_socket_, limitedBroadcast);
    }

    if (!vehicleAnnouncementSent) {
        LOG_WARN << __FUNCTION__ << "The Vehicle Announcement Message could not be sent. ";
    }
}

void DoIpHandler::RecvUDSMessageCallback(const ChannelID channelId, const uds_transport::ByteVector& payload)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    DoIpConnection::Ptr connection_ptr = doip_connection_mgr_->FindConnectionByID(channelId);
    if (connection_ptr == nullptr)
    {
        LOG_ERROR << __FUNCTION__ << "Can not FindConnectionByID: " << channelId;
        return;
    }

    const DoIpPacket tmpPacket{DoIpPacket::kHost, doip_connection_mgr_->GetLogicalAddress()};

    uint16_t source_address{ 0 };
    uint16_t target_address{ 0 };
    if (payload.size() > 3)
    {
        SetByte(source_address, payload.at(0), 1);
        SetByte(source_address, payload.at(1), 0);
        SetByte(target_address, payload.at(2), 1);
        SetByte(target_address, payload.at(3), 0);
    }

    const std::size_t size = payload.size();
    if (size < 5)
    {
        LOG_ERROR << __FUNCTION__ << "payload size < 5! size: " << size;
        DoIpPacket doipNackPacket = tmpPacket.ConstructDiagNack(DoIpDiagMessageNackCodes::kTransportProtocolError, doip_connection_mgr_->GetLogicalAddress(), source_address);
        (void)connection_ptr->Transmit(doipNackPacket);
        return;
    }

    TargetAddressType addressingType {TargetAddressType::kPhysical};
    if (target_address == doip_connection_mgr_->GetFunctionalAddress())
    {
        addressingType = TargetAddressType::kFunctional;
    }

    const uds_transport::UdsTransportProtocolMgr::GlobalChannelIdentifier globalChannelID = connection_ptr->GetIdentifyingTuple();
    LOG_DEBUG << __FUNCTION__ << "ChannelID: " << std::get<0>(globalChannelID) << "," << std::get<1>(globalChannelID);

    //Indicat Message
    auto IndicateResult = transportprotocol_manager_.IndicateMessage(source_address, target_address, addressingType, globalChannelID,
                                                                     size, 0 /*priority*/, "" /*protocolKind*/, {} /*payloadInfo*/);

    if (IndicateResult.first != uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk)
    {
        LOG_ERROR << __FUNCTION__ << "IndicateMessage failed! errno: " << static_cast<unsigned>(IndicateResult.first);
        DoIpPacket doipNackPacket = tmpPacket.ConstructDiagNack(DoIpDiagMessageNackCodes::kOutOfMemoryToStore, doip_connection_mgr_->GetLogicalAddress(), source_address);
        (void)connection_ptr->Transmit(doipNackPacket);
        return;
    }

    uds_transport::UdsMessageExtPtr UdsMsgPtr = std::move(IndicateResult.second);

    // LOG_INFO << __FUNCTION__ << "IndicateMessage OK! ";
    // DoIpPacket doipAckPacket = tmpPacket.ConstructDiagnosticAck(doip_connection_mgr_->GetLogicalAddress(), source_address);
    // connection_ptr->Transmit(doipAckPacket);

    //Handle Message
    uds_transport::ByteVector& UdsMsgPayload = UdsMsgPtr->GetPayload();
    UdsMsgPayload.assign(payload.begin() + 4, payload.end());
    transportprotocol_manager_.HandleMessage(std::move(UdsMsgPtr));
}

void DoIpHandler::Transmit(UdsMessageConstPtr const message, const ChannelID channelId)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    DoIpConnection::Ptr connection_ptr = doip_connection_mgr_->FindConnectionByID(channelId);
    if (connection_ptr == nullptr)
    {
        LOG_ERROR << __FUNCTION__ << "Can not FindConnectionByID: " << channelId;
        return;
    }
    const bool send_result = connection_ptr->Transmit(*message);
    if (!send_result)
    {
        LOG_ERROR << __FUNCTION__ << "failed! ChannelID: " << GetHandlerID() << "," << connection_ptr->GetConnectionID();
        LOG_ERROR << __FUNCTION__ << "failed! size: " << message->GetPayload().size() << "payload: " << LOG_RAW_BUFFER(message->GetPayload());
    }
    LOG_DEBUG << __FUNCTION__ << "Transmit Message Result: " << send_result;
    const TransmissionResult rst = ((send_result == true) ? TransmissionResult::kTransmitOk : TransmissionResult::kTransmitFailed);
    transportprotocol_manager_.TransmitConfirmation(rst);
}

void DoIpHandler::TransmitAck(const uint16_t sa, const uint16_t ta, const ChannelID channelId)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const DoIpPacket tmpPacket{DoIpPacket::kHost, sa};
    DoIpPacket doipAckPacket = tmpPacket.ConstructDiagnosticAck(sa, ta);

    DoIpConnection::Ptr connection_ptr = doip_connection_mgr_->FindConnectionByID(channelId);
    if (connection_ptr == nullptr)
    {
        LOG_ERROR << __FUNCTION__ << "Can not FindConnectionByID: " << channelId;
        return;
    }
    const bool send_result = connection_ptr->Transmit(doipAckPacket);
    LOG_DEBUG << __FUNCTION__ << "Transmit Message Result: " << send_result;
}

void DoIpHandler::TransmitNack(const uint16_t sa, const uint16_t ta, const uint8_t nack_code, const ChannelID channelId)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const DoIpPacket tmpPacket{DoIpPacket::kHost, sa};
    DoIpPacket doipAckPacket = tmpPacket.ConstructDiagNack(nack_code, sa, ta);

    DoIpConnection::Ptr connection_ptr = doip_connection_mgr_->FindConnectionByID(channelId);
    if (connection_ptr == nullptr)
    {
        LOG_ERROR << __FUNCTION__ << "can not FindConnectionByID: " << channelId;
        return;
    }
    const bool send_result = connection_ptr->Transmit(doipAckPacket);
    LOG_DEBUG << __FUNCTION__ << "Transmit Message Result: " << send_result << "nack_code: " << nack_code;
}

bool DoIpHandler::NotifyReestablishment(const ChannelID channelId)
{
    LOG_DEBUG << __FUNCTION__ << "channelId: " << channelId;
    if (find(channel_state_notify_tbl_.begin(), channel_state_notify_tbl_.end(), channelId) == channel_state_notify_tbl_.end())
    {
        channel_state_notify_tbl_.push_back(channelId);
    }
    return true;
}

void DoIpHandler::ConnectionStateChanged(const uds_transport::ChannelID channelId, const bool state)
{
    if (channel_state_notify_tbl_.empty())
    {
        return;
    }
    if (find(channel_state_notify_tbl_.begin(), channel_state_notify_tbl_.end(), channelId) == channel_state_notify_tbl_.end())
    {
        LOG_WARN << __FUNCTION__ << "can not find channelId: " << channelId;
        return;
    }
    LOG_DEBUG << __FUNCTION__ << "channelId: " << channelId << "state: " << state;
    const uds_transport::UdsTransportProtocolMgr::GlobalChannelIdentifier globalChannelID {uds_transport::DoIp, channelId};
    if (state == true)
    {
        transportprotocol_manager_.ChannelReestablished(globalChannelID);
    }
    else
    {
        transportprotocol_manager_.ChannelDisconnected(globalChannelID);
    }
}

bool DoIpHandler::FindInterfaceIP(const struct in_addr local_ip, std::string& ifa_name)
{
    bool success = false;
    struct ifaddrs *currentInterface;
    struct ifaddrs *firstInterface;

    try {
        if (getifaddrs(&firstInterface) == 0) {
            for (currentInterface = firstInterface; currentInterface != nullptr;
                currentInterface = currentInterface->ifa_next) {
                if (currentInterface->ifa_addr == nullptr) {
                    continue;
                }
                //check IPv4 Address
                if (currentInterface->ifa_addr->sa_family == AF_INET) {
                     struct sockaddr_in* const currentSocketAddress
                        = reinterpret_cast<struct sockaddr_in*>(currentInterface->ifa_addr);
                    // auto tmpAddrPtr=&((struct sockaddr_in *)currentInterface->ifa_addr)->sin_addr;
                    // char addressBuffer[INET_ADDRSTRLEN];
                    // inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                    // LOG_INFO << __FUNCTION__ << "ifa_name: " << currentInterface->ifa_name << "ip_address: " << addressBuffer;

                    if (currentSocketAddress->sin_addr.s_addr == local_ip.s_addr) {
                        ifa_name = currentInterface->ifa_name;
                        success = true;
                        break;
                    }
                }
            }
            freeifaddrs(firstInterface);
        }
    } catch (...) {
        success = false;
    }
    return success;
}


} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
