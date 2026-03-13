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

#include "dcm/connection/do_ip_packet.h"

#include <netinet/in.h>
#include <sys/uio.h>

#include "uds_transport/uds_message_ext.h"
using asf::diag::uds_transport::UdsMessageExt;
#include "common/multi_byte_type.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

DoIpPacket::DoIpPacket(const DoIpPacket::ByteOrder byte_order, const uint16_t givenLogicalAddress)
    : byte_order_(byte_order)
    , logicalAddress_(givenLogicalAddress)
    , protocol_version_(0)
    , inv_protocol_version_(kDoIp_ProtocolVersion_max)
    , payload_type_(0)
    , payload_length_(0)
{}

void DoIpPacket::SetPayloadLength(const PayloadLength payload_length,const bool force)
{
    PayloadOwner::SetPayloadLength(payload_length, force);
    if (force || (payload_length != this->payload_length_)) {
        this->payload_length_ = payload_length;
    }
}

DoIpPacket::ScatterArray DoIpPacket::GetScatterArray()
{
    ScatterArray scatter_array{};

    scatter_array[kProtocolVersionIdx].iov_base = &protocol_version_;
    scatter_array[kProtocolVersionIdx].iov_len = kDoIp_ProtocolVersion_length;
    scatter_array[kInvProtocolVersionIdx].iov_base = &inv_protocol_version_;
    scatter_array[kInvProtocolVersionIdx].iov_len = kDoIp_InvProtocolVersion_length;
    scatter_array[kPayloadTypeIdx].iov_base = &payload_type_;
    scatter_array[kPayloadTypeIdx].iov_len = kDoIp_PayloadType_length;
    scatter_array[kPayloadLengthIdx].iov_base = &payload_length_;
    scatter_array[kPayloadLengthIdx].iov_len = kDoIp_PayloadLength_length;
    scatter_array[kPayloadIdx].iov_base = payload_.data();
    scatter_array[kPayloadIdx].iov_len = payload_.size();

    return scatter_array;
}

DoIpPacket DoIpPacket::ConstructRoutingActivationResponse(const char ack_code)
{
    DoIpPacket packet(this->byte_order_, this->logicalAddress_);
    packet.SetProtocolVersion(kSupportedDoIpVersion);
    // Ensure we are working on a host-byte-order representation
    packet.Ntoh();
    packet.SetPayloadType(DoIpPayloadType::kRoutingActivationResponse);
    packet.SetPayloadLength(9);
    packet.payload_[0] = payload_.at(0);  // Tester Address
    packet.payload_[1] = payload_.at(1);  // Tester Address
    packet.payload_[2] = static_cast<uint8_t>(this->logicalAddress_ >> 8);  // LOGICAL_ADDRESS_OF_DoIp_ENTITY
    packet.payload_[3] = static_cast<uint8_t>(this->logicalAddress_ & 0xff);  // LOGICAL_ADDRESS_OF_DoIp_ENTITY
    //packet.payload_[4] = (DoIpRoutingActivationResponseCodes::kRoutingActivationSuccessfullyActivated);
    packet.payload_[4] = (ack_code);
    packet.payload_[5] = (0x00);  // Reserved by ISO 13400
    packet.payload_[6] = (0x00);  // Reserved by ISO 13400
    packet.payload_[7] = (0x00);  // Reserved by ISO 13400
    packet.payload_[8] = (0x00);  // Reserved by ISO 13400

    return (packet);
}

//header wrong Nack
DoIpPacket DoIpPacket::ConstructNack(const char nack_code) const
{
    DoIpPacket packet{this->byte_order_, this->logicalAddress_};
    packet.SetProtocolVersion(kSupportedDoIpVersion);
    // Ensure we are working on a host-byte-order representation
    packet.Ntoh();
    packet.SetPayloadType(DoIpPayloadType::kGenericDoIpNack);
    packet.SetPayloadLength(1);
    packet.payload_[0] = nack_code;

    return packet;
}

DoIpPacket DoIpPacket::ConstructDiagNack(const char nack_code, const uint16_t Sa, const uint16_t Ta) const
{
    DoIpPacket packet{this->byte_order_, this->logicalAddress_};
    packet.SetProtocolVersion(kSupportedDoIpVersion);
    // Ensure we are working on a host-byte-order representation
    packet.Ntoh();
    packet.SetPayloadType(DoIpPayloadType::kDiagnosticNack);
    packet.SetPayloadLength(5);
    packet.payload_[0] = GetByte(Sa, 1);
    packet.payload_[1] = GetByte(Sa, 0);
    packet.payload_[2] = GetByte(Ta, 1);
    packet.payload_[3] = GetByte(Ta, 0);
    packet.payload_[4] = nack_code;

    return packet;
}

DoIpPacket DoIpPacket::ConstructDiagnosticAck(const UdsMessageExt& uds_message) const
{
    DoIpPacket packet(this->byte_order_, this->logicalAddress_);
    packet.SetProtocolVersion(kSupportedDoIpVersion);
    // Ensure we are working on a host-byte-order representation
    packet.Ntoh();
    packet.SetPayloadType(DoIpPayloadType::kDiagnosticAck);
    // ISO 13400-2:2012 Table-28
    packet.SetPayloadLength(5);
    packet.payload_[0] = GetByte(uds_message.GetTa(), 1);
    packet.payload_[1] = GetByte(uds_message.GetTa(), 0);
    packet.payload_[2] = GetByte(uds_message.GetSa(), 1);
    packet.payload_[3] = GetByte(uds_message.GetSa(), 0);
    packet.payload_[4] = DoIpAckCodes::Ack;

    return (packet);
}

DoIpPacket DoIpPacket::ConstructDiagnosticAck(const uint16_t Sa, const uint16_t Ta) const
{
    DoIpPacket packet(this->byte_order_, this->logicalAddress_);
    packet.SetProtocolVersion(kSupportedDoIpVersion);
    // Ensure we are working on a host-byte-order representation
    packet.Ntoh();
    packet.SetPayloadType(DoIpPayloadType::kDiagnosticAck);
    // ISO 13400-2:2012 Table-28
    packet.SetPayloadLength(5);
    packet.payload_[0] = GetByte(Sa, 1);
    packet.payload_[1] = GetByte(Sa, 0);
    packet.payload_[2] = GetByte(Ta, 1);
    packet.payload_[3] = GetByte(Ta, 0);
    packet.payload_[4] = DoIpAckCodes::Ack;

    return (packet);
}

void DoIpPacket::Hton()
{
    if (byte_order_ != kNetwork) {
        payload_type_ = htons(payload_type_);
        payload_length_ = htonl(payload_length_);
        byte_order_ = kNetwork;
    }
}

void DoIpPacket::Ntoh()
{
    if (byte_order_ != kHost) {
        payload_type_ = ntohs(payload_type_);
        payload_length_ = ntohl(payload_length_);
        byte_order_ = kHost;
    }
}

void DoIpPacket::SetPayloadType(const PayloadType type)
{
    payload_type_ = type;
}

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
