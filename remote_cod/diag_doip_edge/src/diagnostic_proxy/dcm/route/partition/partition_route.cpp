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

#include "dcm/route/partition/partition_route.h"

#include "dcm/route/partition/partition_sync.h"

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

PartitionRoute::PartitionRoute(Routing& routing, const std::string& config, const uint8_t type)
    : Route(type)
    , routing_(routing)
    , config_(config)
    , processors_()
    , transmit_map_()
    , mutex_()
    , transmit_mutex_()
    , last_transmit_cid_(0U)
    , service_address_(0U)
{
}

void PartitionRoute::Initialize()
{
    service_address_ = routing_.GetServiceAddr();

    std::ifstream ifs(config_);
    if (ifs.good())
    {
        rapidjson::IStreamWrapper ist(ifs);
        rapidjson::Document doc;
        (void)doc.ParseStream(ist);

        const rapidjson::Value& partition = doc["RoutingPartitionTable"];
        for (uint16_t i = 0; i < partition.Size(); i++)
        {
            if ((partition[i].HasMember("Id") && partition[i]["Id"].IsInt())
             && (partition[i].HasMember("diagnosticAddress") && partition[i]["diagnosticAddress"].IsInt()))
            {
                const auto id = static_cast<uint8_t>(partition[i]["Id"].GetInt());
                const auto diagnostic_address = static_cast<uint16_t>(partition[i]["diagnosticAddress"].GetInt());
                PartitionProcessorInit(id, diagnostic_address);
            }
        }
    }
}

void PartitionRoute::Deinitialize()
{
    for (auto &i : processors_)
    {
        i.second->Deinitialize();
    }
    processors_.clear();
}

bool PartitionRoute::CheckAndHandleMessage(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto processor_id = message.GetProcessorId();
    const auto target_address = GetPartitionTa(processor_id);
    LOG_DEBUG << __FUNCTION__ << "target_address: " << LOG_HEX(target_address);
    if (message.GetTaType() == UdsMessage::TargetAddressType::kPhysical)
    {
#ifdef PARTITION_SYNC
        const auto sid = message.GetRequestSid().value_or(0);
        if (PartitionSync::GetInstance().IsPartitionSyncServer(sid))
        {
            PartitionRouteSendSyncMessage(message);
            return true;
        }
#endif
        SetLastTransmitConversationId(conversation_id);
        SetTransmitConversationId(target_address, conversation_id);
        LOG_ERROR << __FUNCTION__ << "conversation_id: " << conversation_id << "target_address: " << LOG_HEX(target_address);
    }
    else
    {
        if (message.ShouldSuppressPositiveResponse() == false)
        {
            SetLastTransmitConversationId(conversation_id);
            SetTransmitConversationId(target_address, conversation_id);
            LOG_ERROR << __FUNCTION__ << "conversation_id: " << conversation_id << "target_address: " << LOG_HEX(target_address);
        }
    }
    if (!PartitionRouteSendMessage(processor_id, message))
    {
        LOG_ERROR << __FUNCTION__ << "failed. ";
        HandleRoutingFinish(target_address);
        return false;
    }
    return true;
}

bool PartitionRoute::PartitionRouteSendMessage(const uint8_t processor_id, const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto processor = GetPartitionProcessor(processor_id);
    if (processor != nullptr)
    {
        if (processor->PartitionSendMessage(message))
        {
            return true;
        }
    }
    return false;
}

void PartitionRoute::PartitionRouteSendMessage(const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    for (auto &i : processors_)
    {
        if (!i.second->PartitionSendMessage(message))
        {
            LOG_ERROR << __FUNCTION__ << "failed. ";
        }
    }
}

void PartitionRoute::PartitionRouteSendSyncMessage(const UdsMessageExt& message)
{
    for (auto &i : processors_)
    {
        if (!i.second->PartitionSendSyncMessage(message))
        {
            LOG_ERROR << __FUNCTION__ << "failed. ";
        }
    }
}

void PartitionRoute::PartitionTransmitDiagnosticMsg(const uint8_t processor_id, const uint8_t conversation_id, const uint16_t source_address, const uint8_t sid, const uds_transport::ByteVector& payload)
{
    LOG_DEBUG << __FUNCTION__ << "processor_id: " << processor_id << "sid: " << LOG_HEX(sid);
#ifdef PARTITION_SYNC
    if (PartitionSync::GetInstance().IsPartitionSyncServer(sid))
    {
        PartitionSync::GetInstance().VerifyPartitionSync(sid, payload);
        return;
    }
#endif
    LOG_ERROR << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "conversation_id: " << conversation_id;
    routing_.TransmitDiagnosticMsg(conversation_id, service_address_, payload);
}

void PartitionRoute::PartitionTransmitDiagnosticAck(const uint8_t processor_id, const uint8_t conversation_id, const uint16_t source_address, const uint8_t sid)
{
    LOG_DEBUG << __FUNCTION__ << "processor_id: " << processor_id << "sid: " << LOG_HEX(sid);
#ifdef PARTITION_SYNC
    if (PartitionSync::GetInstance().IsPartitionSyncServer(sid) || PartitionSync::GetInstance().IsTesterPresentServer(sid))
    {
        return;
    }
#endif
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "conversation_id: " << conversation_id;
    routing_.TransmitDiagnosticAck(conversation_id, service_address_);
}

void PartitionRoute::PartitionTransmitDiagnosticNack(const uint8_t processor_id, const uint8_t conversation_id, const uint16_t source_address, const uint8_t sid, const uint8_t nack_code)
{
    LOG_DEBUG << __FUNCTION__ << "processor_id: " << processor_id << "sid: " << LOG_HEX(sid);
#ifdef PARTITION_SYNC
    if (PartitionSync::GetInstance().IsPartitionSyncServer(sid) || PartitionSync::GetInstance().IsTesterPresentServer(sid))
    {
        return;
    }
#endif
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "conversation_id: " << conversation_id;
    routing_.TransmitDiagnosticNack(conversation_id, service_address_, nack_code);
}

void PartitionRoute::PartitionProcessorInit(const uint8_t processor_id, const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "processor_id: " << processor_id << "processor_address: " << LOG_HEX(processor_address);
    CteatePartitionProcessor(processor_id);
    PartitionCreateRoutingTimer(processor_address);
    PartitionCreateArbitrateTimer(processor_address);
}

void PartitionRoute::PartitionCreateRoutingTimer(const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    auto fun = [this](const uint16_t address)
    {
        this->HandleRoutingFinish(address);
    };
    routing_.Subscribe(processor_address, fun);
}

void PartitionRoute::HandleRoutingFinish(const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    EraseTransmitConversationId(processor_address);
    PartitionArbitrateAddressFinish(processor_address);
}

uint8_t PartitionRoute::GetTransmitConversationId(const uint16_t processor_address)
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

void PartitionRoute::SetTransmitConversationId(const uint16_t target_address, const uint8_t conversation_id)
{
    const std::lock_guard<std::mutex> locker_tm(transmit_mutex_);
    transmit_map_[target_address] = conversation_id;
}

void PartitionRoute::EraseTransmitConversationId(const uint16_t processor_address)
{
    const std::lock_guard<std::mutex> locker_tm(transmit_mutex_);
    if (transmit_map_.find(processor_address) != transmit_map_.end())
    {
        (void)transmit_map_.erase(processor_address);
    }
}

void PartitionRoute::SetLastTransmitConversationId(const uint8_t conversation_id)
{
    LOG_ERROR << __FUNCTION__ << "last_conversation_id: " << conversation_id;
    last_transmit_cid_.store(conversation_id, std::memory_order_release);
}

uint8_t PartitionRoute::GetLastTransmitConversationId()
{
    return last_transmit_cid_.load(std::memory_order_acquire);
}

void PartitionRoute::PartitionCreateArbitrateTimer(const uint16_t processor_address)
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

void PartitionRoute::PartitionArbitrateAddressFinish(const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    if (routing_.GetConversationManager().GetArbitration().ArbitrationEnable())
    {
        routing_.GetConversationManager().GetArbitration().ArbitrateAddressFinish(processor_address);
    }
}

void PartitionRoute::HandleArbitrateMessage(const uint16_t processor_address, const uint8_t type, const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_DEBUG << __FUNCTION__ << "processor_address: " << LOG_HEX(processor_address);
    switch (type)
    {
    case ArbitrateMessageCtrl::kArbitratRequestSend:
    {
        PartitionRouteSendCacheMessage(conversation_id, message);
        break;
    }
    case ArbitrateMessageCtrl::kArbitratRequestPending:
    {
        PartitionRouteCacheMessagePending(conversation_id, message);
        break;
    }
    case ArbitrateMessageCtrl::kArbitratRequestBusy:
    {
        PartitionRouteDiscardCacheMessage(conversation_id, message);
        break;
    }
    default:
    {
        break;
    }
    }
}

void PartitionRoute::PartitionRouteSendCacheMessage(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_ERROR << __FUNCTION__ << "conversation_id: " << conversation_id;
    if (CheckAndHandleMessage(conversation_id, message) == false)
    {
        LOG_ERROR << __FUNCTION__ << "failed. ";
    }
}

void PartitionRoute::PartitionRouteCacheMessagePending(const uint8_t conversation_id, const UdsMessageExt& message)
{
    routing_.GetConversationManager().ResponsePending(conversation_id, message.GetTa(), message.GetRequestSid().value_or(0));
}

void PartitionRoute::PartitionRouteDiscardCacheMessage(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_ERROR << __FUNCTION__ << "Arbitrate discard cache message! kResourceTemporarilyNotAvailable. ";
    routing_.GetConversationManager().ResourceTemporarilyNotAvailable(conversation_id, message.GetTa(), message.GetRequestSid().value_or(0));
}

void PartitionRoute::PartitionRouteRoutingIsClosed(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_ERROR << __FUNCTION__ << "Can Partition routing is closed! kResourceTemporarilyNotAvailable. ";
    routing_.GetConversationManager().ResourceTemporarilyNotAvailable(conversation_id, message.GetTa(), message.GetRequestSid().value_or(0));
}

uint16_t PartitionRoute::GetPartitionTa(const uint8_t processor_id)
{
    uint16_t target_address{ 0 };
    const auto processor = GetPartitionProcessor(processor_id);
    if (processor != nullptr)
    {
        target_address = processor->PartitionTa();
    }
    return target_address;
}

void PartitionRoute::CteatePartitionProcessor(const uint8_t processor_id)
{
    const std::lock_guard<std::mutex> locker(mutex_);
    const uint16_t key{processor_id};
    if (processors_.find(key) == processors_.end())
    {
        const auto processor = std::make_shared<PartitionProcessor>(*this, config_, processor_id);
        if (processor != nullptr)
        {
            processor->Initialize();
            (void)processors_.insert(PartitionProcessors::value_type(key, processor));
        }
    }
}

PartitionProcessor::Ptr PartitionRoute::GetPartitionProcessor(const uint8_t processor_id)
{
    const std::lock_guard<std::mutex> locker(mutex_);
    const uint16_t key{processor_id};
    PartitionProcessors::iterator it(processors_.find(key));
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
