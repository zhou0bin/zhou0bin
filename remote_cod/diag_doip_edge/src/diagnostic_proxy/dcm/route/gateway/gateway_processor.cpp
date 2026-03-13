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

#include "dcm/route/gateway/gateway_processor.h"

#include "dcm/route/gateway/gateway_route.h"

#include "dcm/route/routing.h"

#include "dcm/connection/do_ip_packet.h"
using asf::diag::dcm::connection::DoIpPayloadType;

#include "uds_transport/uds_message_ext.h"
using asf::diag::uds_transport::UdsMessageExt;
using asf::diag::uds_transport::UdsMessage;
using asf::diag::uds_transport::ByteVector;

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

GatewayProcessor::GatewayProcessor(GatewayRoute& route, const std::string& config, const uint16_t id)
    : route_(route)
    , config_(config)
    , id_(id)
    , logical_address_(0U)
    , functional_address_(0U)
    , proxy_(nullptr)
{
}

void GatewayProcessor::Initialize()
{
    logical_address_ = route_.GetRouting().GetLogicalAddr();
    functional_address_ = route_.GetRouting().GetFunctionalAddr();

    std::ifstream ifs(config_);
    if (ifs.good())
    {
        rapidjson::IStreamWrapper ist(ifs);
        rapidjson::Document doc;
        (void)doc.ParseStream(ist);

        const rapidjson::Value& gateway = doc["RoutingGatewayTable"];
        for (uint16_t i = 0; i < gateway.Size(); i++)
        {
            if ((gateway[i].HasMember("Id") && gateway[i]["Id"].IsInt())
             && (gateway[i].HasMember("Name") && gateway[i]["Name"].IsString())
             && (gateway[i].HasMember("remote_ip") && gateway[i]["remote_ip"].IsString())
             && (gateway[i].HasMember("remote_port") && gateway[i]["remote_port"].IsInt())
             && (gateway[i].HasMember("port") && gateway[i]["port"].IsInt()))
            {
                const uint16_t id = static_cast<uint16_t>(gateway[i]["Id"].GetInt());
                if (id == id_)
                {
                    const auto name = gateway[i]["Name"].GetString();
                    const auto remote_ip = gateway[i]["remote_ip"].GetString();
                    const auto remote_port = static_cast<uint16_t>(gateway[i]["remote_port"].GetInt());
                    const auto port = static_cast<uint16_t>(gateway[i]["port"].GetInt());
                    bool routing_activation {false};
                    if (remote_port == 13400)
                    {
                        routing_activation = true;
                    }
                    proxy_ = RoutingConnectionFactory::CreateConnection(name, remote_ip, remote_port, port, 0U, functional_address_, logical_address_, routing_activation);
                    if (proxy_ != nullptr)
                    {
                        proxy_->RegisterMessageCallback([this](const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const ByteVector& payload)
                                                        { HandleMessage(payload_type, source_address, target_address, payload); });
                        proxy_->Offer();
                    }
                }
            }
        }
    }
}

void GatewayProcessor::Deinitialize()
{
    if (proxy_ != nullptr)
    {
        proxy_->StopOffer();
    }
}

bool GatewayProcessor::GatewaySendMessage(const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const std::lock_guard<std::mutex> locker(mutex_);
    if (proxy_ != nullptr)
    {
        if (proxy_->ConnectServer())
        {
            GatewayStartReceving(message);
            if (proxy_->SendDiagnosticMessage(message.GetSa(), message.GetTa(), message.GetPayload()))
            {
                return true;
            }
            GatewayStopReceving(message);
        }
    }
    return false;
}

bool GatewayProcessor::GatewaySendCustomMessage(const ByteVector& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const std::lock_guard<std::mutex> locker(mutex_);
    if (proxy_ != nullptr)
    {
        if (proxy_->ConnectServer())
        {
            if (proxy_->SendCustomMessage(logical_address_, functional_address_, message))
            {
                return true;
            }
        }
    }
    return false;
}

void GatewayProcessor::HandleMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const ByteVector& payload)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (payload.size() < 1)
    {
        LOG_ERROR << __FUNCTION__ << "payload size error! " << "size: " << payload.size();
        return;
    }
    switch (payload_type)
    {
    case DoIpPayloadType::kDiagnosticMessage:
    {
        const uint8_t conversation_id = route_.GetTransmitConversationId(source_address);
        uint8_t sid{ 0 };
        if ((payload.size() == 3) && (payload.at(0) == 0x7F))
        {
            sid = payload.at(1);
            if (payload.at(2) == 0x78)
            {
                GatewayCheckResponsePending(source_address, target_address, sid);
            }
            else
            {
                GatewayCheckDiagnosticMsg(source_address, target_address, sid);
            }
        }
        else
        {
            sid = payload.at(0) - 0x40;
            GatewayCheckDiagnosticMsg(source_address, target_address, sid);
        }
        route_.GatewayTransmitDiagnosticMsg(conversation_id, source_address, payload);
        break;
    }
    case DoIpPayloadType::kDiagnosticAck:
    {
        const uint8_t conversation_id = route_.GetTransmitConversationId(source_address);
        GatewayCheckDiagnosticAck(source_address, target_address);
        route_.GatewayTransmitDiagnosticAck(conversation_id, source_address);
        break;
    }
    case DoIpPayloadType::kDiagnosticNack:
    {
        const uint8_t conversation_id = route_.GetTransmitConversationId(source_address);
        GatewayCheckDiagnosticNack(source_address, target_address);
        const uint8_t nack_code = payload.at(0);
        route_.GatewayTransmitDiagnosticNack(conversation_id, source_address, nack_code);
        break;
    }
    case DoIpPayloadType::kCustomMessage:
    {
        LOG_DEBUG << __FUNCTION__ << "id: " << id_ << "state: " << LOG_HEX(payload.at(0));
        route_.GetewayRoutingStateChange((payload.at(0) == 0x01));
        break;
    }
#ifdef GATEWAY_SYNC
    case DoIpPayloadType::kCustomCanTableSync:
    {
        LOG_DEBUG << __FUNCTION__ << "id: " << id_ << "gateway_id: " << payload.at(0);
        route_.GatewayRoutingUpdated(payload);
        break;
    }
#endif
    default:
    {
        LOG_ERROR << __FUNCTION__ << "payload_type: " << LOG_HEX(payload_type);
        break;
    }
    }
}

void GatewayProcessor::GatewayStartReceving(const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (message.GetTaType() == UdsMessage::TargetAddressType::kPhysical)
    {
        const auto target_address = message.GetTa();
        const auto sid = message.GetRequestSid().value_or(0);
        const auto suppress = message.ShouldSuppressPositiveResponse();
        LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(target_address) << LOG_HEX(sid) << suppress;
        const auto routing_timer = route_.GetRouting().GetRoutingTimer(target_address);
        if (routing_timer != nullptr)
        {
            routing_timer->StartReceving(sid, suppress);
        }
    }
}

void GatewayProcessor::GatewayStopReceving(const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (message.GetTaType() == UdsMessage::TargetAddressType::kPhysical)
    {
        const auto target_address = message.GetTa();
        const auto sid = message.GetRequestSid().value_or(0);
        LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(target_address) << LOG_HEX(sid);
        const auto routing_timer = route_.GetRouting().GetRoutingTimer(target_address);
        if (routing_timer != nullptr)
        {
            routing_timer->StopReceving(sid);
        }
    }
}

void GatewayProcessor::GatewayCheckDiagnosticAck(const uint16_t source_address, const uint16_t target_address)
{
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    if (source_address != functional_address_)
    {
        const auto routing_timer = route_.GetRouting().GetRoutingTimer(source_address);
        if (routing_timer != nullptr)
        {
            routing_timer->CheckDiagnosticAck();
        }
    }
}

void GatewayProcessor::GatewayCheckDiagnosticNack(const uint16_t source_address, const uint16_t target_address)
{
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    if (source_address != functional_address_)
    {
        const auto routing_timer = route_.GetRouting().GetRoutingTimer(source_address);
        if (routing_timer != nullptr)
        {
            routing_timer->CheckDiagnosticNack();
        }
    }
}

void GatewayProcessor::GatewayCheckDiagnosticMsg(const uint16_t source_address, const uint16_t target_address, const uint8_t sid)
{
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    if (source_address != functional_address_)
    {
        const auto routing_timer = route_.GetRouting().GetRoutingTimer(source_address);
        if (routing_timer != nullptr)
        {
            routing_timer->CheckDiagnosticMsg(sid);
        }
    }
}

void GatewayProcessor::GatewayCheckResponsePending(const uint16_t source_address, const uint16_t target_address, const uint8_t sid)
{
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    if (source_address != functional_address_)
    {
        const auto routing_timer = route_.GetRouting().GetRoutingTimer(source_address);
        if (routing_timer != nullptr)
        {
            routing_timer->CheckResponsePending(sid);
        }
    }
}

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
