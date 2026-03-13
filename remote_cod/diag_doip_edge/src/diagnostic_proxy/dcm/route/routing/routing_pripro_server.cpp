#include "dcm/route/routing/routing_pripro_server.h"

using Server = DomainSocket::Server;

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

RoutingPriproServer::RoutingPriproServer(std::string_view address)
    : Server(address)
{
}

}
}
}
}
