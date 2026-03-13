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

#ifndef SOURCE_DCM_ROUTE_PARTITION_SYNC_TABLE_H_
#define SOURCE_DCM_ROUTE_PARTITION_SYNC_TABLE_H_

#include <cstdint>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>

#include "uds_transport/uds_message_ext.h"

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
 * \brief Class for tester table.
 */
class PartitionSyncTable
{
public:
    using RequestInfo = std::tuple<uint8_t, uds_transport::UdsMessageExt>;
    using ResponseInfo = std::tuple<uint8_t, uds_transport::ByteVector>;
    using RequestMap = std::map<uint8_t, RequestInfo>;
    using ResponseMap = std::map<uint8_t, ResponseInfo>;

public:
    explicit PartitionSyncTable();
    virtual ~PartitionSyncTable() = default;

    void AddRequestMessage(const uint8_t sid, const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void AddResponseMessage(const uint8_t sid, const uint8_t conversation_id, const uds_transport::ByteVector& payload);

    void EraseRequestMessage(const uint8_t sid);

    void EraseResponseMessage(const uint8_t sid);

    bool HasRequestMessage(const uint8_t sid);

    bool HasResponseMessage(const uint8_t sid);

    uint8_t GetRequestConversationId(const uint8_t sid);

    uint8_t GetResponseConversationId(const uint8_t sid);

    uds_transport::UdsMessageExt GetRequestMessage(const uint8_t sid);

    uds_transport::ByteVector GetResponseMessage(const uint8_t sid);

private:
    explicit PartitionSyncTable(PartitionSyncTable&&) = delete;
    PartitionSyncTable(PartitionSyncTable&) = delete;
    PartitionSyncTable& operator=(PartitionSyncTable&) = delete;
    PartitionSyncTable& operator=(PartitionSyncTable&&) = delete;

private:
    RequestMap request_map_;
    ResponseMap response_map_;
    std::mutex request_mutex_;
    std::mutex response_mutex_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_ROUTE_PARTITION_SYNC_TABLE_H_
