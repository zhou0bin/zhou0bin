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

#ifndef DCM_ROUTE_PARTITION_ROUTE_H
#define DCM_ROUTE_PARTITION_ROUTE_H

#include "dcm/route/route.h"

#include "dcm/route/partition/partition_processor.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class Routing;

class PartitionRoute : public Route
{
public:
    using PartitionProcessors = std::map<uint8_t, PartitionProcessor::Ptr>;
    using RoutingMap = std::map<uint16_t, uint8_t>;
    using TransmitMap = std::map<uint16_t, uint8_t>;

public:
    /**
     * \brief Constructor for PartitionRoute. 
     */
    explicit PartitionRoute(Routing& routing, const std::string& config, const uint8_t type);
    virtual ~PartitionRoute() override = default;

    virtual void Initialize() override;
    virtual void Deinitialize() override;

    virtual bool CheckAndHandleMessage(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message) override;

    bool PartitionRouteSendMessage(const uint8_t processor_id, const uds_transport::UdsMessageExt& message);

    void PartitionRouteSendMessage(const uds_transport::UdsMessageExt& message);

    void PartitionRouteSendSyncMessage(const uds_transport::UdsMessageExt& message);

    void PartitionTransmitDiagnosticMsg(const uint8_t processor_id, const uint8_t conversation_id, const uint16_t source_address, const uint8_t sid, const uds_transport::ByteVector& payload);

    void PartitionTransmitDiagnosticAck(const uint8_t processor_id, const uint8_t conversation_id, const uint16_t source_address, const uint8_t sid);

    void PartitionTransmitDiagnosticNack(const uint8_t processor_id, const uint8_t conversation_id, const uint16_t source_address, const uint8_t sid, const uint8_t nack_code);

    uint8_t GetTransmitConversationId(const uint16_t source_address);

    Routing& GetRouting()
    {
        return routing_;
    }

protected:
    void PartitionProcessorInit(const uint8_t processor_id, const uint16_t processor_address);

    void PartitionCreateRoutingTimer(const uint16_t processor_address);

    void HandleRoutingFinish(const uint16_t processor_address);

    void SetTransmitConversationId(const uint16_t processor_address, const uint8_t conversation_id);

    void EraseTransmitConversationId(const uint16_t processor_address);

    void SetLastTransmitConversationId(const uint8_t conversation_id);

    uint8_t GetLastTransmitConversationId();

    void PartitionCreateArbitrateTimer(const uint16_t processor_address);

    void PartitionArbitrateAddressFinish(const uint16_t processor_address);

    void HandleArbitrateMessage(const uint16_t processor_address, const uint8_t type, const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void PartitionRouteSendCacheMessage(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void PartitionRouteCacheMessagePending(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void PartitionRouteDiscardCacheMessage(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void PartitionRouteRoutingIsClosed(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

private:
    uint16_t GetPartitionTa(const uint8_t processor_id);

    void CteatePartitionProcessor(const uint8_t processor_id);
    PartitionProcessor::Ptr GetPartitionProcessor(const uint8_t processor_id);

    explicit PartitionRoute(PartitionRoute&&) = delete;
    PartitionRoute(const PartitionRoute&) = delete;
    PartitionRoute& operator=(const PartitionRoute&) = delete;
    PartitionRoute& operator=(PartitionRoute&&) = delete;

protected:
    /**
     * \brief The Routing used by the route.
     */
    Routing& routing_;

private:
    std::string config_;

    PartitionProcessors processors_;

    TransmitMap transmit_map_;

    std::mutex mutex_;

    std::mutex transmit_mutex_;

    std::atomic<uint8_t> last_transmit_cid_;

    uint16_t service_address_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif /* DCM_ROUTE_PARTITION_ROUTE_H */
