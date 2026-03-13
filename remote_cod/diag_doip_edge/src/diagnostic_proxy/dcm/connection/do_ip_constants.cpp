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

#include "dcm/connection/do_ip_constants.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{
namespace doip
{

const uint16_t kTcpAliveCheckTimeout = 500;         // milliseconds
const uint16_t kTcpGeneralInactivityTimeout = 5;    // minutes  
const uint16_t kTcpInitialInactivityTimeout = 2;    // seconds
const uint16_t kActiveLineTimeout = 2;              //seconds
const uint16_t kNetworkInterfaceIdTimeout = 2;      //seconds
const uint16_t kDiagnosticMessageTimeout = 2;       //seconds

int DoIpAnnounceWait()
{
    return kDoIpAnnounceWaitGenerator->get();
}

}  // namespace doip
}  // namespace connection
}  // namespace dcm
}  // namespace diag
}  // namespace asf
