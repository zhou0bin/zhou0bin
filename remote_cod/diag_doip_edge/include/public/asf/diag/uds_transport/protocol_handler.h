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

#ifndef APD_ARA_DIAG_UDS_TRANSPORT_PROTOCOL_HANDLER_H_
#define APD_ARA_DIAG_UDS_TRANSPORT_PROTOCOL_HANDLER_H_

#include "protocol_types.h"
// #include "ara/core/result.h"


namespace asf
{
namespace diag
{
namespace uds_transport
{
    class UdsTransportProtocolMgr;

/// @brief Abstract Class, which a specific UDS Transport Protocol (plugin) shall subclass.
class UdsTransportProtocolHandler
{
private:
    /// @brief The id of the handler (shall be set by initializer list of ctor.)
    const asf::diag::uds_transport::UdsTransportProtocolHandlerID handler_id_;

protected:
    /// @brief The UdsTransportProtocolMgr used/provided by the DM/DCM.
    UdsTransportProtocolMgr& transportprotocol_manager_;

public:
    enum class InitializationResult
    {
        kInitializeOk = 0,
        kInitializeFailed = 1
    };

public:
    /// @brief Constructor of UdsTransportProtocolHandler.
    explicit UdsTransportProtocolHandler(const UdsTransportProtocolHandlerID handlerId,
        UdsTransportProtocolMgr& transportProtocolMgr)
        : handler_id_(handlerId)
        , transportprotocol_manager_(transportProtocolMgr)
    {
    }
    explicit UdsTransportProtocolHandler(UdsTransportProtocolHandler&&) = delete;
    UdsTransportProtocolHandler(const UdsTransportProtocolHandler&) = delete;
    UdsTransportProtocolHandler& operator=(const UdsTransportProtocolHandler&) = delete;
    UdsTransportProtocolHandler& operator=(UdsTransportProtocolHandler&&) = delete;
    /// @brief Destructor of UdsTransportProtocolHandler.
    virtual ~UdsTransportProtocolHandler() = default;

    /// @brief Return the UdsTransportProtocolHandlerID, which was given to the implementation during construction (ctor
    /// call).
    virtual UdsTransportProtocolHandlerID GetHandlerID() const
    {
        return handler_id_;
    }

    /// @brief Initializes handler.
    virtual InitializationResult Initialize() = 0;

    /// @brief Start processing the implemented Uds Transport Protocol.
    virtual void Start() = 0;

    /// @brief Method to indicate that this UdsTransportProtocolHandler should terminate.
    virtual void Stop() = 0;

    virtual bool NotifyReestablishment(ChannelID channelId) = 0;

    /// @brief Transmit a Uds message via the underlying Uds Transport Protocol channel.
    virtual void Transmit(UdsMessageConstPtr message, ChannelID channelId) = 0;
    virtual void TransmitAck(const uint16_t sa, const uint16_t ta, ChannelID channelId) = 0;
    virtual void TransmitNack(const uint16_t sa, const uint16_t ta, const uint8_t nack_code, ChannelID channelId) = 0;

};

} /* namespace uds_transport */
} /* namespace diag */
} /* namespace asf */

#endif  // APD_ARA_DIAG_UDS_TRANSPORT_PROTOCOL_HANDLER_H_
