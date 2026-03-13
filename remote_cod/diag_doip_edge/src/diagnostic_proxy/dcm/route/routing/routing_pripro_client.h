#ifndef SOURCE_DCM_ROUTE_ROUTING_PRIPRO_CLIENT_H_
#define SOURCE_DCM_ROUTE_ROUTING_PRIPRO_CLIENT_H_

#include "domain_socket/client.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class RoutingPriproClient : public DomainSocket::Client
{
public:
    explicit RoutingPriproClient(std::string_view address);
    virtual ~RoutingPriproClient() = default; 

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif //SOURCE_DCM_ROUTE_ROUTING_PRIPRO_CLIENT_H_
