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

/// @file

#ifndef APD_ARA_DIAG_UDS_TRANSPORT_PROTOCOL_MGR_H_
#define APD_ARA_DIAG_UDS_TRANSPORT_PROTOCOL_MGR_H_

#include <cstddef>
#include <tuple>
#include <utility>  // std::pair

#include "protocol_types.h"
#include "uds_message.h"

#include "asf/diag/uds_transport/protocol_handler.h"
using asf::diag::uds_transport::UdsTransportProtocolHandler;
//using asf::diag::uds_transport::UdsTransportProtocolHandler;

namespace asf
{
namespace diag
{
namespace uds_transport
{

class UdsTransportProtocolMgr
{
public:
    /// @brief Global unique identifier of a UdsTransport Protocol channel.
    ///
    //using GlobalChannelIdentifier = std::tuple<UdsTransportProtocolHandlerID, ChannelID>;
    using GlobalChannelIdentifier = uds_transport::GlobalChannelIdentifier;
    using UdsTransportHandlerID = uds_transport::UdsTransportProtocolHandlerID;
    using udsHandlerUniquePtr = std::unique_ptr<UdsTransportProtocolHandler>;

    enum IndicationResult
    {
        kIndicationOk = 0,
        kIndicationOccupied = 1,
        kIndicationOverflow = 2,
        kIndicationUnknownTargetAddress = 3
    };

    enum TransmissionResult
    {
        kTransmitOk = 0,
        kTransmitFailed = 1
    };

public:
    UdsTransportProtocolMgr() = default;
    explicit UdsTransportProtocolMgr(UdsTransportProtocolMgr&&) = delete;
    UdsTransportProtocolMgr(const UdsTransportProtocolMgr&) = delete;
    UdsTransportProtocolMgr& operator=(const UdsTransportProtocolMgr&) = delete;
    UdsTransportProtocolMgr& operator=(UdsTransportProtocolMgr&&) = delete;
    virtual ~UdsTransportProtocolMgr() = default;

//////////upstream interface//////////////

    virtual void Initialize() = 0;

    /// @brief Indicates a message start.
    virtual std::pair<IndicationResult, UdsMessageExtPtr> IndicateMessage(UdsMessage::Address sourceAddr,
        UdsMessage::Address targetAddr,
        UdsMessage::TargetAddressType type,
        GlobalChannelIdentifier globalChannelId,
        std::size_t size,
        Priority priority,
        ProtocolKind protocolKind,
        std::vector<uint8_t> payloadInfo)
        = 0;

    virtual void NotifyMessageFailure(UdsMessageExtPtr message) = 0;

    virtual void HandleMessage(UdsMessageExtPtr message) = 0;

    // virtual void TransmitConfirmation(UdsMessageConstPtr message, TransmissionResult result) = 0;
    virtual void TransmitConfirmation(TransmissionResult result) = 0;

    virtual void ChannelDisconnected(GlobalChannelIdentifier globalChannelId) = 0;

    virtual void ChannelReestablished(GlobalChannelIdentifier globalChannelId) = 0;

    virtual void HandlerStopped(UdsTransportProtocolHandlerID handlerId) = 0;

//////////downstream interface//////////////
    void AddHandler(uds_transport::UdsTransportProtocolHandler* UdsTPHandlerPtr)
    {
        // Setup UDS Transport Protocol handlers
        (void)uds_transport_protocol_handlers_.emplace_back(UdsTPHandlerPtr);
    };

    void InitializeHandlers()
    {
        if (!uds_transport_protocol_handlers_.empty()) {
            for (auto& handler : uds_transport_protocol_handlers_) {
                (void)handler->Initialize();
            }
        }
    };

    void StartHandlers()
    {
        if (!uds_transport_protocol_handlers_.empty()) {
            for (auto& handler : uds_transport_protocol_handlers_) {
                handler->Start();
            }
        }
    };

    void StopHandlers()
    {
        if (!uds_transport_protocol_handlers_.empty()) {
            for (auto& handler : uds_transport_protocol_handlers_) {
                handler->Stop();
            }
        }
        if (!uds_transport_protocol_handlers_.empty()) {
            for (auto& handler : uds_transport_protocol_handlers_) {
                WaitHandlerStopped(handler->GetHandlerID());
            }
        }
    };

    void Transmit(uds_transport::UdsMessageExtPtr UdsMsgExtPtr, GlobalChannelIdentifier ToChannelID)
    {
        for (auto& i: uds_transport_protocol_handlers_)
        {
            if (i->GetHandlerID() == std::get<0>(ToChannelID))
            {
                i->Transmit(std::move(UdsMsgExtPtr), std::get<1>(ToChannelID));
            }
        }
    }

    void TransmitAck(const uint16_t sa, const uint16_t ta, GlobalChannelIdentifier ToChannelID)
    {
        for (auto& i: uds_transport_protocol_handlers_)
        {
            if (i->GetHandlerID() == std::get<0>(ToChannelID))
            {
                i->TransmitAck(sa, ta, std::get<1>(ToChannelID));
            }
        }
    }

    void TransmitNack(const uint16_t sa, const uint16_t ta, const uint8_t nack_code, GlobalChannelIdentifier ToChannelID)
    {
        for (auto& i: uds_transport_protocol_handlers_)
        {
            if (i->GetHandlerID() == std::get<0>(ToChannelID))
            {
                i->TransmitNack(sa, ta, nack_code, std::get<1>(ToChannelID));
            }
        }
    }

    bool RegisterChannelStateEvent(GlobalChannelIdentifier ToChannelID)
    {
        for (auto& i: uds_transport_protocol_handlers_)
        {
            if (i->GetHandlerID() == std::get<0>(ToChannelID))
            {
                return i->NotifyReestablishment(std::get<1>(ToChannelID));
            }
        }
        return false;
    }

private:
     virtual void WaitHandlerStopped(UdsTransportHandlerID HandlerID) = 0;

private:
    std::vector<udsHandlerUniquePtr> uds_transport_protocol_handlers_;

};

} /* namespace uds_transport */
} /* namespace diag */
} /* namespace asf */

#endif  // APD_ARA_DIAG_UDS_TRANSPORT_PROTOCOL_MGR_H_
