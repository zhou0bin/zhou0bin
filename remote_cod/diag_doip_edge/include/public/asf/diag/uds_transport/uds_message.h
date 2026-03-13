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

#ifndef APD_ARA_DIAG_UDS_TRANSPORT_UDS_MESSAGE_H_
#define APD_ARA_DIAG_UDS_TRANSPORT_UDS_MESSAGE_H_

#include <cstdint>  // uint8_t
#include <memory>  // unique_ptr

#include "protocol_types.h"  // ByteVector

namespace asf
{
namespace diag
{
namespace uds_transport
{

/// @brief Class which represents a UDS message
class UdsMessage
{
public:
    /// @brief Type for UDS source and target addresses.
    using Address = uint16_t;
    using ProcessorType = uint8_t;
    using ProcessorId = uint8_t;
    /// @brief Type of target address in UdsMessage.
    enum class TargetAddressType : std::uint8_t
    {
        kPhysical = 0,
        kFunctional = 1
    };

//protected:
public:
    /// @brief non public default ctor
    UdsMessage() = default;

    UdsMessage(const Address source_address,
        const Address target_address,
        const uds_transport::ByteVector& payload = {});
protected:
    UdsMessage(const UdsMessage& one);
    UdsMessage& operator=(const UdsMessage& one);
    explicit UdsMessage(UdsMessage&&) = delete;
    UdsMessage& operator=(UdsMessage&&) = delete;

public:
    virtual ~UdsMessage() = default;

    /// @brief Get the UDS message data starting with the SID (A_Data as per ISO)
    virtual const uds_transport::ByteVector& GetPayload() const;

    /// @brief Return the underlying buffer for write access.
    virtual uds_transport::ByteVector& GetPayload();

    /// @brief Get the source address of the uds message.
    virtual Address GetSa() const noexcept;

    /// @brief Get the target address of the uds message.
    virtual Address GetTa() const noexcept;

    /// @brief Get the target address type of the uds message.
    virtual TargetAddressType GetTaType() const noexcept;
    virtual void SetTaType(const TargetAddressType type) noexcept;

    /// @brief Get the target processor type of the uds message.
    virtual ProcessorType GetProcessorType() const noexcept;
    virtual void SetProcessorType(const ProcessorType type) noexcept;

    /// @brief Get the target processor id of the uds message.
    virtual ProcessorId GetProcessorId() const noexcept;
    virtual void SetProcessorId(const ProcessorId id) noexcept;
        
    GlobalChannelIdentifier GetRecvChannelID() const
    {
        return std::make_tuple(udsHandlerID, channelID);
    }

    void SetRecvChannelID(const GlobalChannelIdentifier& recv_channel_id)
    {
        udsHandlerID = std::get<0>(recv_channel_id);
        channelID = std::get<1>(recv_channel_id);
    }

private:
    UdsTransportProtocolHandlerID udsHandlerID;
    ChannelID channelID;
    Address source_address_;
    Address target_address_;
    TargetAddressType addressing_type_;
    ProcessorType processor_type_;
    ProcessorId processor_id_;
    
    /// @brief Payload of the uds message.
    asf::diag::uds_transport::ByteVector payload_;
};

class UdsMessageExt;

/// @brief unique_ptr for constant UdsMessages as provided by the generic/core DM part towards the
/// UdsTransportLayer-Plugin.
using UdsMessageConstPtr = std::unique_ptr<const UdsMessageExt>;

/// @brief unique_ptr for UdsMessagesas provided by the generic/core DM part towards the UdsTransportLayer-Plugin.
using UdsMessagePtr = std::unique_ptr<UdsMessage>;
//using UdsMessagePtr = std::shared_ptr<UdsMessage>;

using UdsMessageExtPtr = std::unique_ptr<UdsMessageExt>;

} /* namespace uds_transport */
} /* namespace diag */
} /* namespace asf */

#endif  // APD_ARA_DIAG_UDS_TRANSPORT_UDS_MESSAGE_H_
