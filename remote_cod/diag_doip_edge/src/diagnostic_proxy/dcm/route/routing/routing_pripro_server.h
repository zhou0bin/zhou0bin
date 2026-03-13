#ifndef SOURCE_DCM_ROUTE_ROUTING_PRIPRO_SERVER_H_
#define SOURCE_DCM_ROUTE_ROUTING_PRIPRO_SERVER_H_

#include "domain_socket/server.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class RoutingPriproServer : public DomainSocket::Server
{
public:
    explicit RoutingPriproServer(std::string_view address);
    virtual ~RoutingPriproServer() = default;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif //SOURCE_DCM_ROUTE_ROUTING_PRIPRO_SERVER_H_
