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

#include "dcm/connection/connection.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

Connection::Connection(const ID connection_id,
    const uds_transport::UdsTransportProtocolHandlerID protocol_id)
    : connection_id_(connection_id)
    , protocol_id_(protocol_id)
    , meta_info_map_()
{
}

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
