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

#ifndef SOURCE_DCM_ROUTE_PARTITION_SYNC_H_
#define SOURCE_DCM_ROUTE_PARTITION_SYNC_H_

#include <functional>

#include "dcm/route/partition/partition_sync_table.h"

#include "uds_transport/uds_message_ext.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class PartitionSync
{
public:
    using PartitionSyncInfo = std::tuple<uint8_t, uint8_t, uint16_t, uint16_t>;
    using PartitionSyncRequestCallback = std::function<void(const uint8_t , const uint8_t , const uds_transport::UdsMessageExt& )>;
    using PartitionSyncResponseCallback = std::function<void(const uint8_t , const uds_transport::ByteVector& )>;

public:
    static PartitionSync& GetInstance()
    {
        static PartitionSync instance;
        return instance;
    }

    void Initialize();
    void Deinitialize();

    bool IsPartitionSyncServer(const uint8_t sid);

    bool IsTesterPresentServer(const uint8_t sid);

    bool IsPartitionSyncServerPositiveResponse(const uint8_t sid, const uds_transport::ByteVector& payload);

    void AddPartitionSyncMessage(const uint8_t sid, const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void ErasePartitionSyncMessage(const uint8_t sid);

    void StartPartitionSync(const uint8_t sid, const uds_transport::ByteVector& payload);

    void VerifyPartitionSync(const uint8_t sid, const uds_transport::ByteVector& payload);

    void SendTesterPresentMessage(const uint8_t conversation_id, const uint8_t processor_id, const uint16_t sa, const uint16_t ta);

    inline void RegisterPartitionSyncRequestCallback(const PartitionSyncRequestCallback callback)
    {
        partition_sync_request_ = callback;
    }

    inline void RegisterPartitionSyncResponseCallback(const PartitionSyncResponseCallback callback)
    {
        partition_sync_response_ = callback;
    }

protected:
    void PartitionSyncQueue();

    void HandlePartitionSyncInfo(const PartitionSyncInfo& info);

    void SendPartitionSyncRequest(const uint8_t conversation_id, const uint8_t processor_id, const uds_transport::UdsMessageExt& message);

    void SendPartitionSyncResponse(const uint8_t conversation_id, const uds_transport::ByteVector& payload);

    void AddPartitionSyncInfo(PartitionSyncInfo info);

    void GetPartitionSyncInfo(PartitionSyncInfo& info);

    void ClearPartitionSyncQueue();

    bool PartitionSyncQueueIsEmpty();

private:
    explicit PartitionSync();
    virtual ~PartitionSync() = default;
    explicit PartitionSync(PartitionSync&&) = delete;
    PartitionSync(PartitionSync&) = delete;
    PartitionSync& operator=(PartitionSync&) = delete;
    PartitionSync& operator=(PartitionSync&&) = delete;

private:
    PartitionSyncTable partition_sync_table_;

    PartitionSyncRequestCallback partition_sync_request_;
    PartitionSyncResponseCallback partition_sync_response_;

    std::atomic_bool exit_requested_;
    std::condition_variable condition_variable_;
    std::mutex mutex_;
    std::queue<PartitionSyncInfo> sync_queue_;
    std::thread thread_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif // SOURCE_DCM_ROUTE_PARTITION_SYNC_H_
