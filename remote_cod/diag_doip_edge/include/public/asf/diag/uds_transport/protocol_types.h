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
/// @brief Common Types for the UDS TransportLayer C++ Interfaces

#ifndef APD_ARA_DIAG_UDS_TRANSPORT_PROTOCOL_TYPES_H_
#define APD_ARA_DIAG_UDS_TRANSPORT_PROTOCOL_TYPES_H_

#include <cstdint>

// #include "ara/core/string.h"
// #include "ara/core/vector.h"
#include <string>
#include <vector>

namespace asf
{
namespace diag
{
namespace uds_transport
{

/// @brief The type of UDS message payloads.
using ByteVector = std::vector<uint8_t>;

using StrVector = std::vector<std::string>;

/// @brief The identifier of a logical (network) channel, over which UDS messages can be sent/received.
using ChannelID = uint32_t;

/// @brief Used by the DiagnosticServer to prioritize incoming requests.
using Priority = uint8_t;

/// @brief Used to identify the protocol kind associated to an incoming request.
using ProtocolKind = std::string;

/// @brief The identifier of an Uds Transport Protocol implementation.
using UdsTransportProtocolHandlerID = uint8_t;

using GlobalChannelIdentifier = std::tuple<UdsTransportProtocolHandlerID, ChannelID>;

}  // namespace uds_transport
}  // namespace diag
}  // namespace asf

#endif  // APD_ARA_DIAG_UDS_TRANSPORT_PROTOCOL_TYPES_H_
