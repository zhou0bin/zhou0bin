#ifndef SOURCE_DCM_ROUTE_ROUTING_CONNECTION_H_
#define SOURCE_DCM_ROUTE_ROUTING_CONNECTION_H_

#include <cstdint>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <string>
#include <queue>
#include <vector>
#include <functional>
#include <arpa/inet.h>

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class RoutingConnection
{
public:
    /**
     * \brief A shared pointer to a RoutingConnection object.
     */
    typedef std::shared_ptr<RoutingConnection> Ptr;

    using MessageCallback = std::function<void(const uint16_t , const uint16_t , const uint16_t , const std::vector<uint8_t>& )>;

    /**
     * \brief Constructor for RoutingConnection. 
     */
    explicit RoutingConnection(const std::string& connection_name);
    virtual ~RoutingConnection() = default;

    explicit RoutingConnection(RoutingConnection&&) = delete;
    RoutingConnection(const RoutingConnection&) = delete;
    RoutingConnection& operator=(const RoutingConnection&) = delete;
    RoutingConnection& operator=(RoutingConnection&&) = delete;

    inline void RegisterMessageCallback(const MessageCallback fun)
    {
        callback_ = fun;
    }

    virtual void Offer() = 0;

    virtual void StopOffer() = 0;

    virtual bool ConnectServer() = 0;

    virtual bool SendDiagnosticMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload) = 0;

    virtual bool SendCustomMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload) = 0;

protected:
    std::string connection_name_;

    MessageCallback callback_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_ROUTE_ROUTING_CONNECTION_H_
