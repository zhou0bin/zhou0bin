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

#ifndef DCM_ROUTE_NODE_ROUTE_H
#define DCM_ROUTE_NODE_ROUTE_H

#include "dcm/route/route.h"

#include "dcm/route/node/node_processor.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class Routing;

class NodeRoute : public Route
{
public:
    using NodeProcessors = std::map<uint8_t, NodeProcessor::Ptr>;
    using RoutingMap = std::map<uint16_t, uint8_t>;
    using TransmitMap = std::map<uint16_t, uint8_t>;

public:
    /**
     * \brief Constructor for NodeRoute. 
     */
    explicit NodeRoute(Routing& routing, const std::string& config, const uint8_t type);
    virtual ~NodeRoute() override = default;

    virtual void Initialize() override;
    virtual void Deinitialize() override;

    virtual bool CheckAndHandleMessage(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message) override;

    bool NodeRouteSendMessage(const uint8_t processor_id, const uds_transport::UdsMessageExt& message);

    void NodeRouteSendMessage(const uds_transport::UdsMessageExt& message);

    void NodeTransmitDiagnosticMsg(const uint8_t conversation_id, const uint16_t source_address, const uds_transport::ByteVector& payload);

    void NodeTransmitDiagnosticAck(const uint8_t conversation_id, const uint16_t source_address);

    void NodeTransmitDiagnosticNack(const uint8_t conversation_id, const uint16_t source_address, const uint8_t nack_code);

    uint8_t GetTransmitConversationId(const uint16_t source_address);

    Routing& GetRouting()
    {
        return routing_;
    }

protected:
    void NodeProcessorInit(const uint8_t processor_id, const uint16_t processor_address);

    void NodeCreateRoutingTimer(const uint16_t processor_address);

    void HandleRoutingFinish(const uint16_t processor_address);

    void SetTransmitConversationId(const uint16_t processor_address, const uint8_t conversation_id);

    void SetTransmitConversationId(const uint8_t conversation_id);

    void EraseTransmitConversationId(const uint16_t processor_address);

    void SetLastTransmitConversationId(const uint8_t conversation_id);

    uint8_t GetLastTransmitConversationId();

    void NodeCreateArbitrateTimer(const uint16_t processor_address);

    void NodeArbitrateAddressFinish(const uint16_t processor_address);

    void HandleArbitrateMessage(const uint16_t processor_address, const uint8_t type, const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void NodeRouteSendCacheMessage(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void NodeRouteCacheMessagePending(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void NodeRouteDiscardCacheMessage(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

private:
    void CteateNodeProcessor(const uint8_t processor_id);
    NodeProcessor::Ptr GetNodeProcessor(const uint8_t processor_id);

    explicit NodeRoute(NodeRoute&&) = delete;
    NodeRoute(const NodeRoute&) = delete;
    NodeRoute& operator=(const NodeRoute&) = delete;
    NodeRoute& operator=(NodeRoute&&) = delete;

protected:
    /**
     * \brief The Routing used by the route.
     */
    Routing& routing_;

private:
    std::string config_;

    NodeProcessors processors_;

    TransmitMap transmit_map_;

    std::mutex mutex_;

    std::mutex transmit_mutex_;

    std::atomic<uint8_t> last_transmit_cid_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif /* DCM_ROUTE_NODE_ROUTE_H */
