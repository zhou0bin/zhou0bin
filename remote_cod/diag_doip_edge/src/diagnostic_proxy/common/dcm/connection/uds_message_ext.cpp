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

#include "uds_transport/uds_message_ext.h"

#include "asf/diag/uds_transport/protocol_types.h"
using asf::diag::uds_transport::ByteVector;

namespace asf
{
namespace diag
{
namespace uds_transport
{

// ISO 14229-1:2013, Table 2
constexpr static uint8_t s_negativeResponseSid{0x7f};
constexpr static uint8_t s_positiveResponseSidOffset{0x40};

// ISO 14229-1:2013, Table 10
enum class RequestPduData : int
{
    ServiceRequestSidOffset = 0,
    DataOffset = 1
};
// ISO 14229-1:2013, Table 9
enum class SubFunctionRequestPduData : int
{
    ServiceRequestSidOffset = 0,
    SubFunctionOffset = 1,
    DataOffset = 2
};
// ISO 14229-1:2013, Table 3
enum class NegativeResponsePduData : int
{
    NegativeResponseSidOffset = 0,
    ServiceRequestSidOffset = 1,
    ResponseCodeOffset = 2
};
// ISO 14229-1:2013, Table 16
enum class PositiveResponsePduData : int
{
    ServiceResponseSidOffset = 0,
    DataOffset = 1
};
enum class SubFunctionPositiveResponsePduData : int
{
    ServiceResponseSidOffset = 0,
    SubFunctionOffset = 1,
    DataOffset = 2
};

constexpr static bool IsNegativeResponseSid(const int sid) noexcept
{
    return sid == s_negativeResponseSid;
}

constexpr static bool IsPositiveResponseSid(const int sid) noexcept
{
    // ISO 14229-1:2013, Table 2
    return ((sid >= 0x50) && (sid <= 0x7E)) || ((sid >= 0xC3) && (sid <= 0xC8)) || ((sid >= 0xFA) && (sid <= 0xFE));
}

constexpr static bool HasSubFunctionField(const int sid) noexcept
{
    return ((sid == 0x10)   // SessionControl
        || (sid == 0x11)    // EcuReset
        || (sid == 0x27)    // SecurityAccess
        || (sid == 0x28)    // CommunicationControl
        || (sid == 0x29)    // Authentication
        || (sid == 0x38)    // FileTransfer
        || (sid == 0x3E)    // TesterPresent
        || (sid == 0x83)    // AccessTimingParameter
        || (sid == 0x85)    // ControlDtcSetting
        || (sid == 0x86)    // ResponseOnEvent
        || (sid == 0x87)    // LinkControl
        || (sid == 0x2C)    // DynamicallyDefineDataIdentifier
        || (sid == 0x19)    // ReadDtcInformation
        || (sid == 0x31));  // RoutineControl
}

UdsMessageExt::UdsMessageExt(const Address source_address,
    const Address target_address,
    const uds_transport::ByteVector& payload)
    : UdsMessage(source_address, target_address, payload)
{}

bool UdsMessageExt::IsNegativeResponseMessage() const
{
    const auto& payload = GetPayload();

    return ((!payload.empty())
        && IsNegativeResponseSid(static_cast<int>(payload.at(static_cast<int>(NegativeResponsePduData::NegativeResponseSidOffset)))));
}

bool UdsMessageExt::IsPositiveResponseMessage() const
{
    const auto& payload = GetPayload();

    return ((!payload.empty())
        && IsPositiveResponseSid(static_cast<int>(payload.at(static_cast<int>(PositiveResponsePduData::ServiceResponseSidOffset)))));
}

//this function will return the request sid associated with this message when the message is a P/N response 
//eg. if the message's sid is 7E, then the return value of this message is 3e which is 7e - 0x40.
std::optional<uint8_t> UdsMessageExt::GetRequestSid() const
{
    const auto& payload = GetPayload();
    std::optional<uint8_t> sid;
    using size_type = std::decay<decltype(payload)>::type::size_type;

    if (IsNegativeResponseMessage()) {
        const auto offset = static_cast<size_type>(NegativeResponsePduData::ServiceRequestSidOffset);
        if (payload.size() > offset) {
            sid = payload[offset];
        }
    } else if (IsPositiveResponseMessage()) {
        const auto offset = static_cast<size_type>(PositiveResponsePduData::ServiceResponseSidOffset);
        if (payload.size() > offset) {
            sid = payload[offset] - 0x40U;
        }
    } else {
        const auto offset = static_cast<size_type>(RequestPduData::ServiceRequestSidOffset);
        if (payload.size() > offset) {
            sid = payload[offset];
        }
    }

    return sid;
}

std::optional<uint8_t> UdsMessageExt::GetMessageSid() const
{
    const auto& payload = GetPayload();
    std::optional<uint8_t> sid;
    using size_type = std::decay<decltype(payload)>::type::size_type;

    const auto offset = static_cast<size_type>(RequestPduData::ServiceRequestSidOffset);
    if (payload.size() > offset) {
        sid = payload[offset];
    }

    return sid;
}

std::optional<uint8_t> UdsMessageExt::GetNrc() const
{
    const auto& payload = GetPayload();
    std::optional<uint8_t> nrc;
    using size_type = std::decay<decltype(payload)>::type::size_type;

    if (IsNegativeResponseMessage()) {
        const auto offset = static_cast<size_type>(NegativeResponsePduData::ResponseCodeOffset);
        if (payload.size() > offset) {
            nrc = payload[offset];
        }
    }

    return nrc;
}

bool UdsMessageExt::ShouldSuppressPositiveResponse() const
{
    const auto& payload = GetPayload();

    if ((!IsNegativeResponseMessage()) && (!IsPositiveResponseMessage())) {
        auto sid = GetRequestSid();
        if (sid.has_value() && HasSubFunctionField(static_cast<int>(*sid))) {
            using size_type = std::decay<decltype(payload)>::type::size_type;
            const auto offset = static_cast<size_type>(SubFunctionRequestPduData::SubFunctionOffset);
            // Check the bit
            return (payload.size() > offset) && ((payload[offset] & 0x80U) == 0x80U);
        }
        // Don't suppress, if no sub-function field (e.g. service doesn't have sub-functions)
        return false;
    }

    // Suppress, because it's non-sense to have response for response message
    return true;
}

std::optional<uint8_t> UdsMessageExt::GetSubFunction() const
{
    const auto& payload = GetPayload();
    std::optional<uint8_t> subFunction;
    using size_type = std::decay<decltype(payload)>::type::size_type;

    if (!IsNegativeResponseMessage()) {
        auto sid = GetRequestSid();
        if (sid.has_value() && HasSubFunctionField(static_cast<int>(*sid))) {
            if (IsPositiveResponseMessage()) {
                const auto offset = static_cast<size_type>(SubFunctionPositiveResponsePduData::SubFunctionOffset);
                if (payload.size() > offset) {
                    subFunction = payload[offset] & 0x7FU;
                }
            } else {
                const auto offset = static_cast<size_type>(SubFunctionRequestPduData::SubFunctionOffset);
                if (payload.size() > offset) {
                    subFunction = payload[offset] & 0x7FU;
                }
            }
        }
    }

    return subFunction;
}

} /* namespace uds_transport */
} /* namespace diag */
} /* namespace asf */
