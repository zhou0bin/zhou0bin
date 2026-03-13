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
#include "dcm/route/route_factory.h"

#include "dcm/route/route.h"
#include "dcm/route/gateway/gateway_route.h"
#include "dcm/route/node/node_route.h"
#include "dcm/route/partition/partition_route.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

Route::Ptr RouteFactory::CreateRoute(Routing& routing, const std::string& config, const uint8_t type)
{
    Route::Ptr ptr = nullptr;
    if (type == RouteType::kPartition)
    {
        ptr = std::make_shared<PartitionRoute>(routing, config, type);
    }
    else if (type == RouteType::kGateway)
    {
        ptr = std::make_shared<GatewayRoute>(routing, config, type);
    }
    else if (type == RouteType::kNode)
    {
        ptr = std::make_shared<NodeRoute>(routing, config, type);
    }
    else
    {
    }
    return ptr;
}

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
