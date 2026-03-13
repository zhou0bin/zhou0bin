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

#include "transport_protocol_mgr.h"
#include "asf/diag/uds_transport/protocol_mgr.h"
#include "asf/diag/uds_transport/protocol_types.h"
#include "uds_transport/uds_message_ext.h"

#include "dcm/connection/do_ip_handler.h"

#ifdef UDS_ON_NEUSAR_NET
#include "dcm/connection/uds_on_neusar_net.h"
#endif

#include "log.h"

using asf::diag::uds_transport::UdsTransportProtocolMgr;
using asf::diag::uds_transport::Priority;
using asf::diag::uds_transport::ProtocolKind;
using asf::diag::uds_transport::UdsMessagePtr;
using asf::diag::uds_transport::UdsMessage;
using asf::diag::uds_transport::UdsMessageExt;
using asf::diag::uds_transport::UdsMessageExtPtr;

namespace asf
{
namespace diag
{
namespace uds_transport
{

TransportProtocolMgr::TransportProtocolMgr(dcm::DiagServerInterface& DSPtr, std::string& config)
    : UdsTransportProtocolMgr()
    , DSRef_(DSPtr)
    , config_(config)
    , UdsMsgExtVect()
    , doipStopSem()
    , udsOnNeusatNetStopSem()
{
    (void)sem_init(&doipStopSem,0,0);
    (void)sem_init(&udsOnNeusatNetStopSem,0,0);
}

void TransportProtocolMgr::Initialize()
{
    AddHandler(new dcm::connection::DoIpHandler(config_, *this));
#ifdef UDS_ON_NEUSAR_NET
    AddHandler(new dcm::connection::UdsOnNeusarNet(config_, *this));
#endif
    InitializeHandlers();
}

std::pair<UdsTransportProtocolMgr::IndicationResult, UdsMessageExtPtr> TransportProtocolMgr::IndicateMessage(
    UdsMessage::Address sourceAddr,
    UdsMessage::Address targetAddr,
    const UdsMessage::TargetAddressType type,
    const UdsTransportProtocolMgr::GlobalChannelIdentifier globalChannelId,
    const std::size_t size,
    const Priority priority,
    const ProtocolKind protocolKind,
    std::vector<uint8_t> /*payloadInfo*/)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto canBeHandled = DSRef_.RequestCanBeHandled(sourceAddr, targetAddr, type, globalChannelId, size, priority, protocolKind);
    if (canBeHandled)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(sourceAddr) << "ta: " << LOG_HEX(targetAddr) << "payload_size: " << size;
        // make a new UdsMessage
        UdsMessageExtPtr UdsMsgPtr = std::make_unique<UdsMessageExt>(sourceAddr, targetAddr);
        UdsMsgPtr->SetTaType(type);
        UdsMsgPtr->SetRecvChannelID(globalChannelId);
        return std::make_pair(UdsTransportProtocolMgr::IndicationResult::kIndicationOk, std::move(UdsMsgPtr));
    }
    return std::make_pair(UdsTransportProtocolMgr::IndicationResult::kIndicationOccupied, nullptr);
}

void TransportProtocolMgr::HandleMessage(UdsMessageExtPtr message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    DSRef_.HandleMessage(std::move(message));
}

void TransportProtocolMgr::TransmitConfirmation(const TransmissionResult result)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const bool trans_result = (result == kTransmitOk) ? true : false;
    DSRef_.TransmitConfirm(trans_result);
}

void TransportProtocolMgr::ChannelDisconnected(GlobalChannelIdentifier globalChannelId)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    DSRef_.HandleChannelDisconnected(globalChannelId);
}

void TransportProtocolMgr::ChannelReestablished(GlobalChannelIdentifier globalChannelId)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    DSRef_.HandleChannelReestablished(globalChannelId);
}

void TransportProtocolMgr::HandlerStopped(const UdsTransportHandlerID handlerId)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (handlerId == uds_transport::DoIp)
    {
        LOG_DEBUG << __FUNCTION__ << "Id: " << handlerId;
        (void)sem_post(&doipStopSem);
    }
    else if (handlerId == uds_transport::UDSonNeusarNet)
    {
        LOG_DEBUG << __FUNCTION__ << "Id: " << handlerId;
        (void)sem_post(&udsOnNeusatNetStopSem);
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "Handler id wrong! Id: " << handlerId;
    }
}

void TransportProtocolMgr::WaitHandlerStopped(const UdsTransportHandlerID handlerId)
{
    if (handlerId == uds_transport::DoIp)
    {
        LOG_DEBUG << __FUNCTION__ << "Id: " << handlerId;
        (void)sem_wait(&doipStopSem);
        (void)sem_destroy(&doipStopSem);
        LOG_DEBUG << __FUNCTION__ << "Id: " << handlerId << "out! ";
    }
    else if (handlerId == uds_transport::UDSonNeusarNet)
    {
        LOG_DEBUG << __FUNCTION__ << "Id: " << handlerId;
        (void)sem_wait(&udsOnNeusatNetStopSem);
        (void)sem_destroy(&udsOnNeusatNetStopSem);
        LOG_DEBUG << __FUNCTION__ << "Id: " << handlerId << "out! ";
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "Handler id wrong! Id: " << handlerId;
    }
}

} /* namespace uds_transport */
} /* namespace diag */
} /* namespace asf */
