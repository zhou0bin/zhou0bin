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

#include "dcm/conversation/conversation.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <utility>

// #include "ara/core/optional.h"
#include <optional>

#include "common/multi_byte_type.h"
#include "log.h"

#include "dcm/conversation/conversation_manager.h"

#include "dcm/connection/connection.h"
#include "dcm/connection/do_ip_packet.h"
using asf::diag::dcm::connection::DoIpPacket;
#include "asf/diag/uds_transport/protocol_types.h"
using asf::diag::uds_transport::ByteVector;
#include "uds_transport/uds_message_ext.h"
using asf::diag::uds_transport::UdsMessage;
using asf::diag::uds_transport::UdsMessageExt;
using asf::diag::uds_transport::UdsMessageExtPtr;

namespace asf
{
namespace diag
{
namespace dcm
{
namespace conversation
{

Conversation::Conversation(const std::shared_ptr<ConversationManager> conversation_manager,
    const uds_transport::GlobalChannelIdentifier global_channel_id,
    const logicalAddr uds_sa,
    const logicalAddr uds_ta,
    const ConversationId conversation_id)
    : conversation_manager_(conversation_manager)
    , global_channel_id_(global_channel_id)
    , uds_sa_(uds_sa)
    , uds_ta_(uds_ta)
    , conversation_id_(conversation_id)
    , conversation_mutex_()
{
}

void Conversation::HandleMessage(UdsMessageExtPtr message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const std::lock_guard<std::recursive_mutex> locker{conversation_mutex_};
    const auto conversation_id = GetId();
    const auto source_address = message->GetSa();
    const auto target_address = message->GetTa();
    const auto sid = message->GetRequestSid().value_or(0);
    LOG_ERROR << __FUNCTION__ << "conversation_id: " << conversation_id << " sa: " << LOG_HEX(source_address);

    const auto service_address = GetConversationManager()->GetRouting().GetServiceAddr();
    LOG_DEBUG << __FUNCTION__ << " service_address: " << LOG_HEX(service_address);

    const auto tester_protocol = std::get<0>(message->GetRecvChannelID());
    const auto tester_type = GetConversationManager()->GetTesterTable().GetTesterType(tester_protocol, source_address);
    if (tester_type == TesterType::kUndefined)
    {
        LOG_ERROR << __FUNCTION__ << "Tester Address kUndefined! kResourceTemporarilyNotAvailable. ";
        GetConversationManager()->ResourceTemporarilyNotAvailable(conversation_id, target_address, sid);
        return;
    }
    LOG_DEBUG << __FUNCTION__ << "TesterType: " << tester_type << "(1: Local, 2: Virtual, 3: VirtualLocal)";

    uint8_t tester_priority = GetConversationManager()->GetTesterTable().GetTesterPriority(tester_type, source_address);

    // Diagnostic Authentication
    if (GetConversationManager()->GetAuthentication().AuthenticationEnable() == true)
    {
        uint8_t authenticate_result{AuthenticateResult::kAuthenticateInitial};
        if (tester_type == TesterType::kLocal)
        {
            if (GetConversationManager()->GetAuthentication().IsAuthenticationService(service_address, target_address, sid) == true)
            {
                (void)GetConversationManager()->RegChannelStateEvent(GetChannelId(), conversation_id);
                GetConversationManager()->GetAuthentication().StartAuthenticate(conversation_id);
#ifdef AUTH_DEAL_MESSAGE
                GetConversationManager()->GetAuthentication().RegisterAuthenticationResponseCallback([this, source_address, target_address](const ByteVector &payload)
                                                                                                     {
                                                                                                          const UdsMessageExt response = {target_address, source_address, payload};
                                                                                                          GetConversationManager()->TransmitUdsMsg(response, this->GetChannelId());
                                                                                                     });
                GetConversationManager()->GetAuthentication().DealAuthenticationMessage(message->GetPayload());
                return;
#endif
            }
            else
            {
                if (GetConversationManager()->GetAuthentication().IsAuthenticationExemption(service_address, target_address) == false)
                {
                    authenticate_result = GetConversationManager()->GetAuthentication().Authenticate(conversation_id);
                    if (authenticate_result != AuthenticateResult::kAuthenticateSuccess)
                    {
                        LOG_ERROR << __FUNCTION__ << "LocalTester Authenticate failed! kResourceTemporarilyNotAvailable. ";
                        GetConversationManager()->ResourceTemporarilyNotAvailable(conversation_id, target_address, sid);
                        return;
                    }
                }
            }
        }
        else if (tester_type == TesterType::kVirtualLocal)
        {
            authenticate_result = GetConversationManager()->GetAuthentication().Authenticate();
            if (authenticate_result == AuthenticateResult::kAuthenticateSuccess)
            {
                tester_priority = GetConversationManager()->GetTesterTable().GetTesterPriority(TesterType::kLocal, source_address);
            }
            else if (authenticate_result == AuthenticateResult::kAuthenticateFailure)
            {
                LOG_ERROR << __FUNCTION__ << "VirtualLocalTester Authenticate failed! kResourceTemporarilyNotAvailable. ";
                GetConversationManager()->ResourceTemporarilyNotAvailable(conversation_id, target_address, sid);
                return;
            }
            else
            {
                // AuthenticateResult::kAuthenticateInitial
            }
        }
        else
        {
            // TesterType::kVirtual
        }
    }
    LOG_DEBUG << __FUNCTION__ << " tester_address: " << LOG_HEX(source_address) << "tester_priority: " << tester_priority;

    // Diagnostic Routing
    const auto processor_info = GetConversationManager()->GetRouting().GetProcessorInfo(*message);
    const auto processor_type = std::get<0>(processor_info);
    const auto processor_id = std::get<1>(processor_info);
    const auto processor_address = std::get<2>(processor_info);
    message->SetProcessorType(processor_type);
    message->SetProcessorId(processor_id);

    // Diagnostic Arbitrate
    if (GetConversationManager()->GetArbitration().ArbitrationEnable() == true)
    {
        uint8_t arbitrate_result{ArbitrateResult::kArbitratePass};
        if (message->GetTaType() == UdsMessage::TargetAddressType::kPhysical)
        {
            arbitrate_result = GetConversationManager()->GetArbitration().Arbitrate(processor_address, source_address, tester_priority);
        }
        else
        {
            if (message->ShouldSuppressPositiveResponse() == false)
            {
                arbitrate_result = GetConversationManager()->GetArbitration().Arbitrate(processor_address);
            }
        }
        if (arbitrate_result == ArbitrateResult::kArbitratePending)
        {
            const auto cache_message = std::make_tuple(conversation_id, *message);
            GetConversationManager()->GetArbitration().ArbitrateCacheMessage(processor_address, cache_message);
            return;
        }
        else if (arbitrate_result == ArbitrateResult::kArbitrateReject)
        {
            LOG_ERROR << __FUNCTION__ << "Arbitrate reject! kResourceTemporarilyNotAvailable. ";
            GetConversationManager()->ResourceTemporarilyNotAvailable(conversation_id, target_address, sid);
            return;
        }
        else
        {
            // kArbitratePass
        }
    }

    // Diagnostic Routing
    GetConversationManager()->GetRouting().RoutingMessage(conversation_id, *message);
}

} /* namespace conversation */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
