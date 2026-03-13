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

#ifndef DCM_ROUTE_PARTITION_PROCESSOR_H_
#define DCM_ROUTE_PARTITION_PROCESSOR_H_

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

class PartitionRoute;

class PartitionProcessor
{
public:
    /**
     * \brief A shared pointer to a PartitionProcessor object.
     */
    typedef std::shared_ptr<PartitionProcessor> Ptr;

    using CacheMessage = std::tuple<uint8_t, uds_transport::UdsMessageExt>;

public:
    /**
     * \brief Constructor for PartitionProcessor. 
     */
    explicit PartitionProcessor(PartitionRoute& route, const std::string& config, const uint16_t id);
    virtual ~PartitionProcessor() = default;

    void Initialize();

    void Deinitialize();

    bool PartitionSendMessage(const uds_transport::UdsMessageExt& message);

    bool PartitionSendSyncMessage(const uds_transport::UdsMessageExt& message);

    uint16_t PartitionTa();

protected:
    void HandleMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const uds_transport::ByteVector& payload);

    void PartitionStartReceving(const uds_transport::UdsMessageExt& message);

    void PartitionStopReceving(const uds_transport::UdsMessageExt& message);

    void PartitionCheckDiagnosticAck(const uint16_t source_address, const uint16_t target_address);

    void PartitionCheckDiagnosticNack(const uint16_t source_address, const uint16_t target_address);

    void PartitionCheckDiagnosticMsg(const uint16_t source_address, const uint16_t target_address, const uint8_t sid);

    void PartitionCheckResponsePending(const uint16_t source_address, const uint16_t target_address, const uint8_t sid);

private:
    explicit PartitionProcessor(PartitionProcessor&&) = delete;
    PartitionProcessor(const PartitionProcessor&) = delete;
    PartitionProcessor& operator=(const PartitionProcessor&) = delete;
    PartitionProcessor& operator=(PartitionProcessor&&) = delete;

private:
    PartitionRoute& route_;

    std::string config_;

    uint16_t id_;
 
    uint16_t logical_address_;
 
    uint16_t diagnostic_address_;
 
    uint16_t functional_address_;

    std::mutex mutex_;

    std::shared_ptr<RoutingConnection> proxy_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif /* DCM_ROUTE_PARTITION_PROCESSOR_H_ */
