#include "dcm/route/routing/routing_pripro_transmision_protocol.h"

#include "dcm/connection/do_ip_packet.h"
using asf::diag::dcm::connection::DoIpPayloadType;

#include "common/multi_byte_type.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

RoutingPriproCommunication::RoutingPriproCommunication()
    : client_(DIAG_DOIP_REQUEST)
    , server_(DIAG_DOIP_RESPONSE)
    , mutex_()
    , callback_()
{
}

void RoutingPriproCommunication::StartPriproCommunication()
{
    server_.SetMessageCallback([this](std::string_view str)
                               { ReceivePriproMessage(str); });
    server_.Start();
}

void RoutingPriproCommunication::StopPriproCommunication()
{
    server_.Stop();
}

bool RoutingPriproCommunication::ConnectPriproCommunication()
{
    return client_.Connect();
}

bool RoutingPriproCommunication::TransmitPriproMessage(const std::vector<uint8_t>& message)
{
    if (client_.Connect())
    {
        if (client_.SendMsg(message))
        {
            return true;
        }
    }
    return false;
}

bool RoutingPriproCommunication::SendPriproMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& uds)
{
    const std::lock_guard<std::mutex> locker(mutex_);
    LOG_ERROR << __FUNCTION__ << "payload type: " << LOG_HEX(payload_type)
                              << "source address: " << LOG_HEX(source_address)
                              << "target address: " << LOG_HEX(target_address);
    const uint32_t payload_length = sizeof(uint16_t) * 2 + uds.size();
    std::vector<uint8_t> message{};
    message.push_back(connection::kSupportedDoIpVersion);
    message.push_back(~connection::kSupportedDoIpVersion);
    message.push_back(GetByte(payload_type, 1U));
    message.push_back(GetByte(payload_type, 0U));
    message.push_back(GetByte(payload_length, 3U));
    message.push_back(GetByte(payload_length, 2U));
    message.push_back(GetByte(payload_length, 1U));
    message.push_back(GetByte(payload_length, 0U));
    message.push_back(GetByte(source_address, 1U));
    message.push_back(GetByte(source_address, 0U));
    message.push_back(GetByte(target_address, 1U));
    message.push_back(GetByte(target_address, 0U));
    message.insert(message.cend(), uds.begin(), uds.end());
    if (TransmitPriproMessage(message))
    {
        LOG_ERROR << __FUNCTION__ << "success! ";
        return true;
    }
    LOG_ERROR << __FUNCTION__ << "failed! ";
    return false;
}

void RoutingPriproCommunication::ReceivePriproMessage(std::string_view str)
{
    LOG_ERROR << __FUNCTION__ << "size: " << str.size();
    const std::vector<std::uint8_t> message(str.begin(), str.end());
    if (message.size() < connection::kDoIp_HeaderTotal_length)
    {
        LOG_ERROR << __FUNCTION__ << "doip size error! size: " << message.size();
        return;
    }
    uint8_t version{0U};
    uint8_t inv_version{0U};
    SetByte(version, message.at(0), 0);
    SetByte(inv_version, message.at(1), 0);
    if (((version + inv_version) != static_cast<uint8_t>(0xff)) || (version != connection::kSupportedDoIpVersion))
    {
        LOG_ERROR << __FUNCTION__ << "recv doip header version error! " << LOG_HEX(version) << LOG_HEX(inv_version);
        return;
    }
    uint16_t payload_type{0U};
    SetByte(payload_type, message.at(2), 1);
    SetByte(payload_type, message.at(3), 0);
    if ((payload_type != DoIpPayloadType::kDiagnosticMessage)
     && (payload_type != DoIpPayloadType::kDiagnosticAck)
     && (payload_type != DoIpPayloadType::kDiagnosticNack))
    {
        LOG_ERROR << __FUNCTION__ << "recv doip payload type error! " << LOG_HEX(payload_type);
        return;
    }
    uint32_t payload_length{0U};
    SetByte(payload_length, message.at(4), 3);
    SetByte(payload_length, message.at(5), 2);
    SetByte(payload_length, message.at(6), 1);
    SetByte(payload_length, message.at(7), 0);
    if (payload_length < connection::kDoIp_DiagnosticMessage_length_min)
    {
        LOG_ERROR << __FUNCTION__ << "recv doip payload length error! " << LOG_HEX(payload_length);
        return;
    }
    uint16_t source_address{};
    SetByte(source_address, message.at(8), 1);
    SetByte(source_address, message.at(9), 0);
    uint16_t target_address{};
    SetByte(target_address, message.at(10), 1);
    SetByte(target_address, message.at(11), 0);
    std::vector<std::uint8_t> payload;
    payload.insert(payload.cend(), message.begin() + 12, message.end());
    HandlePriproMessage(payload_type, source_address, target_address, payload);
}

void RoutingPriproCommunication::HandlePriproMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& uds)
{
    if (callback_ != nullptr)
    {
        callback_(payload_type, source_address, target_address, uds);
    }
}

}
}
}
}
