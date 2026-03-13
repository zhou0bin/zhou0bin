// --------------------------------------------------------------------------
// |                _     _              _____         _____                |
// |               |  \  | |            / ____|  /\   |  __ \               |
// |               | | \ | |  __       | (___   /  \  | |__) |              |
// |               | |\ \| | /__\|   |  \___ \ / /\ \ |  _  /               |
// |               | | \ \ ||    |   |   ___) / /__\ \| | \ \               |
// |               |_|  \_\| \__/ \_/|/|_____/________\_|  \_\              |
// |                                                                        |
// --------------------------------------------------------------------------
// COPYRIGHT
// --------------------------------------------------------------------------
//
// This software is copyright protected and proprietary to Neusoft Reach.
// Neusoft Reach grants to you only those rights as set out in the license 
// conditions.
// All other rights remain with Neusoft Reach.
// --------------------------------------------------------------------------

#ifndef DCM_ROUTE_DIAG_GATEWAY_PROCESSOR_H_
#define DCM_ROUTE_DIAG_GATEWAY_PROCESSOR_H_

#include "dcm/route/routing/routing_connection_factory.h"

#include "uds_transport/uds_message_ext.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class GatewayRoute;

class GatewayProcessor
{
public:
    /**
     * \brief A shared pointer to a GatewayProcessor object.
     */
    typedef std::shared_ptr<GatewayProcessor> Ptr;

    /**
     * \brief Constructor for GatewayProcessor. 
     */
    explicit GatewayProcessor(GatewayRoute& route, const std::string& config, const uint16_t id);
    virtual ~GatewayProcessor() = default;

    void Initialize();

    void Deinitialize();

    bool GatewaySendMessage(const uds_transport::UdsMessageExt& message);

    bool GatewaySendCustomMessage(const uds_transport::ByteVector& message);

protected:
    void HandleMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const uds_transport::ByteVector& payload);

    void GatewayStartReceving(const uds_transport::UdsMessageExt& message);

    void GatewayStopReceving(const uds_transport::UdsMessageExt& message);

    void GatewayCheckDiagnosticAck(const uint16_t source_address, const uint16_t target_address);

    void GatewayCheckDiagnosticNack(const uint16_t source_address, const uint16_t target_address);

    void GatewayCheckDiagnosticMsg(const uint16_t source_address, const uint16_t target_address, const uint8_t sid);

    void GatewayCheckResponsePending(const uint16_t source_address, const uint16_t target_address, const uint8_t sid);

private:
    explicit GatewayProcessor(GatewayProcessor&&) = delete;
    GatewayProcessor(const GatewayProcessor&) = delete;
    GatewayProcessor& operator=(const GatewayProcessor&) = delete;
    GatewayProcessor& operator=(GatewayProcessor&&) = delete;

private:
    GatewayRoute& route_;

    std::string config_;

    uint16_t id_;

    uint16_t logical_address_;

    uint16_t functional_address_;

    std::shared_ptr<RoutingConnection> proxy_;

    std::mutex mutex_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif /* DCM_ROUTE_DIAG_GATEWAY_PROCESSOR_H_ */
