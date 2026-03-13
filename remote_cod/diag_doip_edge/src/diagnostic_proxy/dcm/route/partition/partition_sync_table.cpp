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

#include "dcm/route/partition/partition_sync_table.h"

#include "uds_transport/uds_message_ext.h"
using asf::diag::uds_transport::UdsMessageExt;
using asf::diag::uds_transport::ByteVector;

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

PartitionSyncTable::PartitionSyncTable()
    : request_map_()
    , response_map_()
    , request_mutex_()
    , response_mutex_()
{
}

void PartitionSyncTable::AddRequestMessage(const uint8_t sid, const uint8_t conversation_id, const UdsMessageExt& message)
{
    const std::lock_guard<std::mutex> locker_req(request_mutex_);
    if (request_map_.find(sid) != request_map_.end()) {
        (void)request_map_.erase(sid);
    }
    RequestInfo request_info{conversation_id, message};
    (void)request_map_.insert(RequestMap::value_type(sid, request_info));
}

void PartitionSyncTable::AddResponseMessage(const uint8_t sid, const uint8_t conversation_id, const ByteVector& payload)
{
    const std::lock_guard<std::mutex> locker_res(response_mutex_);
    if (response_map_.find(sid) != response_map_.end()) {
        (void)response_map_.erase(sid);
    }
    ResponseInfo response_info{conversation_id, payload};
    (void)response_map_.insert(ResponseMap::value_type(sid, response_info));
}

void PartitionSyncTable::EraseRequestMessage(const uint8_t sid)
{
    const std::lock_guard<std::mutex> locker_req(request_mutex_);
    if (request_map_.find(sid) != request_map_.end()) {
        (void)request_map_.erase(sid);
    }
}

void PartitionSyncTable::EraseResponseMessage(const uint8_t sid)
{
    const std::lock_guard<std::mutex> locker_res(response_mutex_);
    if (response_map_.find(sid) != response_map_.end()) {
        (void)response_map_.erase(sid);
    }
}

bool PartitionSyncTable::HasRequestMessage(const uint8_t sid)
{
    const std::lock_guard<std::mutex> locker_req(request_mutex_);
    if (request_map_.find(sid) != request_map_.end()) {
        return true;
    }
    return false;
}

bool PartitionSyncTable::HasResponseMessage(const uint8_t sid)
{
    const std::lock_guard<std::mutex> locker_res(response_mutex_);
    if (response_map_.find(sid) != response_map_.end()) {
        return true;
    }
    return false;
}

uint8_t PartitionSyncTable::GetRequestConversationId(const uint8_t sid)
{
    const std::lock_guard<std::mutex> locker_req(request_mutex_);
    const auto it(request_map_.find(sid));
    return std::get<0>(it->second);
}

uint8_t PartitionSyncTable::GetResponseConversationId(const uint8_t sid)
{
    const std::lock_guard<std::mutex> locker_res(response_mutex_);
    const auto it(response_map_.find(sid));
    return std::get<0>(it->second);
}

UdsMessageExt PartitionSyncTable::GetRequestMessage(const uint8_t sid)
{
    const std::lock_guard<std::mutex> locker_req(request_mutex_);
    const auto it(request_map_.find(sid));
    return std::get<1>(it->second);
}

ByteVector PartitionSyncTable::GetResponseMessage(const uint8_t sid)
{
    const std::lock_guard<std::mutex> locker_res(response_mutex_);
    const auto it(response_map_.find(sid));
    return std::get<1>(it->second);
}

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
