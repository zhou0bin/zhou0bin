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

#include "dcm/route/partition/partition_processor.h"

#include "dcm/route/partition/partition_route.h"

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

PartitionProcessor::PartitionProcessor(PartitionRoute& route, const std::string& config, const uint16_t id)
    : route_(route)
    , config_(config)
    , id_(id)
    , logical_address_(0U)
    , diagnostic_address_(0U)
    , functional_address_(0U)
    , proxy_(nullptr)
{
}

void PartitionProcessor::Initialize()
{
    logical_address_ = route_.GetRouting().GetLogicalAddr();

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
             && (partition[i].HasMember("Name") && partition[i]["Name"].IsString())
             && (partition[i].HasMember("remote_ip") && partition[i]["remote_ip"].IsString())
             && (partition[i].HasMember("remote_port") && partition[i]["remote_port"].IsInt())
             && (partition[i].HasMember("port") && partition[i]["port"].IsInt())
             && (partition[i].HasMember("diagnosticAddress") && partition[i]["diagnosticAddress"].IsInt())
             && (partition[i].HasMember("functionalAddress") && partition[i]["functionalAddress"].IsInt())
             && (partition[i].HasMember("routingActivation") && partition[i]["routingActivation"].IsBool()))
            {
                const uint16_t id = static_cast<uint16_t>(partition[i]["Id"].GetInt());
                if (id == id_)
                {
                    const auto name = partition[i]["Name"].GetString();
                    const auto remote_ip = partition[i]["remote_ip"].GetString();
                    const auto remote_port = static_cast<uint16_t>(partition[i]["remote_port"].GetInt());
                    const auto port = static_cast<uint16_t>(partition[i]["port"].GetInt());
                    const auto diagnostic_address = static_cast<uint16_t>(partition[i]["diagnosticAddress"].GetInt());
                    const auto functional_address = static_cast<uint16_t>(partition[i]["functionalAddress"].GetInt());
                    const auto routing_activation = partition[i]["routingActivation"].GetBool();
                    diagnostic_address_ = diagnostic_address;
                    functional_address_ = functional_address;
                    proxy_ = RoutingConnectionFactory::CreateConnection(name, remote_ip, remote_port, port, diagnostic_address_, functional_address_, logical_address_, routing_activation);
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

void PartitionProcessor::Deinitialize()
{
    if (proxy_ != nullptr)
    {
        proxy_->StopOffer();
    }
}

bool PartitionProcessor::PartitionSendMessage(const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const std::lock_guard<std::mutex> locker(mutex_);
    if (proxy_ != nullptr)
    {
        if (proxy_->ConnectServer())
        {
            auto target_address = diagnostic_address_;
            if (message.GetTaType() == UdsMessage::TargetAddressType::kFunctional)
            {
                target_address = functional_address_;
            }
            PartitionStartReceving(message);
            if (proxy_->SendDiagnosticMessage(message.GetSa(), target_address, message.GetPayload()))
            {
                return true;
            }
            PartitionStopReceving(message);
        }
    }
    return false;
}

bool PartitionProcessor::PartitionSendSyncMessage(const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const std::lock_guard<std::mutex> locker(mutex_);
    if (proxy_ != nullptr)
    {
        if (proxy_->ConnectServer())
        {
            auto target_address = diagnostic_address_;
            if (message.GetTaType() == UdsMessage::TargetAddressType::kFunctional)
            {
                target_address = functional_address_;
            }
            if (proxy_->SendDiagnosticMessage(message.GetSa(), target_address, message.GetPayload()))
            {
                return true;
            }
        }
    }
    return false;
}

void PartitionProcessor::HandleMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const ByteVector& payload)
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
                PartitionCheckResponsePending(source_address, target_address, sid);
            }
            else
            {
                PartitionCheckDiagnosticMsg(source_address, target_address, sid);
            }
        }
        else
        {
            sid = payload.at(0) - 0x40;
            PartitionCheckDiagnosticMsg(source_address, target_address, sid);
        }
        route_.PartitionTransmitDiagnosticMsg(id_, conversation_id, source_address, sid, payload);
        break;
    }
    case DoIpPayloadType::kDiagnosticAck:
    {
        const uint8_t conversation_id = route_.GetTransmitConversationId(source_address);
        PartitionCheckDiagnosticAck(source_address, target_address);
        uint8_t sid{ 0 };
        if (payload.size() == 2)
        {
            sid = payload.at(1);
        }
        route_.PartitionTransmitDiagnosticAck(id_, conversation_id, source_address, sid);
        break;
    }
    case DoIpPayloadType::kDiagnosticNack:
    {
        const uint8_t conversation_id = route_.GetTransmitConversationId(source_address);
        PartitionCheckDiagnosticNack(source_address, target_address);
        uint8_t sid{ 0 };
        if (payload.size() == 2)
        {
            sid = payload.at(1);
        }
        const uint8_t nack_code = payload.at(0);
        route_.PartitionTransmitDiagnosticNack(id_, conversation_id, source_address, sid, nack_code);
        break;
    }
    default:
    {
        LOG_ERROR << __FUNCTION__ << "payload_type: " << LOG_HEX(payload_type);
        break;
    }
    }
}

void PartitionProcessor::PartitionStartReceving(const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (message.GetTaType() == UdsMessage::TargetAddressType::kPhysical)
    {
        const auto sid = message.GetRequestSid().value_or(0);
        const auto suppress = message.ShouldSuppressPositiveResponse();
        LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(diagnostic_address_) << LOG_HEX(sid) << suppress;
        const auto routing_timer = route_.GetRouting().GetRoutingTimer(diagnostic_address_);
        if (routing_timer != nullptr)
        {
            routing_timer->StartReceving(sid, suppress);
        }
    }
}

void PartitionProcessor::PartitionStopReceving(const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (message.GetTaType() == UdsMessage::TargetAddressType::kPhysical)
    {
        const auto sid = message.GetRequestSid().value_or(0);
        LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(diagnostic_address_) << LOG_HEX(sid);
        const auto routing_timer = route_.GetRouting().GetRoutingTimer(diagnostic_address_);
        if (routing_timer != nullptr)
        {
            routing_timer->StopReceving(sid);
        }
    }
}

void PartitionProcessor::PartitionCheckDiagnosticAck(const uint16_t source_address, const uint16_t target_address)
{
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    if (source_address == diagnostic_address_)
    {
        const auto routing_timer = route_.GetRouting().GetRoutingTimer(diagnostic_address_);
        if (routing_timer != nullptr)
        {
            routing_timer->CheckDiagnosticAck();
        }
    }
}

void PartitionProcessor::PartitionCheckDiagnosticNack(const uint16_t source_address, const uint16_t target_address)
{
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    if (source_address == diagnostic_address_)
    {
        const auto routing_timer = route_.GetRouting().GetRoutingTimer(diagnostic_address_);
        if (routing_timer != nullptr)
        {
            routing_timer->CheckDiagnosticNack();
        }
    }
}

void PartitionProcessor::PartitionCheckDiagnosticMsg(const uint16_t source_address, const uint16_t target_address, const uint8_t sid)
{
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    if (source_address == diagnostic_address_)
    {
        const auto routing_timer = route_.GetRouting().GetRoutingTimer(diagnostic_address_);
        if (routing_timer != nullptr)
        {
            routing_timer->CheckDiagnosticMsg(sid);
        }
    }
}

void PartitionProcessor::PartitionCheckResponsePending(const uint16_t source_address, const uint16_t target_address, const uint8_t sid)
{
    LOG_DEBUG << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    if (source_address == diagnostic_address_)
    {
        const auto routing_timer = route_.GetRouting().GetRoutingTimer(diagnostic_address_);
        if (routing_timer != nullptr)
        {
            routing_timer->CheckResponsePending(sid);
        }
    }
}

uint16_t PartitionProcessor::PartitionTa()
{
    return diagnostic_address_;
}

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
