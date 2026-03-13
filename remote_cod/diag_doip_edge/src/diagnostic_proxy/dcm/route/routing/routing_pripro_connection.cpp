#include "dcm/route/routing/routing_pripro_connection.h"

#include "dcm/connection/do_ip_packet.h"
using asf::diag::dcm::connection::DoIpPayloadType;

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

RoutingPriproConnection::RoutingPriproConnection(const std::string& connection_name, const uint16_t logical_address)
    : RoutingConnection(connection_name)
    , logical_address_(logical_address)
    , connect_result_(false)
    , proxy_(nullptr)
{
}

void RoutingPriproConnection::Offer()
{
    proxy_ = std::make_shared<RoutingPriproCommunication>();
    if (proxy_ != nullptr)
    {
        proxy_->RegisterPriproMessageCallback([this](const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t> &payload)
                                              { ReceiveMessage(payload_type, source_address, target_address, payload); });
        proxy_->StartPriproCommunication();
    }
}

void RoutingPriproConnection::StopOffer()
{
    if (proxy_ != nullptr)
    {
        proxy_->StopPriproCommunication();
    }
}

bool RoutingPriproConnection::ConnectServer()
{
    if (!GetConnectResult())
    {
        if (!ConnectSocket())
        {
            return false;
        }
        SetConnectResult(true);
    }
    return true;
}

void RoutingPriproConnection::SetConnectResult(const bool result)
{
    connect_result_.store(result, std::memory_order_release);
}

bool RoutingPriproConnection::GetConnectResult()
{
    return connect_result_.load(std::memory_order_acquire);
}

bool RoutingPriproConnection::ConnectSocket()
{
    if (proxy_ != nullptr)
    {
        if (proxy_->ConnectPriproCommunication())
        {
            return true;
        }
    }
    return false;
}

bool RoutingPriproConnection::SendDiagnosticMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)
{
    LOG_ERROR << __FUNCTION__ << "source address: " << LOG_HEX(source_address) << "target address: " << LOG_HEX(target_address) << "payload size: " << payload.size() << "payload: " << LOG_RAW_BUFFER(payload);
    if (proxy_ != nullptr)
    {
        if (proxy_->SendPriproMessage(DoIpPayloadType::kDiagnosticMessage, logical_address_, target_address, payload))
        {
            LOG_ERROR << __FUNCTION__ << "success! ";
            return true;
        }
    }
    LOG_ERROR << __FUNCTION__ << "failed! ";
    return false;

}

bool RoutingPriproConnection::SendCustomMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)
{
    LOG_ERROR << __FUNCTION__ << "source address: " << LOG_HEX(source_address) << "target address: " << LOG_HEX(target_address) << "payload size: " << payload.size() << "payload: " << LOG_RAW_BUFFER(payload);
    if (proxy_ != nullptr)
    {
        if (proxy_->SendPriproMessage(DoIpPayloadType::kCustomMessage, logical_address_, target_address, payload))
        {
            LOG_ERROR << __FUNCTION__ << "success! ";
            return true;
        }
    }
    LOG_ERROR << __FUNCTION__ << "failed! ";
    return false;
}


void RoutingPriproConnection::ReceiveMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t> &payload)
{
    switch (payload_type)
    {
    case DoIpPayloadType::kDiagnosticMessage:
    case DoIpPayloadType::kDiagnosticAck:
    case DoIpPayloadType::kDiagnosticNack:
    {
        LOG_ERROR << __FUNCTION__ << "doip payload type: " << LOG_HEX(payload_type) << "[" << connection_name_ << "]";
        HandleDiagnosticMessage(payload_type, source_address, target_address, payload);
        break;
    }
    case DoIpPayloadType::kCustomMessage:
    case DoIpPayloadType::kCustomNack:
    case DoIpPayloadType::kCustomCanTableSync:
    {
        HandleCustomMessage(payload_type, source_address, target_address, payload);
        break;
    }
    case DoIpPayloadType::kGenericDoIpNack:
    {
        LOG_ERROR << __FUNCTION__ << "recv generic doip nack: " << payload.at(0) << "[" << connection_name_ << "]";
        break;
    }
    case DoIpPayloadType::kRoutingActivationResponse:
    {
        LOG_ERROR << __FUNCTION__ << "recv routing activation response. " << "[" << connection_name_ << "]";
        break;
    }
    default:
    {
        LOG_ERROR << __FUNCTION__ << "recv unknow type. type: " << LOG_HEX(payload_type) << "[" << connection_name_ << "]";
        break;
    }
    }
}

void RoutingPriproConnection::HandleDiagnosticMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)
{
    if (target_address != logical_address_)
    {
        LOG_ERROR << __FUNCTION__ << "target address error! target address: " << LOG_HEX(target_address) << "diagnostic address: " << LOG_HEX(logical_address_);
    }
    if (payload_type == DoIpPayloadType::kDiagnosticMessage)
    {
        LOG_ERROR << __FUNCTION__ << "source address: " << LOG_HEX(source_address) << "target address: " << LOG_HEX(target_address) << "payload size: " << payload.size() << "payload: " << LOG_RAW_BUFFER(payload);
    }
    HandleMessage(payload_type, source_address, target_address, payload);
}

void RoutingPriproConnection::HandleCustomMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)
{
    HandleMessage(payload_type, source_address, target_address, payload);
}

void RoutingPriproConnection::HandleMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)
{
    if (callback_ != nullptr)
    {
        callback_(payload_type, source_address, target_address, payload);
    }
}

}
}
}
}
