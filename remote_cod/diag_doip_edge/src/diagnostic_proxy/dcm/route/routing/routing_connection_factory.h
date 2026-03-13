#ifndef SOURCE_DCM_ROUTE_ROUTING_CONNECTION_FACTORY_H_
#define SOURCE_DCM_ROUTE_ROUTING_CONNECTION_FACTORY_H_

#include "dcm/route/routing/routing_do_ip_connection.h"

#include "dcm/route/routing/routing_pripro_connection.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class RoutingConnectionFactory
{
public:
    static RoutingConnection::Ptr CreateConnection(const std::string &connection_name,
                                                   const std::string &remote_ip,
                                                   const uint16_t remote_port,
                                                   const uint16_t port,
                                                   const uint16_t diagnostic_address,
                                                   const uint16_t functional_address,
                                                   const uint16_t logical_address,
                                                   const bool routing_activation);

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_ROUTE_ROUTING_CONNECTION_FACTORY_H_
