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

#include "dcm/route/routing/routing_connection.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

RoutingConnection::RoutingConnection(const std::string& connection_name)
    : connection_name_(connection_name)
    , callback_(nullptr)
{
}

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
