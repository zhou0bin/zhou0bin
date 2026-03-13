#include "dcm/route/routing/routing_pripro_client.h"

using Client = DomainSocket::Client;

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

RoutingPriproClient::RoutingPriproClient(std::string_view address)
    : Client(address)
{
    
}

}
}
}
}
