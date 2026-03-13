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

#include "dcm/route/routing.h"

#include "dcm/route/partition/partition_sync.h"

#include "dcm/conversation/conversation_manager.h"

#include "asf/diag/uds_transport/protocol_types.h"
using asf::diag::uds_transport::ByteVector;
#include "uds_transport/uds_message_ext.h"
using asf::diag::uds_transport::UdsMessage;
using asf::diag::uds_transport::UdsMessageExt;
using asf::diag::uds_transport::UdsMessageExtPtr;

#include "dcm/connection/uds_services.h"
using asf::diag::dcm::connection::UdsServices;

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

Routing::Routing(conversation::ConversationManager& conversation_manager, const std::string& config)
    : conversation_manager_(conversation_manager)
    , config_(config)
    , routing_table_()
    , logical_address_(0U)
    , service_address_(0U)
    , functional_address_(0U)
    , p2_(50)
    , p2_star_(2000)
    , transmission_delay_(200)
    , exit_requested_(false)
    , mutex_()
    , condition_variable_()
    , queue_()
{
    thread_ = std::thread([this]() { RoutingMessageQueue(); });
}

void Routing::Initialize()
{
    for (const auto& it : kRouteSet)
    {
        const auto route_ptr = RouteFactory::CreateRoute(*this, config_, it);
        if (route_ptr != nullptr)
        {
            routing_table_.AddRoute(it, route_ptr);
        }
    }

    std::ifstream ifs(config_);
    if (ifs.good())
    {
        rapidjson::IStreamWrapper ist(ifs);
        rapidjson::Document doc;
        (void)doc.ParseStream(ist);

        const rapidjson::Value& gateway_routing_table = doc["RoutingTable"];
        for (uint16_t i = 0; i < gateway_routing_table.Size(); i++)
        {
            if ((gateway_routing_table[i].HasMember("diagnosticAddress") && gateway_routing_table[i]["diagnosticAddress"].IsInt())
             && (gateway_routing_table[i].HasMember("gateway") && gateway_routing_table[i]["gateway"].IsInt()))
            {
                const auto diagnostic_address = static_cast<uint16_t>(gateway_routing_table[i]["diagnosticAddress"].GetInt());
                const auto processor_id = static_cast<uint8_t>(gateway_routing_table[i]["gateway"].GetInt());
                routing_table_.AddGateway(diagnostic_address, processor_id);
            }
        }

        const rapidjson::Value& routing_node_table = doc["RoutingNodeTable"];
        for (uint16_t i = 0; i < routing_node_table.Size(); i++)
        {
            if ((routing_node_table[i].HasMember("Id") && routing_node_table[i]["Id"].IsInt())
             && (routing_node_table[i].HasMember("diagnosticAddress") && routing_node_table[i]["diagnosticAddress"].IsInt()))
            {
                const auto processor_id = static_cast<uint8_t>(routing_node_table[i]["Id"].GetInt());
                const auto diagnostic_address = static_cast<uint16_t>(routing_node_table[i]["diagnosticAddress"].GetInt());
                routing_table_.AddNode(diagnostic_address, processor_id);
            }
        }

        const rapidjson::Value& routing_partition_table = doc["RoutingPartitionTable"];
        for (uint16_t i = 0; i < routing_partition_table.Size(); i++)
        {
            if ((routing_partition_table[i].HasMember("Id") && routing_partition_table[i]["Id"].IsInt())
             && (routing_partition_table[i].HasMember("diagnosticAddress") && routing_partition_table[i]["diagnosticAddress"].IsInt()))
            {
                const auto processor_id = static_cast<uint8_t>(routing_partition_table[i]["Id"].GetInt());
                const auto diagnostic_address = static_cast<uint16_t>(routing_partition_table[i]["diagnosticAddress"].GetInt());
                routing_table_.AddPartition(diagnostic_address, processor_id);
            }
        }

        const rapidjson::Value& service_routing_table_by_did_to_read_did = doc["ServiceRoutingTableByDataIdToReadDid"];
        for (uint16_t i = 0; i < service_routing_table_by_did_to_read_did.Size(); i++)
        {
            if ((service_routing_table_by_did_to_read_did[i].HasMember("DID") && service_routing_table_by_did_to_read_did[i]["DID"].IsInt())
             && (service_routing_table_by_did_to_read_did[i].HasMember("partition") && service_routing_table_by_did_to_read_did[i]["partition"].IsInt()))
            {
                const auto did = static_cast<uint16_t>(service_routing_table_by_did_to_read_did[i]["DID"].GetInt());
                const auto processor_id = static_cast<uint8_t>(service_routing_table_by_did_to_read_did[i]["partition"].GetInt());
                routing_table_.AddServiceRoutingTableByDidToReadDid(did, processor_id);
            }
        }

        const rapidjson::Value& service_routing_table_by_did_to_write_did = doc["ServiceRoutingTableByDataIdToWriteDid"];
        for (uint16_t i = 0; i < service_routing_table_by_did_to_write_did.Size(); i++)
        {
            if ((service_routing_table_by_did_to_write_did[i].HasMember("DID") && service_routing_table_by_did_to_write_did[i]["DID"].IsInt())
             && (service_routing_table_by_did_to_write_did[i].HasMember("partition") && service_routing_table_by_did_to_write_did[i]["partition"].IsInt()))
            {
                const auto did = static_cast<uint16_t>(service_routing_table_by_did_to_write_did[i]["DID"].GetInt());
                const auto processor_id = static_cast<uint8_t>(service_routing_table_by_did_to_write_did[i]["partition"].GetInt());
                routing_table_.AddServiceRoutingTableByDidToWriteDid(did, processor_id);
            }
        }

        const rapidjson::Value& service_routing_table_by_rid = doc["ServiceRoutingTableByRoutineId"];
        for (uint16_t i = 0; i < service_routing_table_by_rid.Size(); i++)
        {
            if ((service_routing_table_by_rid[i].HasMember("RID") && service_routing_table_by_rid[i]["RID"].IsInt())
             && (service_routing_table_by_rid[i].HasMember("partition") && service_routing_table_by_rid[i]["partition"].IsInt()))
            {
                const auto rid = static_cast<uint16_t>(service_routing_table_by_rid[i]["RID"].GetInt());
                const auto processor_id = static_cast<uint8_t>(service_routing_table_by_rid[i]["partition"].GetInt());
                routing_table_.AddServiceRoutingTableByRid(rid, processor_id);
            }
        }

        const rapidjson::Value& service_routing_table_by_sid = doc["ServiceRoutingTableByServiceId"];
        for (uint16_t i = 0; i < service_routing_table_by_sid.Size(); i++)
        {
            if ((service_routing_table_by_sid[i].HasMember("SID") && service_routing_table_by_sid[i]["SID"].IsInt())
             && (service_routing_table_by_sid[i].HasMember("partition") && service_routing_table_by_sid[i]["partition"].IsInt()))
            {
                const auto sid = static_cast<uint8_t>(service_routing_table_by_sid[i]["SID"].GetInt());
                const auto processor_id = static_cast<uint8_t>(service_routing_table_by_sid[i]["partition"].GetInt());
                routing_table_.AddServiceRoutingTableBySid(sid, processor_id);
            }
        }
    
        const rapidjson::Value& edge_node = doc["EdgeNode"];
        if (edge_node.HasMember("diagnosticAddress") && edge_node["diagnosticAddress"].IsInt())
        {
            logical_address_ = static_cast<uint16_t>(edge_node["diagnosticAddress"].GetInt());
        }

        const rapidjson::Value &ecu_instance = doc["EcuInstance"];
        if (ecu_instance.HasMember("diagnosticAddress") && ecu_instance["diagnosticAddress"].IsInt())
        {
            service_address_ = static_cast<uint16_t>(ecu_instance["diagnosticAddress"].GetInt());
            functional_address_ = static_cast<uint16_t>(ecu_instance["functionalAddress"].GetInt());
        }

        const rapidjson::Value& timings = doc["P2Timings"];
        if ((timings.HasMember("p2") && timings["p2"].IsInt())
         && (timings.HasMember("p2_star") && timings["p2_star"].IsInt())
         && (timings.HasMember("transmission_delay") && timings["transmission_delay"].IsInt()))
        {
            p2_ = timings["p2"].GetInt();
            p2_star_ = timings["p2_star"].GetInt();
            transmission_delay_ = timings["transmission_delay"].GetInt();
        }
    }

    for (const auto& it : kRouteSet)
    {
        const auto ptr = routing_table_.GetRoute(it);
        if (ptr != nullptr)
        {
            ptr->Initialize();
        }
    }

#ifdef PARTITION_SYNC
    PartitionSync::GetInstance().RegisterPartitionSyncRequestCallback([this](const uint8_t conversation_id, const uint8_t processor_id, const UdsMessageExt &message)
                                                                      { SendPartitionSyncMessage(conversation_id, processor_id, message); });
    PartitionSync::GetInstance().RegisterPartitionSyncResponseCallback([this](const uint8_t conversation_id, const ByteVector &payload)
                                                                       { TransmitPartitionSyncMsg(conversation_id, payload); });
    PartitionSync::GetInstance().Initialize();
#endif
}

void Routing::Deinitialize()
{
    exit_requested_.store(true, std::memory_order_release);
    {
        const std::unique_lock<std::mutex> locker(mutex_);
        ClearRoutingMessageQueue();
    }
    condition_variable_.notify_all();
    if (thread_.joinable())
    {
        thread_.join();
    }
    for (const auto& it : kRouteSet)
    {
        const auto ptr = routing_table_.GetRoute(it);
        if (ptr != nullptr)
        {
            ptr->Deinitialize();
        }
    }
#ifdef PARTITION_SYNC
    PartitionSync::GetInstance().Deinitialize();
#endif
}

RoutingTable& Routing::GetRoutingTable()
{
    return routing_table_;
}

Routing::logicalAddr Routing::GetLogicalAddr()
{
    return logical_address_;
}

Routing::logicalAddr Routing::GetServiceAddr()
{
    return service_address_;
}

Routing::logicalAddr Routing::GetFunctionalAddr()
{
    return functional_address_;
}

Routing::ProcessorInfo Routing::GetProcessorInfo(const UdsMessageExt& message)
{
    const auto target_address = message.GetTa();
    LOG_DEBUG << __FUNCTION__ << "target address: " << LOG_HEX(target_address);

    uint8_t processor_type{0x01U};
    uint8_t processor_id{0x01U};
    uint16_t processor_address{target_address};
    if (message.GetTaType() == UdsMessage::TargetAddressType::kPhysical)
    {
        if (target_address == service_address_)
        {
            processor_type = RouteType::kPartition;
            processor_id = routing_table_.GetProcessorByServiceRoutingTable(message);
            const auto processors = routing_table_.GetPartitionMap();
            for (const auto& it : processors)
            {
                if (it.second == processor_id)
                {
                    processor_address = it.first;
                }
            }
            if (processor_id == 0U)
            {
                processor_type = RouteType::kGateway;
                processor_id = routing_table_.GetGatewayProcessorId(target_address);
            }
        }
        else if (routing_table_.IsGatewayAddress(target_address))
        {
            processor_type = RouteType::kGateway;
            processor_id = routing_table_.GetGatewayProcessorId(target_address);
        }
        else if (routing_table_.IsNodeAddress(target_address))
        {
            processor_type = RouteType::kNode;
            processor_id = routing_table_.GetNodeProcessorId(target_address);
        }
        else
        {
            LOG_ERROR << __FUNCTION__ << "target address: " << LOG_HEX(target_address) << "not found! ";
        }
    }
    else
    {
        processor_type = RouteType::kPartition;
        processor_id = routing_table_.GetProcessorByServiceRoutingTable(message);
        const auto processors = routing_table_.GetPartitionMap();
        for (const auto& it : processors)
        {
            if (it.second == processor_id)
            {
                processor_address = it.first;
            }
        }
        if (processor_id == 0U)
        {
            processor_type = RouteType::kGateway;
            processor_id = routing_table_.GetGatewayProcessorId(target_address);
        }
    }
    LOG_DEBUG << __FUNCTION__ << "type: " << static_cast<unsigned>(processor_type) << "id: " << static_cast<unsigned>(processor_id) << "address: " << LOG_HEX(processor_address);
    return {processor_type, processor_id, processor_address};
}

void Routing::RoutingMessage(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    {
        const std::unique_lock<std::mutex> locker(mutex_);
        AddRoutingMessage({conversation_id, message});
    }
    condition_variable_.notify_all();
}

void Routing::TransmitDiagnosticMsg(const uint8_t conversation_id, const uint16_t source_address, const uds_transport::ByteVector& payload)
{
    LOG_INFO << __FUNCTION__ << "in! ";
#ifdef PARTITION_SYNC
    if (source_address == service_address_)
    {
        uint8_t sid{ 0 };
        if ((payload.size() == 3) && (payload.at(0) == 0x7F))
        {
            sid = payload.at(1);
        }
        else
        {
            sid = payload.at(0) - 0x40;
        }
        if (PartitionSync::GetInstance().IsPartitionSyncServerPositiveResponse(sid, payload))
        {
            PartitionSync::GetInstance().StartPartitionSync(sid, payload);
            return;
        }
    }
#endif
#ifdef AUTH_CHECK_MESSAGE
    if (source_address == service_address_)
    {
        if ((payload.size() == 3) && (payload.at(0) == 0x7F))
        {
            if (payload.at(1) == UdsServices::kAuthentication)
            {
                conversation_manager_.GetAuthentication().SetAuthenticationResult(conversation_id, AuthenticationState::kFailure);
            }
        }
        else
        {
            if ((payload.size() > 1) && ((payload.at(0) - 0x40) == UdsServices::kAuthentication))
            {
                const uint8_t subfunction = payload.at(1);
                LOG_ERROR << __FUNCTION__ << "subfunction: " << LOG_HEX(subfunction);
                if (subfunction == 0x00)
                {
                    conversation_manager_.GetAuthentication().SetAuthenticationResult(conversation_id, AuthenticationState::kInitial);
                }
                else if (subfunction == 0x03)
                {
                    conversation_manager_.GetAuthentication().SetAuthenticationResult(conversation_id, AuthenticationState::kSuccess);
                }
                else
                {
                    // donothing
                }
            }
        }
    }
#endif
    const auto ptr = conversation_manager_.GetConversation(conversation_id);
    if (ptr != nullptr)
    {
        const auto target_address = ptr->GetSa();
        const auto channel_id = ptr->GetChannelId();
        const UdsMessageExt message = {source_address, target_address, payload};
        conversation_manager_.TransmitUdsMsg(message, channel_id);
    }
}

void Routing::TransmitDiagnosticAck(const uint8_t conversation_id, const uint16_t source_address)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto ptr = conversation_manager_.GetConversation(conversation_id);
    if (ptr != nullptr)
    {
        const auto target_address = ptr->GetSa();
        const auto channel_id = ptr->GetChannelId();
        conversation_manager_.TransmitDiagAck(source_address, target_address, channel_id);
    }
}

void Routing::TransmitDiagnosticNack(const uint8_t conversation_id, const uint16_t source_address, const uint8_t nack_code)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto ptr = conversation_manager_.GetConversation(conversation_id);
    if (ptr != nullptr)
    {
        const auto target_address = ptr->GetSa();
        const auto channel_id = ptr->GetChannelId();
        conversation_manager_.TransmitDiagNack(source_address, target_address, nack_code, channel_id);
    }
}

void Routing::RoutingMessageQueue()
{
    std::unique_lock<std::mutex> locker(mutex_);
    while (!exit_requested_.load(std::memory_order_acquire))
    {
        if (RoutingMessageQueueIsEmpty())
        {
            condition_variable_.wait(locker);
            continue;
        }
        if (!RoutingMessageQueueIsEmpty())
        {
            const auto message = GetRoutingMessage();
            const auto cid = std::get<0>(message);
            const auto msg = std::get<1>(message);
            locker.unlock();
            {
                HandleRoutingMessage(cid, msg);
            }
            locker.lock();
        }
    }
}

void Routing::HandleRoutingMessage(const uint8_t conversation_id, const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (message.GetTaType() == UdsMessage::TargetAddressType::kPhysical)
    {
        LOG_INFO << __FUNCTION__  << "Physical! ";
        const auto processor_type = message.GetProcessorType();
#ifdef PARTITION_SYNC
        const auto target_address = message.GetTa();
        const auto sid = message.GetRequestSid().value_or(0);
        if (processor_type == RouteType::kPartition)
        {
            PartitionSync::GetInstance().SendTesterPresentMessage(conversation_id, 0U, logical_address_, service_address_);
        }
        else
        {
            if (target_address == service_address_)
            {
                if (PartitionSync::GetInstance().IsPartitionSyncServer(sid))
                {
                    PartitionSync::GetInstance().AddPartitionSyncMessage(sid, conversation_id, message);
                }
                else
                {
                    PartitionSync::GetInstance().SendTesterPresentMessage(conversation_id, PartitionEnum::kPartitionDcm, logical_address_, service_address_);
                }
            }
        }
        if (SendRoutingMessage(processor_type, conversation_id, message) == false)
        {
            LOG_ERROR << __FUNCTION__ << "failed. ";
            if (target_address == service_address_)
            {
                if (PartitionSync::GetInstance().IsPartitionSyncServer(sid))
                {
                    PartitionSync::GetInstance().ErasePartitionSyncMessage(sid);
                }
            }
        }
#else
        if (SendRoutingMessage(processor_type, conversation_id, message) == false)
        {
            LOG_ERROR << __FUNCTION__ << "failed. ";
        }
#endif
    }
    else
    {
        LOG_INFO << __FUNCTION__  << "Functional! ";
        for (const auto& it : kRouteSet)
        {
            if (it == RouteType::kPartition)
            {
#ifdef PARTITION_SYNC
                PartitionSync::GetInstance().SendTesterPresentMessage(conversation_id, PartitionEnum::kPartitionDcm, logical_address_, service_address_);
#endif
                continue;
            }
            if (SendRoutingMessage(it, conversation_id, message) == false)
            {
                LOG_ERROR << __FUNCTION__ << "type: " << it << "failed. ";
            }
        }
    }
}

bool Routing::SendRoutingMessage(const uint8_t processor_type, const uint8_t conversation_id, const UdsMessageExt& message)
{
    const auto ptr = routing_table_.GetRoute(processor_type);
    if (ptr != nullptr)
    {
        if (ptr->CheckAndHandleMessage(conversation_id, message))
        {
            return true;
        }
    }
    return false;
}

void Routing::SendPartitionSyncMessage(const uint8_t conversation_id, const uint8_t processor_id, const UdsMessageExt& message)
{
    LOG_DEBUG << __FUNCTION__ << "conversation id: " << LOG_HEX(conversation_id) << "processor id: " << LOG_HEX(processor_id) << "sid: " << LOG_HEX(message.GetRequestSid().value_or(0));
    if (processor_id == 0U)
    {
        (void)SendRoutingMessage(RouteType::kGateway, conversation_id, message);
    }
    else
    {
        (void)SendRoutingMessage(RouteType::kPartition, conversation_id, message);
    }
}

void Routing::TransmitPartitionSyncMsg(const uint8_t conversation_id, const ByteVector& payload)
{
    const auto ptr = conversation_manager_.GetConversation(conversation_id);
    if (ptr != nullptr)
    {
        const auto source_address = service_address_;
        const auto target_address = ptr->GetSa();
        const auto channel_id = ptr->GetChannelId();
        const UdsMessageExt message = {source_address, target_address, payload};
        conversation_manager_.TransmitUdsMsg(message, channel_id);
    }
}

void Routing::ProcessorRecevingFinish(const uint16_t processor_address)
{
    LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(processor_address);
    HandleRecevingFinish(processor_address);
}

void Routing::ProcessorRecevingTimeout(const uint16_t processor_address)
{
    LOG_ERROR << __FUNCTION__ << "address: " << LOG_HEX(processor_address);
    HandleRecevingFinish(processor_address);
}

void Routing::Subscribe(const uint16_t processor_address, const RecevingFinishFunc fun)
{
    CreateRoutingTimer(processor_address);
    callback_map_[processor_address] = fun;
}

void Routing::HandleRecevingFinish(const uint16_t processor_address)
{
    if (callback_map_[processor_address] != nullptr)
    {
        callback_map_[processor_address](processor_address);
    }
}

void Routing::CreateRoutingTimer(const uint16_t processor_address)
{
    const std::lock_guard<std::mutex> locker_at(routing_timer_mutex_);
    uint16_t key{processor_address};
    RoutingTimerMap::iterator it(routing_timer_map_.find(key));
    if (it == routing_timer_map_.end()) {
        (void)routing_timer_map_.insert({key, std::make_shared<RoutingTimer>(*this, processor_address, p2_, p2_star_, transmission_delay_)});
    }
}

RoutingTimer::Ptr Routing::GetRoutingTimer(const uint16_t processor_address)
{
    const std::lock_guard<std::mutex> locker_at(routing_timer_mutex_);
    uint16_t key{processor_address};
    RoutingTimerMap::iterator it(routing_timer_map_.find(key));
    if (it != routing_timer_map_.end()) {
        return it->second;
    }
    return nullptr;
}

void Routing::AddRoutingMessage(RoutingMsg message)
{
    queue_.push(std::move(message));
}

Routing::RoutingMsg Routing::GetRoutingMessage()
{
    auto message = std::move(queue_.front());
    queue_.pop();
    return message;
}

void Routing::ClearRoutingMessageQueue()
{
    std::queue<RoutingMsg> empty;
    queue_.swap(empty);
}

bool Routing::RoutingMessageQueueIsEmpty()
{
    return queue_.empty();
}

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
