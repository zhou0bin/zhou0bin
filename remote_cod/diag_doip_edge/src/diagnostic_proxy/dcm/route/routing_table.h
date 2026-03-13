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

#ifndef DCM_ROUTE_ROUTING_TABLE_H_
#define DCM_ROUTE_ROUTING_TABLE_H_

#include "dcm/route/route.h"

#include "diag_proxy_common.h"

// #include "ara/core/map.h"
#include <map>

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

/**
 * \brief Class for Routing dispatching.
 */
class RoutingTable
{
public:

    typedef struct
    {
        Route::Ptr route; /**< \brief The route of this entry. */
    } Entry;

    using RouteMap = std::map<uint8_t, Entry>;

    using RoutingMap = std::map<uint16_t, uint8_t>;

public:
    explicit RoutingTable();
    virtual ~RoutingTable() = default;

    void AddRoute(const uint8_t type, const Route::Ptr route);
    Route::Ptr GetRoute(const uint8_t type) const;

    void AddGateway(const uint16_t processor_address, const uint8_t processor_id);

    uint8_t GetGatewayProcessorId(const uint16_t processor_address);

    bool IsGatewayAddress(const uint16_t processor_address);

    RoutingMap GetGatewayMap();

    void AddNode(const uint16_t processor_address, const uint8_t processor_id);

    uint8_t GetNodeProcessorId(const uint16_t processor_address);

    bool IsNodeAddress(const uint16_t processor_address);

    RoutingMap GetNodeMap();

    void AddPartition(const uint16_t processor_address, const uint16_t processor_id);

    uint8_t GetPartitionProcessorId(const uint16_t processor_address);

    RoutingMap GetPartitionMap();

    void AddServiceRoutingTableBySid(const uint16_t id, const uint8_t processor_id);

    uint8_t GetServiceRoutingTableBySid(const uint16_t id);

    void AddServiceRoutingTableByDidToReadDid(const uint16_t id, const uint8_t processor_id);

    uint8_t GetServiceRoutingTableByDidToReadDid(const uint16_t id);

    void AddServiceRoutingTableByDidToWriteDid(const uint16_t id, const uint8_t processor_id);

    uint8_t GetServiceRoutingTableByDidToWriteDid(const uint16_t id);

    void AddServiceRoutingTableByRid(const uint16_t id, const uint8_t processor_id);

    uint8_t GetServiceRoutingTableByRid(const uint16_t id);

    uint8_t GetProcessorByServiceRoutingTable(const uds_transport::UdsMessageExt& message);

private:
    void TransferServiceRouting(const uint8_t sid, const uint16_t sa, uint8_t& processor_id);

    explicit RoutingTable(RoutingTable&&) = delete;
    RoutingTable(RoutingTable&) = delete;
    RoutingTable& operator=(RoutingTable&) = delete;
    RoutingTable& operator=(RoutingTable&&) = delete;

private:
    RouteMap route_map_;

    RoutingMap gateway_map_;
    RoutingMap node_map_;
    RoutingMap partition_map_;
    RoutingMap sid_map_;
    RoutingMap read_did_map_;
    RoutingMap write_did_map_;
    RoutingMap rid_map_;
    RoutingMap transfer_map_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // DCM_ROUTE_ROUTING_TABLE_H_
