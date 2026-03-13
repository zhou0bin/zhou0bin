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

#ifndef SOURCE_DCM_CONVERSATION_CONVERSATION_MANAGER_H_
#define SOURCE_DCM_CONVERSATION_CONVERSATION_MANAGER_H_

#include <bitset>
#include <memory>
#include <mutex>
#include <tuple>

// #include "ara/core/map.h"
#include <map>

#include "dcm/connection/connection.h"
#include "dcm/conversation/conversation.h"
#include "dcm/diagnostic_server_interface.h"
#include "uds_transport/uds_message_ext.h"

#include "dcm/arbitration/arbitration.h"
using asf::diag::dcm::arbitration::Arbitration;

#include "dcm/authentication/authentication.h"
using asf::diag::dcm::authentication::Authentication;

#include "dcm/route/routing.h"
using asf::diag::dcm::route::Routing;

#include "dcm/tester/tester_table.h"
using asf::diag::dcm::tester::TesterTable;

namespace asf
{
namespace diag
{
namespace dcm
{
namespace conversation
{

/*
 * \brief Definition of maximum or invalid conversation Id
 */
static const Conversation::ConversationId kConversationIdMax{0xFFU};

/**
 * \brief base class for proxy route.
 */
class ConversationManager : public std::enable_shared_from_this<ConversationManager>
{
    /**
     * \brief Type for a pointer to the ConversationManager.
     */
    using Ptr = std::shared_ptr<ConversationManager>;

    /**
     * \brief ConversationKey.
     */
    using ConversationKey = std::tuple<connection::Connection::IdentifyingTuple, uint16_t>;

    /**
     * \brief A collection of conversations.
     */
    using ConversationCollection = std::map<ConversationKey, Conversation::Ptr>;

    using ChannelStateRegTbl = std::map<uds_transport::GlobalChannelIdentifier, Conversation::ConversationId>;

protected:
    DiagServerInterface& DSRef_;

    /**
     * \brief All conversations of the ConversationManager.
     */
    ConversationCollection conversations_;

public:
    /**
     * \brief Constructor for ConversationManager.
     */
    explicit ConversationManager(DiagServerInterface& DSRef, const std::string& config);
    explicit ConversationManager(ConversationManager&&) = delete;
    ConversationManager(ConversationManager&) = delete;
    ConversationManager& operator=(ConversationManager&) = delete;
    ConversationManager& operator=(ConversationManager&&) = delete;
    virtual ~ConversationManager() = default;

    virtual void Start();

    virtual void Stop();

    /**
     * \brief Return a conversation for the given connection and message.
     *
     * \param message The UdsMessage containing Source Address and Target Address.
     * \param connection The Uds Transport Protocol Connection for which to find a conversation.
     */
    virtual Conversation::Ptr FindOrCreateConversation(const uds_transport::UdsMessageExt& message);

    /**
     * \brief Get a new conversation id.
     */
    Conversation::ConversationId GetNextConversationId();

    /**
     * \brief Returns Conversation with given Id.
     * @param conversation_id
     */
    Conversation::Ptr GetConversation(const Conversation::ConversationId conversation_id);

    void TransmitUdsMsg(
        const uds_transport::UdsMessageExt& UdsMsgExt,
        const uds_transport::GlobalChannelIdentifier GlobalChannelID);

    void TransmitDiagAck(
        const uint16_t sa,
        const uint16_t ta,
        const uds_transport::GlobalChannelIdentifier GlobalChannelID);

    void TransmitDiagNack(
        const uint16_t sa,
        const uint16_t ta,
        const uint8_t nack_code,
        const uds_transport::GlobalChannelIdentifier GlobalChannelID);

    bool RegChannelStateEvent(
        const uds_transport::GlobalChannelIdentifier ChannelID,
        const Conversation::ConversationId ConversationID);

    void HandleChannelDisconnected(const uds_transport::GlobalChannelIdentifier GlobalChannelID);

    void HandleChannelReestablished(const uds_transport::GlobalChannelIdentifier GlobalChannelID);

    void ResourceTemporarilyNotAvailable(const Conversation::ConversationId conversation_id, const uint16_t address, const uint8_t sid);

    void ResponsePending(const Conversation::ConversationId conversation_id, const uint16_t address, const uint8_t sid);

    virtual Arbitration& GetArbitration()
    {
        return diag_arbitration_;
    }

    virtual Authentication& GetAuthentication()
    {
        return diag_authentication_;
    }

    virtual Routing& GetRouting()
    {
        return diag_routing_;
    }

    virtual TesterTable& GetTesterTable()
    {
        return tester_table_;
    }

private:

    std::mutex handle_conversations_;

    /**
     * \brief The bitset storing used conversation ids.
     * Implementation is optimized for ConversationId of type uint8_t.
     */
    std::bitset<256> conversation_ids_;

    /**
     * Mutex to block concurrent adaptation to conversation_ids;
     */
    std::mutex handle_conversation_ids_;

    /**
     * \brief The last used conversation id.
     */
    Conversation::ConversationId last_conversation_id_;

    ChannelStateRegTbl channel_state_reg_tbl_;

    Arbitration diag_arbitration_;

    Authentication diag_authentication_;

    Routing diag_routing_;

    TesterTable tester_table_;

};

} /* namespace conversation */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_CONVERSATION_CONVERSATION_MANAGER_H_
