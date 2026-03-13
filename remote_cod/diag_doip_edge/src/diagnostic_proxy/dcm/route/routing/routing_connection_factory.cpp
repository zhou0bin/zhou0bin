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
#include "dcm/route/routing/routing_connection_factory.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

RoutingConnection::Ptr RoutingConnectionFactory::CreateConnection(const std::string &connection_name,
                                                                  const std::string &remote_ip,
                                                                  const uint16_t remote_port,
                                                                  const uint16_t port,
                                                                  const uint16_t diagnostic_address,
                                                                  const uint16_t functional_address,
                                                                  const uint16_t logical_address,
                                                                  const bool routing_activation)
{
    RoutingConnection::Ptr ptr = nullptr;
    LOG_DEBUG << __FUNCTION__ << "name: [" << connection_name << "] ip: " << remote_ip << "port: " << remote_port;
    if ((remote_ip != "0.0.0.0") && (remote_port != 0))
    {
        ptr = std::make_shared<RoutingDoIpConnection>(connection_name, remote_ip, remote_port, port, diagnostic_address, functional_address, logical_address, routing_activation);
    }
    else
    {
        ptr = std::make_shared<RoutingPriproConnection>(connection_name, logical_address);
    }
    return ptr;
}

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
