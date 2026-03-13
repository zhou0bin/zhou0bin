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

#include "dcm/connection/do_ip_communication.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

bool DoIpCommunication::IsSocketClosed(const int current_errno)
{
    switch (current_errno) {
    case EBADF:
    case EINVAL:
    case ENETRESET:
    case ECONNABORTED:
    case ECONNRESET: {
        // Socket closed, stop working.
        return true;
    }
    default: {
        // Actual error.
        return false;
    }
    }
}

void DoIpCommunication::SocketReadPayload(const int socket, DoIpPacket& doip_packet, bool& is_socket_open)
{
    DoIpPacket::PayloadLength bufferFill(0);
    while ((bufferFill < doip_packet.payload_.size()) && (is_socket_open)) {
        // Blocking read
        const ssize_t bytesRead{recv(socket, &(doip_packet.payload_.at(bufferFill)), (doip_packet.payload_.size() - bufferFill), 0)};
        //socket close
        if (bytesRead == 0) {
            is_socket_open = false;
            LOG_ERROR << __FUNCTION__ << "socket closed! ";
            break;
        } else if (bytesRead < 0) {
            is_socket_open = false;
            LOG_ERROR << __FUNCTION__ << "socket closed! errno: " << strerror(errno);
            break;
        } else {
            // We read additional data
            bufferFill += static_cast<DoIpPacket::PayloadLength>(bytesRead);
        }
    }
}

void DoIpCommunication::SocketReadPayloadDiscard(const int socket, DoIpPacket::PayloadLength discard_length, bool& is_socket_open)
{
    DoIpPacket::PayloadLength read_length(0);
    uint8_t payload[4096] = {0};
    while ((discard_length > 0) && (is_socket_open)) {
        if (discard_length > 4096) {
            read_length = 4096;
        } else {
            read_length = discard_length;
        }

        //test
        struct iovec io;
        io.iov_base = payload;
        io.iov_len = read_length;
        // Use msghdr for scattered read
        struct msghdr message_header = {};
        message_header.msg_name = NULL;
        message_header.msg_namelen = 0;
        message_header.msg_iov = &io;
        message_header.msg_iovlen = 1;
        message_header.msg_control = nullptr;
        message_header.msg_controllen = 0;
        message_header.msg_flags = 0;

        const auto bytesRead = recvmsg(socket, &message_header, 0);

        if (bytesRead == 0) {
            is_socket_open = false;
            LOG_ERROR << __FUNCTION__ << "socket closed! ";
            break;
        } else if (bytesRead < 0) {
            is_socket_open = false;
            LOG_ERROR << __FUNCTION__ << "socket closed! errno: " << strerror(errno);
            break;
        } else {
            // We read additional data
            discard_length -= static_cast<DoIpPacket::PayloadLength>(bytesRead);
        }
    }
}

void DoIpCommunication::SocketRead(const int socket, DoIpPacket& doip_packet, bool& is_socket_open, const bool read_payload)
{
    if (!is_socket_open) {
        return;
    }

    // Ensure packet is in host byteorder
    doip_packet.Ntoh();

    if (!read_payload) {
        // We do not want to read payload, so delete the payload buffer
        doip_packet.SetPayloadLength(0);
    }

    const DoIpPacket::PayloadLength expected_payload_length{doip_packet.payload_length_};
    DoIpPacket::ScatterArray scatter_array(doip_packet.GetScatterArray());
    // Use msghdr for scattered read
    struct msghdr message_header = {};
    message_header.msg_name = NULL;
    message_header.msg_namelen = 0;
    message_header.msg_iov = scatter_array.begin();
    message_header.msg_iovlen = scatter_array.size();
    message_header.msg_control = nullptr;
    message_header.msg_controllen = 0;
    message_header.msg_flags = 0;

    // Ensure packet is set to network byteorder
    doip_packet.Hton();
    const auto bytesReceived = recvmsg(socket, &message_header, 0);
    
    if (bytesReceived == 0) {
        is_socket_open = false;
        LOG_ERROR << __FUNCTION__ << "socket closed! ";
    } else if (bytesReceived < 0) {
        is_socket_open = false;
        LOG_ERROR << __FUNCTION__ << "socket closed! errno: " << strerror(errno);
    } else {} // Else we received something

    // Convert received packet to host byteorder
    doip_packet.Ntoh();

    if (is_socket_open) {
        // Check what was received

        // Complete Header?
        if (bytesReceived < kDoIp_HeaderTotal_length) {
            throw std::runtime_error("Incomplete DoIp Header received. ");
        }

        if (read_payload) {
            if (expected_payload_length != doip_packet.payload_length_) {
                throw std::runtime_error("Read DoIp Packet Payload of unexpected size. ");
            } else if (static_cast<unsigned long long>(bytesReceived)
             != (kDoIp_HeaderTotal_length + doip_packet.payload_length_)) {
                throw std::runtime_error("Read DoIp Packet of unexpected size. ");
            } else {}
        } else {
            // May not have read more than the header
            if (bytesReceived > kDoIp_HeaderTotal_length) {
                throw std::runtime_error("Extra Payload bytes read for DoIp Packet. ");
            }
        }
    }
}

void DoIpCommunication::SocketWrite(const int socket, DoIpPacket& doip_packet, bool& is_socket_open, struct sockaddr_in* const destination_address)
{
    if ((socket < 0) || !is_socket_open) {
        is_socket_open = false;
        return;
    }

    DoIpPacket::ScatterArray scatter_array(doip_packet.GetScatterArray());
    // Use Message Header for a scattered send
    struct msghdr message_header = {};
    if (destination_address != nullptr) {
        message_header.msg_name = destination_address;
        message_header.msg_namelen = sizeof(*destination_address);
    } else {
        message_header.msg_name = NULL;
        message_header.msg_namelen = 0;
    }
    message_header.msg_iov = scatter_array.begin();
    message_header.msg_iovlen = scatter_array.size();
    message_header.msg_control = nullptr;
    message_header.msg_controllen = 0;
    message_header.msg_flags = 0;

    // Briefly switch DoIpPacket to network byte order
    doip_packet.Hton();
    const auto bytesSent = sendmsg(socket, &message_header, 0);
    doip_packet.Ntoh();
    if (bytesSent == 0) {
        is_socket_open = false;
        LOG_ERROR << __FUNCTION__ << "socket closed! ";
    } else if (bytesSent < 0) {
        // Check for error or socket closed
        is_socket_open = false;
        LOG_ERROR << __FUNCTION__ << "socket closed! errno: " << strerror(errno);
    } else {} // else we are happy.
}

bool DoIpCommunication::CheckGenericDoIPHeaderVersion(DoIpPacket& doip_message)
{
    if ((doip_message.GetProtocolVersion() + doip_message.GetInverseProtocolVersion()) != static_cast<uint8_t>(0xff)) {
        return false;
    }
    if ((DoIpPayloadType::kVehicleIdentificationRequest == doip_message.payload_type_)
     || (DoIpPayloadType::kVehicleIdentificationRequestWithEid == doip_message.payload_type_)
     || (DoIpPayloadType::kVehicleIdentificationRequestWithVin == doip_message.payload_type_)) {
        if ((doip_message.GetProtocolVersion() != kSupportedDoIpVersion)
         && (doip_message.GetProtocolVersion() != kDoIp_ProtocolVersion_max)) {
            return false;
        } else {
            return true;
        }
    } else {
        if (doip_message.GetProtocolVersion() != kSupportedDoIpVersion) {
            return false;
        } else {
            return true;
        }
    }
    //return true;
}

bool DoIpCommunication::CheckPayloadTypeSpecificLength(DoIpPacket& doip_message)
{
    if (doip_message.payload_type_ == DoIpPayloadType::kGenericDoIpNack) {
        if (doip_message.payload_length_ != kDoIp_GenericDoIpNack_length) {
            return false;
        } else {
            return true;
        }
    } else if (doip_message.payload_type_ == DoIpPayloadType::kVehicleIdentificationRequest) {
        if (doip_message.payload_length_ != kDoIp_VehicleIdentificationRequest_length) {
            return false;
        } else {
            return true;
        }
    } else if (doip_message.payload_type_ == DoIpPayloadType::kVehicleIdentificationRequestWithEid) {
        if (doip_message.payload_length_ != kDoIp_VehicleIdentificationRequestWithEid_length) {
            return false;
        } else {
            return true;
        }
    } else if (doip_message.payload_type_ == DoIpPayloadType::kVehicleIdentificationRequestWithVin) {
        if (doip_message.payload_length_ != kDoIp_VehicleIdentificationRequestWithVin_length) {
            return false;
        } else {
            return true;
        }
    } else if (doip_message.payload_type_ == DoIpPayloadType::kRoutingActivationRequest) {
        if ((doip_message.payload_length_ != kDoIp_RoutingActivationRequest_length_min)
         && (doip_message.payload_length_ != kDoIp_RoutingActivationRequest_length_max)) {
            return false;
        } else {
            return true;
        }
    } else if (doip_message.payload_type_ == DoIpPayloadType::kAliveCheckResponse) {
        if (doip_message.payload_length_ != kDoIp_AliveCheckResponse_length) {
            return false;
        } else {
            return true;
        }
    } else if (doip_message.payload_type_ == DoIpPayloadType::kDoIpEntityStatusRequest) {
        if (doip_message.payload_length_ != kDoIp_EntityStatusRequest_length) {
            return false;
        } else {
            return true;
        }
    } else if (doip_message.payload_type_ == DoIpPayloadType::kDiagnosticPowerModeInformationRequest) {
        if (doip_message.payload_length_ != kDoIp_PowerModeInformationRequest_length) {
            return false;
        } else {
            return true;
        }
    } else if (doip_message.payload_type_ == DoIpPayloadType::kDiagnosticMessage) {
        if (doip_message.payload_length_ >= kDoIp_DiagnosticMessage_length_min) {
            return true;
        } else {
            return false;
        }
    } else if (doip_message.payload_type_ == DoIpPayloadType::kVehicleAnnouncement) {
        if ((doip_message.payload_length_ != kDoIp_VehicleAnnouncement_length_min)
         && (doip_message.payload_length_ != kDoIp_VehicleAnnouncement_length_max)) {
            return false;
        } else {
            return true;
        }
    } else {
        return true;
    }
}

bool DoIpCommunication::CheckPayloadType(DoIpPacket& doip_message)
{
    if ((doip_message.payload_type_ == DoIpPayloadType::kGenericDoIpNack)
     || (doip_message.payload_type_ == DoIpPayloadType::kRoutingActivationResponse)
     || (doip_message.payload_type_ == DoIpPayloadType::kDiagnosticMessage)
     || (doip_message.payload_type_ == DoIpPayloadType::kDiagnosticAck)
     || (doip_message.payload_type_ == DoIpPayloadType::kDiagnosticNack)
     || (doip_message.payload_type_ == DoIpPayloadType::kCustomMessage)
     || (doip_message.payload_type_ == DoIpPayloadType::kCustomNack)
     || (doip_message.payload_type_ == DoIpPayloadType::kCustomCanTableSync)) {
        return true;
    } else {
        return false;
    }
}

}  // namespace connection
}  // namespace dcm
}  // namespace diag
}  // namespace asf
