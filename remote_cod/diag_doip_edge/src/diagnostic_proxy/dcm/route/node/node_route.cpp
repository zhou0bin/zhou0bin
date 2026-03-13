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

#include "dcm/route/node/node_route.h"

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

NodeRoute::NodeRoute(Routing& routing, const std::string& config, const uint8_t type)
    : Route(type)
    , routing_(routing)
    , config_(config)
    , processors_()
    , transmit_map_()
    , mutex_()
    , transmit_mutex_()
    , last_transmit_cid_(0U)
{
}

void NodeRoute::Initialize()
{
    std::ifstream ifs(config_);
    if (ifs.good())
    {
        rapidjson::IStreamWrapper ist(ifs);
        rapidjson::Document doc;
        (void)doc.ParseStream(ist);

        const rapidjson::Value& node = doc["RoutingNodeTable"];
        for (uint16_t i = 0; i < node.Size(); i++)
        {
            if ((node[i].HasMember("Id") && node[i]["Id"].IsInt())
             && (node[i].HasMember("diagnosticAddress") && node[i]["diagnosticAddress"].IsInt()))
            {
                const auto id = static_cast<uint8_t>(node[i]["Id"].GetInt());
                const auto diagnostic_address = static_cast<uint16_t>(node[i]["diagnosticAddress"].GetInt());
                NodeProcessorInit(id, diagnostic_address);
            }
        }
    }
}

void NodeRoute::Deinitialize()
{
    for (auto &i : processors_)
    {
        i.second->Deinitialize();
    }
    processors_.clear();
}

bool NodeRoute::CheckAndHandleMessage(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto target_address = message.GetTa();
    LOG_DEBUG << __FUNCTION__ << "target_address: " << LOG_HEX(target_address);
    if (message.GetTaType() == UdsMessage::TargetAddressType::kPhysical)
    {
        const auto processor_id = message.GetProcessorId();
        SetLastTransmitConversationId(conversation_id);
        SetTransmitConversationId(target_address, conversation_id);
        LOG_ERROR << __FUNCTION__ << "conversation_id: " << conversation_id << "target_address: " << LOG_HEX(target_address);
        if (!NodeRouteSendMessage(processor_id, message))
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
        NodeRouteSendMessage(message);
    }
    return true;
}

bool NodeRoute::NodeRouteSendMessage(const uint8_t processor_id, const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto processor = GetNodeProcessor(processor_id);
    if (processor != nullptr)
    {
        if (processor->NodeSendMessage(message))
        {
            return true;
        }
    }
    return false;
}

void NodeRoute::NodeRouteSendMessage(const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    for (auto &i : processors_)
    {
        if (!i.second->NodeSendMessage(message))
        {
            LOG_ERROR << __FUNCTION__ << "failed. ";
        }
    }
}

void NodeRoute::NodeTransmitDiagnosticMsg(const uint8_t conversation_id, const uint16_t source_address, const ByteVector& payload)
{
    LOG_ERROR << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "conversation_id: " << conversation_id;
    routing_.TransmitDiagnosticMsg(conversation_id, source_address, payload);
}

void NodeRoute::NodeTransmitDiagnosticAck(const uint8_t conversation_id, const uint16_t source_address)
{
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "conversation_id: " << conversation_id;
    routing_.TransmitDiagnosticAck(conversation_id, source_address);
}

void NodeRoute::NodeTransmitDiagnosticNack(const uint8_t conversation_id, const uint16_t source_address, const uint8_t nack_code)
{
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "conversation_id: " << conversation_id;
    routing_.TransmitDiagnosticNack(conversation_id, source_address, nack_code);
}

void NodeRoute::NodeProcessorInit(const uint8_t processor_id, const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "processor_id: " << processor_id << "processor_address: " << LOG_HEX(processor_address);
    CteateNodeProcessor(processor_id);
    NodeCreateRoutingTimer(processor_address);
    NodeCreateArbitrateTimer(processor_address);
}

void NodeRoute::NodeCreateRoutingTimer(const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    auto fun = [this](const uint16_t address)
    {
        this->HandleRoutingFinish(address);
    };
    routing_.Subscribe(processor_address, fun);
}

void NodeRoute::HandleRoutingFinish(const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    EraseTransmitConversationId(processor_address);
    NodeArbitrateAddressFinish(processor_address);
}

uint8_t NodeRoute::GetTransmitConversationId(const uint16_t processor_address)
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

void NodeRoute::SetTransmitConversationId(const uint16_t processor_address, const uint8_t conversation_id)
{
    const std::lock_guard<std::mutex> locker_tm(transmit_mutex_);
    transmit_map_[processor_address] = conversation_id;
}

void NodeRoute::SetTransmitConversationId(const uint8_t conversation_id)
{
    const std::lock_guard<std::mutex> locker_tm(transmit_mutex_);
    for (auto &i : transmit_map_)
    {
        i.second = conversation_id;
    }
}

void NodeRoute::EraseTransmitConversationId(const uint16_t processor_address)
{
    const std::lock_guard<std::mutex> locker_tm(transmit_mutex_);
    if (transmit_map_.find(processor_address) != transmit_map_.end())
    {
        (void)transmit_map_.erase(processor_address);
    }
}

void NodeRoute::SetLastTransmitConversationId(const uint8_t conversation_id)
{
    LOG_ERROR << __FUNCTION__ << "last_conversation_id: " << conversation_id;
    last_transmit_cid_.store(conversation_id, std::memory_order_release);
}

uint8_t NodeRoute::GetLastTransmitConversationId()
{
    return last_transmit_cid_.load(std::memory_order_acquire);
}

void NodeRoute::NodeCreateArbitrateTimer(const uint16_t processor_address)
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

void NodeRoute::NodeArbitrateAddressFinish(const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    if (routing_.GetConversationManager().GetArbitration().ArbitrationEnable())
    {
        routing_.GetConversationManager().GetArbitration().ArbitrateAddressFinish(processor_address);
    }
}

void NodeRoute::HandleArbitrateMessage(const uint16_t processor_address, const uint8_t type, const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    switch (type)
    {
    case ArbitrateMessageCtrl::kArbitratRequestSend:
    {
        NodeRouteSendCacheMessage(conversation_id, message);
        break;
    }
    case ArbitrateMessageCtrl::kArbitratRequestPending:
    {
        NodeRouteCacheMessagePending(conversation_id, message);
        break;
    }
    case ArbitrateMessageCtrl::kArbitratRequestBusy:
    {
        NodeRouteDiscardCacheMessage(conversation_id, message);
        break;
    }
    default:
    {
        break;
    }
    }
}

void NodeRoute::NodeRouteSendCacheMessage(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_ERROR << __FUNCTION__ << "conversation_id: " << conversation_id;
    if (CheckAndHandleMessage(conversation_id, message) == false)
    {
        LOG_ERROR << __FUNCTION__ << "failed. ";
    }
}

void NodeRoute::NodeRouteCacheMessagePending(const uint8_t conversation_id, const UdsMessageExt& message)
{
    routing_.GetConversationManager().ResponsePending(conversation_id, message.GetTa(), message.GetRequestSid().value_or(0));
}

void NodeRoute::NodeRouteDiscardCacheMessage(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_ERROR << __FUNCTION__ << "Arbitrate discard cache message! kResourceTemporarilyNotAvailable. ";
    routing_.GetConversationManager().ResourceTemporarilyNotAvailable(conversation_id, message.GetTa(), message.GetRequestSid().value_or(0));
}

void NodeRoute::CteateNodeProcessor(const uint8_t processor_id)
{
    const std::lock_guard<std::mutex> locker(mutex_);
    const uint8_t key{processor_id};
    if (processors_.find(key) == processors_.end())
    {
        const auto processor = std::make_shared<NodeProcessor>(*this, config_, processor_id);
        if (processor != nullptr)
        {
            processor->Initialize();
            (void)processors_.insert(NodeProcessors::value_type(key, processor));
        }
    }
}

NodeProcessor::Ptr NodeRoute::GetNodeProcessor(const uint8_t processor_id)
{
    const std::lock_guard<std::mutex> locker(mutex_);
    const uint8_t key{processor_id};
    NodeProcessors::iterator it(processors_.find(key));
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
