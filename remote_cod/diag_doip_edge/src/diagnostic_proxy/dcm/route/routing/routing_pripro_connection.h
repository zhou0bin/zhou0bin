#ifndef SOURCE_DCM_ROUTE_ROUTING_PRIPRO_CONNECTION_H_
#define SOURCE_DCM_ROUTE_ROUTING_PRIPRO_CONNECTION_H_

#include "dcm/route/routing/routing_connection.h"

#include "dcm/route/routing/routing_pripro_transmision_protocol.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class RoutingPriproConnection : public RoutingConnection
{
public:
    explicit RoutingPriproConnection(const std::string& connection_name, const uint16_t logical_address);

    virtual ~RoutingPriproConnection() = default;

    virtual void Offer() override;

    virtual void StopOffer() override;

    virtual bool ConnectServer() override;

    virtual bool SendDiagnosticMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload) override;

    virtual bool SendCustomMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload) override;

private:
    void SetConnectResult(const bool result);

    bool GetConnectResult();

    bool ConnectSocket();

    void ReceiveMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t> &payload);

    void HandleDiagnosticMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload);

    void HandleCustomMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload);

    void HandleMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload);

    RoutingPriproConnection(const RoutingPriproConnection& ) = delete;
    explicit RoutingPriproConnection(RoutingPriproConnection&& ) = delete;
    RoutingPriproConnection& operator=(const RoutingPriproConnection& ) = delete;
    RoutingPriproConnection& operator=(RoutingPriproConnection&& ) = delete;

private:
    uint16_t logical_address_;

    std::atomic_bool connect_result_;

    std::shared_ptr<RoutingPriproCommunication> proxy_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif //SOURCE_DCM_ROUTE_ROUTING_PRIPRO_CONNECTION_H_
