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

#ifndef DCM_ROUTE_NODE_PROCESSOR_H_
#define DCM_ROUTE_NODE_PROCESSOR_H_

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

class NodeRoute;

class NodeProcessor
{
public:
    /**
     * \brief A shared pointer to a NodeProcessor object.
     */
    typedef std::shared_ptr<NodeProcessor> Ptr;

    using CacheMessage = std::tuple<uint8_t, uds_transport::UdsMessageExt>;

public:
    /**
     * \brief Constructor for NodeProcessor. 
     */
    explicit NodeProcessor(NodeRoute& route, const std::string& config, const uint16_t id);
    virtual ~NodeProcessor() = default;

    void Initialize();

    void Deinitialize();

    bool NodeSendMessage(const uds_transport::UdsMessageExt& message);

protected:
    void HandleMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const uds_transport::ByteVector& payload);

    void NodeStartReceving(const uds_transport::UdsMessageExt& message);

    void NodeStopReceving(const uds_transport::UdsMessageExt& message);

    void NodeCheckDiagnosticAck(const uint16_t source_address, const uint16_t target_address);

    void NodeCheckDiagnosticNack(const uint16_t source_address, const uint16_t target_address);

    void NodeCheckDiagnosticMsg(const uint16_t source_address, const uint16_t target_address, const uint8_t sid);

    void NodeCheckResponsePending(const uint16_t source_address, const uint16_t target_address, const uint8_t sid);

private:
    explicit NodeProcessor(NodeProcessor&&) = delete;
    NodeProcessor(const NodeProcessor&) = delete;
    NodeProcessor& operator=(const NodeProcessor&) = delete;
    NodeProcessor& operator=(NodeProcessor&&) = delete;

private:
    NodeRoute& route_;

    std::string config_;

    uint16_t id_;

    uint16_t logical_address_;

    uint16_t diagnostic_address_;

    uint16_t functional_address_;

    std::shared_ptr<RoutingConnection> proxy_;

    std::mutex mutex_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif /* DCM_ROUTE_NODE_PROCESSOR_H_ */
