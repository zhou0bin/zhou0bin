#ifndef SOURCE_DCM_ROUTE_ROUTING_DOIP_CONNECTION_H_
#define SOURCE_DCM_ROUTE_ROUTING_DOIP_CONNECTION_H_

#include "dcm/route/routing/routing_connection.h"

#include "diag_proxy_common.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class RoutingDoIpConnection  : public RoutingConnection
{
public:
    using MessageCallback = std::function<void(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)>;
    using CustomMessageCallback = std::function<void(const uint16_t payload_type, const std::vector<uint8_t>& paylod)>;

public:
    explicit RoutingDoIpConnection(const std::string& connection_name,
                                   const std::string& remote_ip,
                                   const uint16_t remote_port,
                                   const uint16_t port,
                                   const uint16_t diagnostic_address,
                                   const uint16_t functional_address,
                                   const uint16_t logical_address,
                                   const bool routing_activation);

    virtual ~RoutingDoIpConnection() = default;

    virtual void Offer() override;

    virtual void StopOffer() override;

    virtual bool ConnectServer() override;

    virtual bool SendDiagnosticMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload) override;

    virtual bool SendCustomMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload) override;

private:
    void SetConnectResult(const bool result);
    bool GetConnectResult();

    bool ConnectSocket();
    void CloseSocket();
    void ClearSocket();

    void ReceiveMessage();

    void ReceiveDiagnosticMessage();

    void HandleDiagnosticMessage(const uint16_t payload_type, const std::vector<uint8_t>& payload);

    void HandleCustomMessage(const uint16_t payload_type, const std::vector<uint8_t>& payload);

    void HandleMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload);

    bool SendDoipMessage(const uint16_t payload_type, const uint16_t sa, const uint16_t ta, const std::vector<uint8_t>& payload);

    void SendRoutingActivation();

    RoutingDoIpConnection(const RoutingDoIpConnection& ) = delete;
    explicit RoutingDoIpConnection(RoutingDoIpConnection&& ) = delete;
    RoutingDoIpConnection& operator=(const RoutingDoIpConnection& ) = delete;
    RoutingDoIpConnection& operator=(RoutingDoIpConnection&& ) = delete;

private:
    in_addr_t remote_ip_;
    uint16_t remote_port_;
    uint16_t port_;
    uint16_t diagnostic_address_;
    uint16_t functional_address_;
    uint16_t logical_address_;
    bool routing_activation_;
    int socket_fd_;
    int wait_timeout_;
    std::atomic_bool connect_result_;
    std::atomic_bool exit_requested_;
    std::mutex fds_mutex_;
    std::mutex mutex_;
    std::condition_variable condition_variable_;
    std::thread thread_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif //SOURCE_DCM_ROUTE_ROUTING_DOIP_CONNECTION_H_
