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

#include "dcm/route/routing_table.h"

#include "dcm/connection/uds_services.h"
using asf::diag::dcm::connection::UdsServices;
#include "uds_transport/uds_message_ext.h"
using asf::diag::uds_transport::UdsMessageExt;
using asf::diag::uds_transport::ByteVector;
#include "common/multi_byte_type.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

RoutingTable::RoutingTable()
    : route_map_()
    , gateway_map_()
    , node_map_()
    , partition_map_()
    , sid_map_()
    , read_did_map_()
    , write_did_map_()
    , rid_map_()
    , transfer_map_()
{
}

void RoutingTable::AddRoute(const uint8_t type, const Route::Ptr route)
{
    if (route_map_.find(type) != route_map_.end()) {
        LOG_ERROR << __FUNCTION__ << "already set for type. ";
        return;
    }
    Entry entry {route};
    (void)route_map_.insert(RouteMap::value_type(type, entry));
}

Route::Ptr RoutingTable::GetRoute(const uint8_t type) const
{
    const auto it(route_map_.find(type));
    if (it != route_map_.end()) {
        return it->second.route;
    }
    return nullptr;
}

void RoutingTable::AddGateway(const uint16_t processor_address, const uint8_t processor_id)
{
    if (processor_address == 0) {
        return;
    }
    if (gateway_map_.find(processor_address) != gateway_map_.end()) {
        LOG_ERROR << __FUNCTION__ << "already set for address. ";
        return;
    }
    (void)gateway_map_.insert(RoutingMap::value_type(processor_address, processor_id));
}

uint8_t RoutingTable::GetGatewayProcessorId(const uint16_t processor_address)
{
    const auto it(gateway_map_.find(processor_address));
    if (it != gateway_map_.end()) {
        return it->second;
    }
    return 0x00;
}

bool RoutingTable::IsGatewayAddress(const uint16_t processor_address)
{
    if (gateway_map_.find(processor_address) != gateway_map_.end()) {
        return true;
    }
    return false;
}

RoutingTable::RoutingMap RoutingTable::GetGatewayMap()
{
    return gateway_map_;
}

void RoutingTable::AddNode(const uint16_t processor_address, const uint8_t processor_id)
{
    if (processor_address == 0) {
        return;
    }
    if (node_map_.find(processor_address) != node_map_.end()) {
        LOG_ERROR << __FUNCTION__ << "already set for address. ";
        return;
    }
    (void)node_map_.insert(RoutingMap::value_type(processor_address, processor_id));
}

uint8_t RoutingTable::GetNodeProcessorId(const uint16_t processor_address)
{
    const auto it(node_map_.find(processor_address));
    if (it != node_map_.end()) {
        return it->second;
    }
    return 0x00;
}

bool RoutingTable::IsNodeAddress(const uint16_t processor_address)
{
    if (node_map_.find(processor_address) != node_map_.end()) {
        return true;
    }
    return false;
}

RoutingTable::RoutingMap RoutingTable::GetNodeMap()
{
    return gateway_map_;
}

void RoutingTable::AddPartition(const uint16_t processor_address, const uint16_t processor_id)
{
    if (processor_address == 0) {
        return;
    }
    if (partition_map_.find(processor_address) != partition_map_.end()) {
        LOG_ERROR << __FUNCTION__ << "already set for address. ";
        return;
    }
    (void)partition_map_.insert(RoutingMap::value_type(processor_address, processor_id));
}

uint8_t RoutingTable::GetPartitionProcessorId(const uint16_t processor_address)
{
    const auto it(partition_map_.find(processor_address));
    if (it != partition_map_.end()) {
        return it->second;
    }
    return 0x00;
}

RoutingTable::RoutingMap RoutingTable::GetPartitionMap()
{
    return partition_map_;
}

void RoutingTable::AddServiceRoutingTableBySid(const uint16_t id, const uint8_t processor_id)
{
    if (id == 0) {
        return;
    }
    if (sid_map_.find(id) != sid_map_.end()) {
        LOG_ERROR << __FUNCTION__ << "already set for SID. ";
        return;
    }
    (void)sid_map_.insert(RoutingMap::value_type(id, processor_id));
}

uint8_t RoutingTable::GetServiceRoutingTableBySid(const uint16_t id)
{
    const auto it(sid_map_.find(id));
    if (it != sid_map_.end()) {
        return it->second;
    }
    return 0;
}

void RoutingTable::AddServiceRoutingTableByDidToReadDid(const uint16_t id, const uint8_t processor_id)
{
    if (id == 0) {
        return;
    }
    if (read_did_map_.find(id) != read_did_map_.end()) {
        LOG_ERROR << __FUNCTION__ << "already set for DID. ";
        return;
    }
    (void)read_did_map_.insert(RoutingMap::value_type(id, processor_id));
}

uint8_t RoutingTable::GetServiceRoutingTableByDidToReadDid(const uint16_t id)
{
    const auto it(read_did_map_.find(id));
    if (it != read_did_map_.end()) {
        return it->second;
    }
    return 0;
}

void RoutingTable::AddServiceRoutingTableByDidToWriteDid(const uint16_t id, const uint8_t processor_id)
{
    if (id == 0) {
        return;
    }
    if (write_did_map_.find(id) != write_did_map_.end()) {
        LOG_ERROR << __FUNCTION__ << "already set for DID. ";
        return;
    }
    (void)write_did_map_.insert(RoutingMap::value_type(id, processor_id));
}

uint8_t RoutingTable::GetServiceRoutingTableByDidToWriteDid(const uint16_t id)
{
    const auto it(write_did_map_.find(id));
    if (it != write_did_map_.end()) {
        return it->second;
    }
    return 0;
}

void RoutingTable::AddServiceRoutingTableByRid(const uint16_t id, const uint8_t processor_id)
{
    if (id == 0) {
        return;
    }
    if (rid_map_.find(id) != rid_map_.end()) {
        LOG_ERROR << __FUNCTION__ << "already set for RID. ";
        return;
    }
    (void)rid_map_.insert(RoutingMap::value_type(id, processor_id));
}

uint8_t RoutingTable::GetServiceRoutingTableByRid(const uint16_t id)
{
    const auto it(rid_map_.find(id));
    if (it != rid_map_.end()) {
        return it->second;
    }
    return 0;
}

uint8_t RoutingTable::GetProcessorByServiceRoutingTable(const UdsMessageExt& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    using Address = asf::diag::uds_transport::UdsMessage::Address;
    const Address source_address = message.GetSa();
    const auto sid = message.GetRequestSid().value_or(0);
    LOG_DEBUG << __FUNCTION__ << "Sid: " << LOG_HEX(sid);
    const ByteVector& payload = message.GetPayload();
    uint8_t processor_id {0U};
    if (sid == UdsServices::kReadDataByIdentifier)
    {
        uint16_t data_id{0};
        if (payload.size() > 2)
        {
            SetByte(data_id, payload[1], 1);
            SetByte(data_id, payload[2], 0);
        }
        LOG_DEBUG << __FUNCTION__ << "Did: " << LOG_HEX(data_id);
        processor_id = GetServiceRoutingTableByDidToReadDid(data_id);
    }
    else if (sid == UdsServices::kWriteDataByIdentifier)
    {
        uint16_t data_id{0};
        if (payload.size() > 2)
        {
            SetByte(data_id, payload[1], 1);
            SetByte(data_id, payload[2], 0);
        }
        LOG_DEBUG << __FUNCTION__ << "Did: " << LOG_HEX(data_id);
        processor_id = GetServiceRoutingTableByDidToWriteDid(data_id);
    }
    else if (sid == UdsServices::kRoutineService)
    {
        uint16_t routine_id{0};
        if (payload.size() > 3)
        {
            SetByte(routine_id, payload[2], 1);
            SetByte(routine_id, payload[3], 0);
        }
        LOG_DEBUG << __FUNCTION__ << "Rid: " << LOG_HEX(routine_id);
        processor_id = GetServiceRoutingTableByRid(routine_id);
    }
    else
    {
        processor_id = GetServiceRoutingTableBySid(sid);
    }
    if ((UdsServices::kRequestDownload <= sid) && (sid <= UdsServices::kRequestFileTransfer))
    {
        TransferServiceRouting(sid, source_address, processor_id);
    }
    LOG_DEBUG << __FUNCTION__ << "processor_id: " << processor_id;
    return processor_id;
}

void RoutingTable::TransferServiceRouting(const uint8_t sid, const uint16_t sa, uint8_t& processor_id)
{
    const auto it(transfer_map_.find(sa));
    if (it != transfer_map_.end()) {
        if ((sid == UdsServices::kRequestDownload) || (sid == UdsServices::kRequestUpload) || (sid == UdsServices::kRequestFileTransfer)) {
            transfer_map_[sa] = processor_id;
        } else if (sid == UdsServices::kTransferData) {
            processor_id = it->second;
        } else if (sid == UdsServices::kRequestTransferExit) {
            processor_id = it->second;
            (void)transfer_map_.erase(sa);
        } else {
            LOG_VERBOSE << __FUNCTION__ << "transfer service sid invalid. ";
        }
    } else {
        if ((sid == UdsServices::kRequestDownload) || (sid == UdsServices::kRequestUpload) || (sid == UdsServices::kRequestFileTransfer)) {
            (void)transfer_map_.insert(RoutingMap::value_type(sa, processor_id));
        } else {
            LOG_VERBOSE << __FUNCTION__ << "transfer service wrong sequence. ";
        }
    }
}

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
