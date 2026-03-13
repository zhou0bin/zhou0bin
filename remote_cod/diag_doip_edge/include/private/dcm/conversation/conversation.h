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

#ifndef SOURCE_DCM_CONVERSATION_CONVERSATION_H_
#define SOURCE_DCM_CONVERSATION_CONVERSATION_H_

#include <memory>
#include <mutex>
#include <ostream>
#include <unordered_map>

#include "uds_transport/uds_message_ext.h"

#include "diag_proxy_common.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace conversation
{

/**
 * \brief Forward declaration to avoid cyclic includes
 */
class ConversationManager;

/**
 * \brief Class that represents a diagnostic conversation between a tester and the ECU.
 *
 */
class Conversation
{
public:

    using UdsMessageExtPtr = asf::diag::uds_transport::UdsMessageExtPtr;
    using UdsMessageExt = asf::diag::uds_transport::UdsMessageExt;
    using logicalAddr = asf::diag::uds_transport::UdsMessage::Address;

    /**
     * \brief A shared pointer type to a Conversation object.
     */
    typedef std::shared_ptr<Conversation> Ptr;

    /**
     * \brief type of identifier for conversation.
     */
    typedef uint8_t ConversationId;

    /**
     * \brief Constructor of Conversation.
     *
     * @param conversation_manager Weak Pointer to the ConversationManager that created this Conversation object.
     * @param connection The connection of this conversation.
     * @param conversation_id The connection id of this conversation.
     *
     */
    Conversation(const std::shared_ptr<ConversationManager> conversation_manager,
        const uds_transport::GlobalChannelIdentifier global_channel_id,
        const logicalAddr uds_sa,
        const logicalAddr uds_ta,
        const ConversationId conversation_id);
    explicit Conversation(Conversation&&) = delete;
    Conversation(const Conversation&) = delete;
    Conversation& operator=(const Conversation&) = delete;
    Conversation& operator=(Conversation&&) = delete;

    virtual ~Conversation() = default;

    /**
     * \brief Construct a ServiceProcessor for the UdsMessage.
     * \param message The Uds message with the request.
     *
     */
    virtual void HandleMessage(UdsMessageExtPtr message);

    /**
     * @brief Get a shared_ptr to the conversation_manager_ (const version)
     * @return A shared_ptr to the conversation_manager_
     */
    std::shared_ptr<conversation::ConversationManager> GetConversationManager() const
    {
        return conversation_manager_;
    }

    /**
     * \brief Get a shared pointer to the connection.
     *
     * \return A shared pointer to the connection.
     */
    // std::shared_ptr<connection::Connection> GetConnection() const
    // {
        //return connection_;
    // }

    uds_transport::GlobalChannelIdentifier GetChannelId() const
    {
        return global_channel_id_;
    }

    /**
     * \brief Get the source address of this conversation.
     * @return The source address of this conversation.
     */
    logicalAddr GetSa() const
    {
        return uds_sa_;
    }

    /**
     * \brief Get the identifier of this conversation.
     * @return The identifier of this conversation.
     */
    ConversationId GetId() const
    {
        return conversation_id_;
    }

protected:
    /**
     * \brief The ConversationManager that created this session object
     */
    std::shared_ptr<conversation::ConversationManager> conversation_manager_;
    
    const uds_transport::GlobalChannelIdentifier global_channel_id_;

    /**
     * Source address of uds conversation.
     */
    uint16_t uds_sa_;

    /**
     * Target address of uds conversation.
     */
    uint16_t uds_ta_;

    /**
     * Identifier of this conversation
     */
    ConversationId conversation_id_;

    /**
     * \brief Mutex for access to any Conversation function.
     */
    std::recursive_mutex conversation_mutex_;
};

} /* namespace conversation */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_CONVERSATION_CONVERSATION_H_
