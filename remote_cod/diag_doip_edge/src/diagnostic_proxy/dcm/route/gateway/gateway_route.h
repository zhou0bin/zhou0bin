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

#ifndef DCM_ROUTE_GATEWAY_ROUTE_H
#define DCM_ROUTE_GATEWAY_ROUTE_H

#include "dcm/route/route.h"

#include "dcm/route/gateway/gateway_processor.h"
#include "dcm/route/gateway/gateway_sync.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class Routing;

class GatewayRoute : public Route
{
public:
    using GatewayProcessors = std::map<uint8_t, GatewayProcessor::Ptr>;
    using RoutingMap = std::map<uint16_t, uint8_t>;
    using TransmitMap = std::map<uint16_t, uint8_t>;

public:
    /**
     * \brief Constructor for GatewayRoute. 
     */
    explicit GatewayRoute(Routing& routing, const std::string& config, const uint8_t type);
    virtual ~GatewayRoute() override = default;

    virtual void Initialize() override;
    virtual void Deinitialize() override;

    virtual bool CheckAndHandleMessage(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message) override;

    bool GatewayRouteSendMessage(const uint8_t processor_id, const uds_transport::UdsMessageExt& message);

    void GatewayRouteSendMessage(const uds_transport::UdsMessageExt& message);

    void GatewayRouteSendCustomMessage(const uds_transport::ByteVector& message);

    void GatewayTransmitDiagnosticMsg(const uint8_t conversation_id, const uint16_t source_address, const uds_transport::ByteVector& payload);

    void GatewayTransmitDiagnosticAck(const uint8_t conversation_id, const uint16_t source_address);

    void GatewayTransmitDiagnosticNack(const uint8_t conversation_id, const uint16_t source_address, const uint8_t nack_code);

    uint8_t GetTransmitConversationId(const uint16_t processor_address);

    void GetewayRoutingStateChange(const bool state);

    void GatewayRoutingUpdated(const uds_transport::ByteVector& payload);

    void GatewayRoutingUpdated(const uint16_t address, const uint8_t processor_id);

    Routing& GetRouting()
    {
        return routing_;
    }

protected:
    void GatewayProcessorInit(const uint8_t processor_id);

    void GatewayRoutingInit();

    void GatewayCreateRoutingTimer(const uint16_t processor_address);

    void HandleRoutingFinish(const uint16_t processor_address);

    uint16_t GetGatewayRoutingProcessorId(const uint16_t processor_address);

    void SetGatewayRoutingProcessorId(const uint16_t processor_address, const uint8_t processor_id);

    void SetTransmitConversationId(const uint16_t processor_address, const uint8_t conversation_id);

    void SetTransmitConversationId(const uint8_t conversation_id);

    void EraseTransmitConversationId(const uint16_t processor_address);

    void SetLastTransmitConversationId(const uint8_t conversation_id);

    uint8_t GetLastTransmitConversationId();

    void GatewayCreateArbitrateTimer(const uint16_t processor_address);

    void GatewayArbitrateAddressFinish(const uint16_t processor_address);

    void HandleArbitrateMessage(const uint16_t processor_address, const uint8_t type, const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void GatewayRouteSendCacheMessage(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void GatewayRouteCacheMessagePending(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void GatewayRouteDiscardCacheMessage(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void GatewayRouteRoutingIsClosed(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

private:
    void CteateGatewayProcessor(const uint8_t processor_id);
    GatewayProcessor::Ptr GetGatewayProcessor(const uint8_t processor_id);

    explicit GatewayRoute(GatewayRoute&&) = delete;
    GatewayRoute(const GatewayRoute&) = delete;
    GatewayRoute& operator=(const GatewayRoute&) = delete;
    GatewayRoute& operator=(GatewayRoute&&) = delete;

protected:
    /**
     * \brief The Routing used by the route.
     */
    Routing& routing_;

private:
    std::string config_;

    GatewayProcessors processors_;

    RoutingMap routing_map_;

    TransmitMap transmit_map_;

    std::mutex mutex_;

    std::mutex routing_mutex_;

    std::mutex transmit_mutex_;

    std::atomic<uint8_t> last_transmit_cid_;

    GatewaySync gateway_sync_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif /* DCM_ROUTE_GATEWAY_ROUTE_H */
