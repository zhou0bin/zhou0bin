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

#include "dcm/conversation/conversation_manager.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <utility>

#include "dcm/connection/uds_error_code.h"
#include "dcm/conversation/conversation.h"

#include "log.h"

#include "uds_transport/uds_message_ext.h"
using asf::diag::uds_transport::UdsMessageExt;
using asf::diag::uds_transport::ByteVector;
#include "asf/diag/uds_transport/protocol_types.h"
using asf::diag::uds_transport::GlobalChannelIdentifier;

namespace asf
{
namespace diag
{
namespace dcm
{
namespace conversation
{

ConversationManager::ConversationManager(DiagServerInterface& DSRef, const std::string& config)
    : DSRef_(DSRef)
    , conversations_()
    , handle_conversations_()
    , conversation_ids_()
    , handle_conversation_ids_()
    , last_conversation_id_(kConversationIdMax)
    , channel_state_reg_tbl_()
    , diag_arbitration_(config)
    , diag_authentication_(config)
    , diag_routing_(*this, config)
    , tester_table_()
{
}

void ConversationManager::Start()
{
    diag_authentication_.Initialize();
    diag_arbitration_.Initialize();
    diag_routing_.Initialize();

}

void ConversationManager::Stop()
{
    diag_authentication_.Deinitialize();
    diag_arbitration_.Deinitialize();
    diag_routing_.Deinitialize();

    // clear conversations
    {
        const std::lock_guard<std::mutex> locker(handle_conversations_);
        conversations_.clear();
    }
}

Conversation::Ptr ConversationManager::FindOrCreateConversation(const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const std::lock_guard<std::mutex> locker(handle_conversations_);
    const uds_transport::GlobalChannelIdentifier ChannelID = message.GetRecvChannelID();
    LOG_DEBUG << __FUNCTION__ << "ChannelID: " << std::get<0>(ChannelID) << "," << std::get<1>(ChannelID);
    LOG_DEBUG << __FUNCTION__ << "Sa: " << LOG_HEX(message.GetSa());

    ConversationKey key{ChannelID, message.GetSa()};
    ConversationCollection::iterator it(conversations_.find(key));
    if (it == conversations_.end()) {
        Conversation::ConversationId conversation_id;
        try
        {
            conversation_id = GetNextConversationId();
        }
        catch(const std::exception& e)
        {
            LOG_ERROR << __FUNCTION__ << "exception: " << e.what();
            return nullptr;
        }
        // Create a new conversation
        const std::pair<ConversationCollection::iterator, bool> result {
            conversations_.insert({key, std::make_shared<Conversation>(shared_from_this(), ChannelID, message.GetSa(), message.GetTa(), conversation_id)})
        };
        it = result.first;
        LOG_DEBUG << __FUNCTION__ << "Newly Created Conversation! ID: " << it->second->GetId();
    } else {
        LOG_DEBUG << __FUNCTION__ << "Found Conversation! ID: " << it->second->GetId();
        //it->second->UpdateConnection(connection);
    }
    return it->second;
}

Conversation::ConversationId ConversationManager::GetNextConversationId()
{
    const std::lock_guard<std::mutex> locker_cid(handle_conversation_ids_);
    uint8_t new_id{last_conversation_id_};
    do {
        if (new_id == kConversationIdMax) {
            new_id = 1;
        } else {
            new_id++;
        }
        if (!conversation_ids_[new_id]) {
            (void)conversation_ids_.set(new_id);
            last_conversation_id_ = new_id;
            return last_conversation_id_;
        }
    } while (new_id != last_conversation_id_);
    throw std::runtime_error("No conversation id available. ");
}

Conversation::Ptr ConversationManager::GetConversation(const Conversation::ConversationId conversation_id)
{
    const std::lock_guard<std::mutex> locker(handle_conversations_);
    for (auto& it : conversations_) {
        if (it.second->GetId() == conversation_id) {
            return it.second;
        }
    }
    return nullptr;
}

void ConversationManager::TransmitUdsMsg(
    const UdsMessageExt& UdsMsgExt,
    const GlobalChannelIdentifier GlobalChannelID)
{
    DSRef_.TransmitMsg(UdsMsgExt, GlobalChannelID);
}

void ConversationManager::TransmitDiagAck(
    const uint16_t sa,
    const uint16_t ta,
    const GlobalChannelIdentifier GlobalChannelID)
{
    DSRef_.TransmitAckMsg(sa, ta, GlobalChannelID);
}

void ConversationManager::TransmitDiagNack(
    const uint16_t sa,
    const uint16_t ta,
    const uint8_t nack_code,
    const GlobalChannelIdentifier GlobalChannelID)
{
    DSRef_.TransmitNackMsg(sa, ta, nack_code, GlobalChannelID);
}

bool ConversationManager::RegChannelStateEvent(
    const GlobalChannelIdentifier ChannelID,
    const Conversation::ConversationId conversation_id)
{
    channel_state_reg_tbl_[ChannelID] = conversation_id;
    return DSRef_.RegChannelStateEvent(ChannelID);
}

void ConversationManager::HandleChannelDisconnected(const GlobalChannelIdentifier ChannelID)
{
    if (channel_state_reg_tbl_.find(ChannelID) != channel_state_reg_tbl_.end())
    {
        const auto conversation_id = channel_state_reg_tbl_[ChannelID];
        if (diag_authentication_.AuthenticationEnable() == true)
        {
            diag_authentication_.StartAuthenticationTimer(conversation_id);
        }
    }
}

void ConversationManager::HandleChannelReestablished(const GlobalChannelIdentifier ChannelID)
{
    if (channel_state_reg_tbl_.find(ChannelID) != channel_state_reg_tbl_.end())
    {
        const auto conversation_id = channel_state_reg_tbl_[ChannelID];
        if (diag_authentication_.AuthenticationEnable() == true)
        {
            diag_authentication_.StopAuthenticationTimer(conversation_id);
        }
    }
}

void ConversationManager::ResourceTemporarilyNotAvailable(const Conversation::ConversationId conversation_id, const uint16_t address, const uint8_t sid)
{
    const auto ptr = GetConversation(conversation_id);
    if (ptr != nullptr)
    {
        const auto ta = ptr->GetSa();
        const auto channel_id = ptr->GetChannelId();
        const ByteVector payload = {0x7f, sid, connection::UdsNegativeResponseCode::kResourceTemporarilyNotAvailable};
        const UdsMessageExt message = {address, ta, payload};
        TransmitUdsMsg(message, channel_id);
    }
}

void ConversationManager::ResponsePending(const Conversation::ConversationId conversation_id, const uint16_t address, const uint8_t sid)
{
    const auto ptr = GetConversation(conversation_id);
    if (ptr != nullptr)
    {
        const auto ta = ptr->GetSa();
        const auto channel_id = ptr->GetChannelId();
        const ByteVector payload = {0x7f, sid, connection::UdsNegativeResponseCode::kRequestCorrectlyReceived_responsePending};
        const UdsMessageExt message = {address, ta, payload};
        TransmitUdsMsg(message, channel_id);
    }
}

} /* namespace conversation */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
