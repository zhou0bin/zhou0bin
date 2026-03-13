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

#ifndef SOURCE_DCM_DIAGNOSTIC_COMMUNICATION_MANAGER_H_
#define SOURCE_DCM_DIAGNOSTIC_COMMUNICATION_MANAGER_H_

#include <memory>

#include "dcm/diagnostic_server_interface.h"

#include "asf/diag/uds_transport/protocol_mgr.h"
#include "asf/diag/uds_transport/protocol_handler.h"

#include "dcm/conversation/conversation_manager.h"

#include "common/diagnostic_proxy.h"

namespace asf
{
namespace diag
{
namespace dcm
{

/// @brief The Diagnostic Communication Manager.
///
/// This class combines all handling of network communication.
///
class DCM
    : public common::DiagnosticProxy
    , public DiagServerInterface
{
protected:
    /**
     * \brief The UdsTransportProtocolMgr used by the DCM.
     */
    std::shared_ptr<uds_transport::UdsTransportProtocolMgr> transportprotocolMgrPtr_;

    /**
     * \brief The ConversationManager used by the DCM.
     */
    std::shared_ptr<conversation::ConversationManager> conversation_manager_;

public:
    /**
     * \brief Constructor of DCM.
     *
     * \param config The config object.
     */
    DCM(const std::string& config);
    explicit DCM(DCM&&) = delete;
    DCM(DCM&) = delete;
    DCM& operator=(DCM&) = delete;
    DCM& operator=(DCM&&) = delete;
    ~DCM() override = default;

    /**
     * \brief Initialization to start the UdsTransportProtocolHandlers.
     */
    virtual void Initialize() override;

    /**
     * \brief Runtime work of the DCM.
     *
     * This method is currently empty.
     */
    virtual void Run() override;

    /**
     * \brief Terminates communication and performs cleanup.
     *
     * This method causes all sockets to be closed and waits for all threads spawned by the DCM to terminate.
     */
    virtual void Shutdown() override;

    virtual void HandleMessage(uds_transport::UdsMessageExtPtr UdsMsgExtPtr) override;
    // virtual void TransmitConfirm(const uds_transport::UdsMessageConstPtr message, const bool result) override;
    virtual void TransmitConfirm(const bool result) override;

    virtual void TransmitMsg(uds_transport::UdsMessageExt UdsMsgExt, const GlobalChannelID ToChannelID) override;
    virtual void TransmitAckMsg(const uint16_t sa, const uint16_t ta, const GlobalChannelID ToChannelID) override;
    virtual void TransmitNackMsg(const uint16_t sa, const uint16_t ta, const uint8_t nack_code, const GlobalChannelID ToChannelID) override;

    virtual bool RegChannelStateEvent(const GlobalChannelID ChannelID) override;
    virtual void HandleChannelDisconnected(GlobalChannelID ChannelID) override;
    virtual void HandleChannelReestablished(GlobalChannelID ChannelID) override;

};

} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_DIAGNOSTIC_COMMUNICATION_MANAGER_H_
