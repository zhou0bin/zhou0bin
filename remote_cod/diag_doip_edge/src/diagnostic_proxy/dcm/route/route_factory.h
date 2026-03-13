#ifndef SOURCE_DCM_ROUTE_ROUTE_FACTORY_H_
#define SOURCE_DCM_ROUTE_ROUTE_FACTORY_H_

#include "dcm/route/routing.h"

#include "diag_proxy_common.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class RouteFactory
{
public:
    static Route::Ptr CreateRoute(Routing& routing, const std::string& config, const uint8_t type);

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_ROUTE_ROUTE_FACTORY_H_
