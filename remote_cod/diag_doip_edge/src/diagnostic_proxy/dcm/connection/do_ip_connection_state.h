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

#ifndef SOURCE_DCM_CONNECTION_DO_IP_CONNECTION_STATE_H_
#define SOURCE_DCM_CONNECTION_DO_IP_CONNECTION_STATE_H_

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

/// @brief The DoIp Connection state defined in ISO 13400-2 p.43
/// Currently only a subset of those states is used, further states may be added in the future (e.g. pending for
/// authentication state)
enum class DoIpConnectionState : uint8_t
{
    kListen = 0x00,
    kInitialized = 0x01,
    kRegistered = 0x02,
    kFinalize = 0x03
};

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_CONNECTION_DO_IP_CONNECTION_STATE_H_
