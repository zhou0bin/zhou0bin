#ifndef SOURCE_DCM_ROUTE_ROUTING_PRIPRO_COMMUNICATION_H_
#define SOURCE_DCM_ROUTE_ROUTING_PRIPRO_COMMUNICATION_H_

#include <mutex>

#include "dcm/route/routing/routing_pripro_client.h"

#include "dcm/route/routing/routing_pripro_server.h"

#define DIAG_DOIP_REQUEST "/tmp/DiagDoipClientRequest.sock"
#define DIAG_DOIP_RESPONSE "/tmp/DiagDoipServerResponse.sock"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class RoutingPriproCommunication
{
public:
    using PriproMessageCallback = std::function<void(const uint16_t, const uint16_t, const uint16_t, const std::vector<uint8_t>& )>;

public:
    explicit RoutingPriproCommunication();

    virtual ~RoutingPriproCommunication() = default;

    void StartPriproCommunication();

    void StopPriproCommunication();

    bool ConnectPriproCommunication();

    bool SendPriproMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& uds);

    inline void RegisterPriproMessageCallback(const PriproMessageCallback fun)
    {
        callback_ = fun;
    }

protected:
    bool TransmitPriproMessage(const std::vector<uint8_t>& msg);

    void ReceivePriproMessage(std::string_view str);

    void HandlePriproMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& uds);

private:
    RoutingPriproClient client_;
    RoutingPriproServer server_;

    std::mutex mutex_;

    PriproMessageCallback callback_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif //SOURCE_DCM_ROUTE_ROUTING_PRIPRO_COMMUNICATION_H_
