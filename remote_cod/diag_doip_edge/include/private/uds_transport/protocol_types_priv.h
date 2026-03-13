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

#ifndef APD_ARA_DIAG_UDSTRANSPORT_PROTOCOLTYPESPRIV_HPP_
#define APD_ARA_DIAG_UDSTRANSPORT_PROTOCOLTYPESPRIV_HPP_

#include "asf/diag/uds_transport/protocol_types.h"

namespace asf
{
namespace diag
{
namespace uds_transport
{

enum UdsTransportProtocol : UdsTransportProtocolHandlerID
{
    Unknown = 0,
    DoIp,
    CanTp,
    FlexrayTp,
    J1939Tp,
    LinTp,
    UDSonNeusarNet
};

}  // namespace uds_transport
}  // namespace diag
}  // namespace asf

#endif  // APD_ARA_DIAG_UDSTRANSPORT_PROTOCOLTYPESPRIV_HPP_
