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

#include "dcm/route/gateway/gateway_route.h"

#include "dcm/route/routing.h"

#include "dcm/conversation/conversation_manager.h"

#include "uds_transport/uds_message_ext.h"
using asf::diag::uds_transport::UdsMessageExt;
using asf::diag::uds_transport::UdsMessage;
using asf::diag::uds_transport::ByteVector;

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

GatewayRoute::GatewayRoute(Routing& routing, const std::string& config, const uint8_t type)
    : Route(type)
    , routing_(routing)
    , config_(config)
    , processors_()
    , routing_map_()
    , transmit_map_()
    , mutex_()
    , routing_mutex_()
    , transmit_mutex_()
    , last_transmit_cid_(0U)
    , gateway_sync_(*this)
{
}

void GatewayRoute::Initialize()
{
    std::ifstream ifs(config_);
    if (ifs.good())
    {
        rapidjson::IStreamWrapper ist(ifs);
        rapidjson::Document doc;
        (void)doc.ParseStream(ist);

        const rapidjson::Value& gateway = doc["RoutingGatewayTable"];
        for (uint16_t i = 0; i < gateway.Size(); i++)
        {
            if (gateway[i].HasMember("Id") && gateway[i]["Id"].IsInt())
            {
                const auto id = static_cast<uint16_t>(gateway[i]["Id"].GetInt());
                GatewayProcessorInit(id);
            }
        }
    }
    GatewayRoutingInit();
}

void GatewayRoute::Deinitialize()
{
#ifdef GATEWAY_SYNC
    gateway_sync_.StopGatewaySync();
#endif
    for (auto &i : processors_)
    {
        i.second->Deinitialize();
    }
    processors_.clear();
}

bool GatewayRoute::CheckAndHandleMessage(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (GetRoutingState() == false)
    {
        GatewayRouteRoutingIsClosed(conversation_id, message);
        return false;
    }
#ifdef GATEWAY_SYNC
    if (!gateway_sync_.GetSyncResult())
    {
        LOG_ERROR << __FUNCTION__ << "Uninitialized! ";
    }
#endif
    const auto target_address = message.GetTa();
    LOG_DEBUG << __FUNCTION__ << "target_address: " << LOG_HEX(target_address);
    if (message.GetTaType() == UdsMessage::TargetAddressType::kPhysical)
    {
#ifdef GATEWAY_SYNC
        const auto processor_id = GetGatewayRoutingProcessorId(target_address);
#else
        const auto processor_id = message.GetProcessorId();
#endif
        SetLastTransmitConversationId(conversation_id);
        SetTransmitConversationId(target_address, conversation_id);
        LOG_ERROR << __FUNCTION__ << "conversation_id: " << conversation_id << "target_address: " << LOG_HEX(target_address);
        if (!GatewayRouteSendMessage(processor_id, message))
        {
            LOG_ERROR << __FUNCTION__ << "failed. ";
            HandleRoutingFinish(target_address);
            return false;
        }
    }
    else
    {
        if (message.ShouldSuppressPositiveResponse() == false)
        {
            SetLastTransmitConversationId(conversation_id);
            SetTransmitConversationId(conversation_id);  
        }
        LOG_ERROR << __FUNCTION__ << "conversation_id: " << conversation_id << "target_address: " << LOG_HEX(target_address);
        GatewayRouteSendMessage(message);
    }
    return true;
}

bool GatewayRoute::GatewayRouteSendMessage(const uint8_t processor_id, const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto processor = GetGatewayProcessor(processor_id);
    if (processor != nullptr)
    {
        if (processor->GatewaySendMessage(message))
        {
            return true;
        }
    }
    LOG_ERROR << __FUNCTION__ << "failed. ";
    return false;
}

void GatewayRoute::GatewayRouteSendMessage(const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    for (auto &i : processors_)
    {
        if (!i.second->GatewaySendMessage(message))
        {
            LOG_ERROR << __FUNCTION__ << "failed. ";
        }
    }
}

void GatewayRoute::GatewayRouteSendCustomMessage(const ByteVector& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto processor = GetGatewayProcessor(GatewayEnum::kGatewayM);
    if (processor != nullptr)
    {
        (void)processor->GatewaySendCustomMessage(message);
    }
}

void GatewayRoute::GatewayTransmitDiagnosticMsg(const uint8_t conversation_id, const uint16_t source_address, const ByteVector& payload)
{
    if (GetRoutingState() == false)
    {
        LOG_ERROR << __FUNCTION__ << "can node routing is closed! ";
        return;
    }
    LOG_ERROR << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "conversation_id: " << conversation_id;
    routing_.TransmitDiagnosticMsg(conversation_id, source_address, payload);
}

void GatewayRoute::GatewayTransmitDiagnosticAck(const uint8_t conversation_id, const uint16_t source_address)
{
    if (GetRoutingState() == false)
    {
        LOG_ERROR << __FUNCTION__ << "can node routing is closed! ";
        return;
    }
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "conversation_id: " << conversation_id;
    routing_.TransmitDiagnosticAck(conversation_id, source_address);
}

void GatewayRoute::GatewayTransmitDiagnosticNack(const uint8_t conversation_id, const uint16_t source_address, const uint8_t nack_code)
{
    if (GetRoutingState() == false)
    {
        LOG_ERROR << __FUNCTION__ << "can node routing is closed! ";
        return;
    }
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "conversation_id: " << conversation_id;
    routing_.TransmitDiagnosticNack(conversation_id, source_address, nack_code);
}

void GatewayRoute::GatewayProcessorInit(const uint8_t processor_id)
{
    CteateGatewayProcessor(processor_id);
#ifdef GATEWAY_SYNC
    gateway_sync_.AddGatewayProcessor(processor_id);
#endif
}

void GatewayRoute::GatewayRoutingInit()
{
    const auto routing_table = routing_.GetRoutingTable().GetGatewayMap();
    for (auto &i : routing_table)
    {
        const auto processor_address = i.first;
        const auto processor_id = i.second;
        GatewayRoutingUpdated(processor_address, processor_id);
    }
#ifdef GATEWAY_SYNC
    gateway_sync_.StartGatewaySync();
#endif
}

void GatewayRoute::GatewayRoutingUpdated(const uds_transport::ByteVector& payload)
{
    LOG_DEBUG << __FUNCTION__ << "payload size: " << payload.size();
#ifdef GATEWAY_SYNC
    gateway_sync_.GetewayRecvSyncMessage(payload);
#endif
}

void GatewayRoute::GatewayRoutingUpdated(const uint16_t processor_address, const uint8_t processor_id)
{
    LOG_DEBUG << __FUNCTION__ << "processor_id: " << processor_id << "processor_address: " << LOG_HEX(processor_address);
    SetGatewayRoutingProcessorId(processor_address, processor_id);
    GatewayCreateRoutingTimer(processor_address);
    GatewayCreateArbitrateTimer(processor_address);
}

void GatewayRoute::GatewayCreateRoutingTimer(const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    auto fun = [this](const uint16_t address)
    {
        this->HandleRoutingFinish(address);
    };
    routing_.Subscribe(processor_address, fun);
}

void GatewayRoute::HandleRoutingFinish(const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    EraseTransmitConversationId(processor_address);
    GatewayArbitrateAddressFinish(processor_address);
}

uint16_t GatewayRoute::GetGatewayRoutingProcessorId(const uint16_t processor_address)
{
    const std::lock_guard<std::mutex> locker_rm(routing_mutex_);
    if (routing_map_.find(processor_address) != routing_map_.end())
    {
        return routing_map_[processor_address];
    }
    return GatewayEnum::kGatewayM;
}

void GatewayRoute::SetGatewayRoutingProcessorId(const uint16_t processor_address, const uint8_t processor_id)
{
    const std::lock_guard<std::mutex> locker_rm(routing_mutex_);
    routing_map_[processor_address] = processor_id;
}

uint8_t GatewayRoute::GetTransmitConversationId(const uint16_t processor_address)
{
    uint8_t conversation_id = GetLastTransmitConversationId();
    {
        const std::lock_guard<std::mutex> locker_tm(transmit_mutex_);
        if (transmit_map_.find(processor_address) != transmit_map_.end())
        {
            conversation_id = transmit_map_[processor_address];
        }
    }
    return conversation_id;
}

void GatewayRoute::SetTransmitConversationId(const uint16_t processor_address, const uint8_t conversation_id)
{
    const std::lock_guard<std::mutex> locker_tm(transmit_mutex_);
    transmit_map_[processor_address] = conversation_id;
}

void GatewayRoute::SetTransmitConversationId(const uint8_t conversation_id)
{
    const std::lock_guard<std::mutex> locker_tm(transmit_mutex_);
    for (auto &i : transmit_map_)
    {
        i.second = conversation_id;
    }
}

void GatewayRoute::EraseTransmitConversationId(const uint16_t processor_address)
{
    const std::lock_guard<std::mutex> locker_tm(transmit_mutex_);
    if (transmit_map_.find(processor_address) != transmit_map_.end())
    {
        (void)transmit_map_.erase(processor_address);
    }
}

void GatewayRoute::SetLastTransmitConversationId(const uint8_t conversation_id)
{
    LOG_ERROR << __FUNCTION__ << "last_conversation_id: " << conversation_id;
    last_transmit_cid_.store(conversation_id, std::memory_order_release);
}

uint8_t GatewayRoute::GetLastTransmitConversationId()
{
    return last_transmit_cid_.load(std::memory_order_acquire);
}

void GatewayRoute::GatewayCreateArbitrateTimer(const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    if (routing_.GetConversationManager().GetArbitration().ArbitrationEnable())
    {
        auto fun = [this](const uint16_t address, const uint8_t type, const uint8_t conversation_id, const UdsMessageExt &message)
        {
            this->HandleArbitrateMessage(address, type, conversation_id, message);
        };
        routing_.GetConversationManager().GetArbitration().Subscribe(processor_address, fun);
    }
}

void GatewayRoute::GatewayArbitrateAddressFinish(const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    if (routing_.GetConversationManager().GetArbitration().ArbitrationEnable())
    {
        routing_.GetConversationManager().GetArbitration().ArbitrateAddressFinish(processor_address);
    }
}

void GatewayRoute::HandleArbitrateMessage(const uint16_t processor_address, const uint8_t type, const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    switch (type)
    {
    case ArbitrateMessageCtrl::kArbitratRequestSend:
    {
        GatewayRouteSendCacheMessage(conversation_id, message);
        break;
    }
    case ArbitrateMessageCtrl::kArbitratRequestPending:
    {
        GatewayRouteCacheMessagePending(conversation_id, message);
        break;
    }
    case ArbitrateMessageCtrl::kArbitratRequestBusy:
    {
        GatewayRouteDiscardCacheMessage(conversation_id, message);
        break;
    }
    default:
    {
        break;
    }
    }
}

void GatewayRoute::GatewayRouteSendCacheMessage(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (CheckAndHandleMessage(conversation_id, message) == false)
    {
        LOG_ERROR << __FUNCTION__ << "failed. ";
    }
}

void GatewayRoute::GatewayRouteCacheMessagePending(const uint8_t conversation_id, const UdsMessageExt& message)
{
    routing_.GetConversationManager().ResponsePending(conversation_id, message.GetTa(), message.GetRequestSid().value_or(0));
}

void GatewayRoute::GatewayRouteDiscardCacheMessage(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_ERROR << __FUNCTION__ << "Arbitrate discard cache message! kResourceTemporarilyNotAvailable. ";
    routing_.GetConversationManager().ResourceTemporarilyNotAvailable(conversation_id, message.GetTa(), message.GetRequestSid().value_or(0));
}

void GatewayRoute::GatewayRouteRoutingIsClosed(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_ERROR << __FUNCTION__ << "Can node routing is closed! kResourceTemporarilyNotAvailable. ";
    routing_.GetConversationManager().ResourceTemporarilyNotAvailable(conversation_id, message.GetTa(), message.GetRequestSid().value_or(0));
}

void GatewayRoute::GetewayRoutingStateChange(const bool state)
{
    LOG_ERROR << __FUNCTION__ << "can node routing state: " << state;
    SetRoutingState(state);
}

void GatewayRoute::CteateGatewayProcessor(const uint8_t processor_id)
{
    const std::lock_guard<std::mutex> locker(mutex_);
    const uint8_t key{processor_id};
    if (processors_.find(key) == processors_.end())
    {
        const auto processor = std::make_shared<GatewayProcessor>(*this, config_, processor_id);
        if (processor != nullptr)
        {
            processor->Initialize();
            (void)processors_.insert(GatewayProcessors::value_type(key, processor));
        }
    }
}

GatewayProcessor::Ptr GatewayRoute::GetGatewayProcessor(const uint8_t processor_id)
{
    const std::lock_guard<std::mutex> locker(mutex_);
    const uint8_t key{processor_id};
    GatewayProcessors::iterator it(processors_.find(key));
    if (it != processors_.end())
    {
        return it->second;
    }
    return nullptr;
}

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
