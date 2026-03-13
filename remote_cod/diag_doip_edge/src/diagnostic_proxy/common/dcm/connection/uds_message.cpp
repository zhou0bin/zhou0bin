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

#include "asf/diag/uds_transport/uds_message.h"

namespace asf
{
namespace diag
{
namespace uds_transport
{

UdsMessage::UdsMessage(const Address source_address,
    const Address target_address,
    const uds_transport::ByteVector& payload)
    : udsHandlerID()
    , channelID()
    , source_address_(source_address)
    , target_address_(target_address)
    , addressing_type_( UdsMessage::TargetAddressType::kPhysical )
    , processor_type_(0x01)
    , processor_id_(0x01)
    , payload_(payload)
{
}

UdsMessage::UdsMessage(const UdsMessage& one)
{
    udsHandlerID = one.udsHandlerID;
    channelID = one.channelID;
    source_address_ = one.source_address_;
    target_address_ = one.target_address_;
    addressing_type_ = one.addressing_type_;
    processor_type_ = one.processor_type_;
    processor_id_ = one.processor_id_;

    (void)payload_.assign(one.payload_.begin(), one.payload_.end());
}

UdsMessage& UdsMessage::operator=(const UdsMessage& one)
{
    if (this != &one)
    {
        udsHandlerID = one.udsHandlerID;
        channelID = one.channelID;
        source_address_ = one.source_address_;
        target_address_ = one.target_address_;
        addressing_type_ = one.addressing_type_;
        processor_type_ = one.processor_type_;
        processor_id_ = one.processor_id_;

        (void)payload_.assign(one.payload_.begin(), one.payload_.end());
    }
    return *this;
}

const uds_transport::ByteVector& UdsMessage::GetPayload() const
{
    return payload_;
}

uds_transport::ByteVector& UdsMessage::GetPayload()
{
    return payload_;
}

UdsMessage::Address UdsMessage::GetSa() const noexcept
{
    return source_address_;
}

UdsMessage::Address UdsMessage::GetTa() const noexcept
{
    return target_address_;
}

UdsMessage::TargetAddressType UdsMessage::GetTaType() const noexcept
{
    return addressing_type_;
}

void UdsMessage::SetTaType(const UdsMessage::TargetAddressType type) noexcept
{
    addressing_type_ = type;
}

UdsMessage::ProcessorType UdsMessage::GetProcessorType() const noexcept
{
    return processor_type_;
}

void UdsMessage::SetProcessorType(const UdsMessage::ProcessorType type) noexcept
{
    processor_type_ = type;
}

UdsMessage::ProcessorId UdsMessage::GetProcessorId() const noexcept
{
    return processor_id_;
}

void UdsMessage::SetProcessorId(const UdsMessage::ProcessorId id) noexcept
{
    processor_id_ = id;
}

} /* namespace uds_transport */
} /* namespace diag */
} /* namespace asf */
