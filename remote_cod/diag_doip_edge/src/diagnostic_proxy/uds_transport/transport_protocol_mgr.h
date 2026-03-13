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

#ifndef SOURCE_UDS_TRANSPORT_DOIP_TRANSPORT_PROTOCOL_H_
#define SOURCE_UDS_TRANSPORT_DOIP_TRANSPORT_PROTOCOL_H_

#include <semaphore.h>

#include "asf/diag/uds_transport/protocol_mgr.h"
#include "asf/diag/uds_transport/protocol_types.h"
#include "asf/diag/uds_transport/uds_message.h"
#include "uds_transport/protocol_types_priv.h"

#include "dcm/diagnostic_server_interface.h"
using asf::diag::dcm::DiagServerInterface;
// class asf::diag::dcm::DiagServerInterface;

namespace asf
{
namespace diag
{
namespace uds_transport
{

class TransportProtocolMgr : public asf::diag::uds_transport::UdsTransportProtocolMgr
{
public:
    using uds_transport::UdsTransportProtocolMgr::GlobalChannelIdentifier;
    using UdsMsgConstPtr = asf::diag::uds_transport::UdsMessageConstPtr;
    using UdsMessageExtPtr = asf::diag::uds_transport::UdsMessageExtPtr;
    using DSRefType = dcm::DiagServerInterface&;
    
    TransportProtocolMgr(dcm::DiagServerInterface& DSPtr, std::string& config);

    explicit TransportProtocolMgr(TransportProtocolMgr&&) = delete;
    TransportProtocolMgr(TransportProtocolMgr&) = delete;
    TransportProtocolMgr& operator=(TransportProtocolMgr&) = delete;
    TransportProtocolMgr& operator=(TransportProtocolMgr&&) = delete;
    ~TransportProtocolMgr() override = default;

    virtual void Initialize() override;

    virtual std::pair<IndicationResult, UdsMessageExtPtr> IndicateMessage(
        asf::diag::uds_transport::UdsMessage::Address sourceAddr,
        asf::diag::uds_transport::UdsMessage::Address targetAddr,
        const asf::diag::uds_transport::UdsMessage::TargetAddressType type,
        const GlobalChannelIdentifier globalChannelId,
        const std::size_t size,
        const asf::diag::uds_transport::Priority priority,
        const asf::diag::uds_transport::ProtocolKind protocolKind,
        std::vector<uint8_t> payloadInfo) override;

    virtual void NotifyMessageFailure(const UdsMessageExtPtr /*message*/) override
    {
    }

    virtual void HandleMessage(UdsMessageExtPtr message) override;

    // virtual void TransmitConfirmation(UdsMsgConstPtr message, const TransmissionResult result) override;
    virtual void TransmitConfirmation(const TransmissionResult result) override;

    virtual void ChannelDisconnected(GlobalChannelIdentifier globalChannelId) override;

    virtual void ChannelReestablished(GlobalChannelIdentifier globalChannelId) override;

    virtual void HandlerStopped(const UdsTransportHandlerID handlerId) override;

private:
    virtual void WaitHandlerStopped(const UdsTransportHandlerID handlerId) override;

private:
    DSRefType DSRef_;
    std::string config_;
    std::vector<UdsMessageExtPtr> UdsMsgExtVect;
    sem_t doipStopSem;
    sem_t udsOnNeusatNetStopSem;

};

} /* namespace uds_transport */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_UDS_TRANSPORT_DOIP_TRANSPORT_PROTOCOL_H_
