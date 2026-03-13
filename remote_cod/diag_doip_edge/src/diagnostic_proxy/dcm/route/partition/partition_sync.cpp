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

#include "dcm/route/partition/partition_sync.h"

#include "dcm/route/partition/partition_route.h"

#include "uds_transport/uds_message_ext.h"
using asf::diag::uds_transport::UdsMessageExt;
using asf::diag::uds_transport::UdsMessage;
using asf::diag::uds_transport::ByteVector;

#include "dcm/connection/uds_services.h"
using asf::diag::dcm::connection::UdsServices;

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

PartitionSync::PartitionSync()
    : partition_sync_table_()
    , partition_sync_request_(nullptr)
    , partition_sync_response_(nullptr)
    , exit_requested_(false)
{
}

void PartitionSync::Initialize()
{
    thread_ = std::thread([this]() { PartitionSyncQueue(); });
}

void PartitionSync::Deinitialize()
{
    exit_requested_.store(true, std::memory_order_release);
    {
        const std::unique_lock<std::mutex> locker(mutex_);
        ClearPartitionSyncQueue();
    }
    condition_variable_.notify_all();
    if (thread_.joinable())
    {
        thread_.join();
    }
}

bool PartitionSync::IsPartitionSyncServer(const uint8_t sid)
{
    if ((sid == UdsServices::kDiagnosticSessionControl) || (sid == UdsServices::kSecurityAccess))
    {
        return true;
    }
    return false;
}

bool PartitionSync::IsTesterPresentServer(const uint8_t sid)
{
    if (sid == UdsServices::kTesterPresent)
    {
        return true;
    }
    return false;
}

bool PartitionSync::IsPartitionSyncServerPositiveResponse(const uint8_t sid, const ByteVector& payload)
{
    if ((sid == UdsServices::kDiagnosticSessionControl) || (sid == UdsServices::kSecurityAccess))
    {
        if ((payload.size() > 0) && (payload.at(0) == (sid + 0x40)))
        {
            return true;
        }
    }
    return false;
}

void PartitionSync::AddPartitionSyncMessage(const uint8_t sid, const uint8_t conversation_id, const uds_transport::UdsMessageExt& message)
{
    LOG_DEBUG << __FUNCTION__ << "Sid: " << LOG_HEX(sid);
    partition_sync_table_.AddRequestMessage(sid, conversation_id, message);
}

void PartitionSync::ErasePartitionSyncMessage(const uint8_t sid)
{
    LOG_DEBUG << __FUNCTION__ << "Sid: " << LOG_HEX(sid);
    partition_sync_table_.EraseRequestMessage(sid);
}

void PartitionSync::StartPartitionSync(const uint8_t sid, const ByteVector& payload)
{
    LOG_DEBUG << __FUNCTION__ << "Sid: " << LOG_HEX(sid);
    if (partition_sync_table_.HasRequestMessage(sid))
    {
        const auto conversation_id = partition_sync_table_.GetRequestConversationId(sid);
        {
            partition_sync_table_.AddResponseMessage(sid, conversation_id, payload);
        }
        const auto request = partition_sync_table_.GetRequestMessage(sid);
        partition_sync_table_.EraseRequestMessage(sid);
        LOG_ERROR << __FUNCTION__ << "Sid: " << LOG_HEX(sid) << "SubFun: " << LOG_HEX(request.GetSubFunction().value_or(0));
        SendPartitionSyncRequest(conversation_id, PartitionEnum::kPartitionDcm, request);
    }
}

void PartitionSync::VerifyPartitionSync(const uint8_t sid, const ByteVector& payload)
{
    LOG_DEBUG << __FUNCTION__ << "Sid: " << LOG_HEX(sid);
    if (partition_sync_table_.HasResponseMessage(sid))
    {
        const auto conversation_id = partition_sync_table_.GetResponseConversationId(sid);
        if ((payload.size() == 3) && (payload.at(0) == 0x7f))
        {
            if (payload.at(2) == 0x78)
            {
                LOG_ERROR << __FUNCTION__ << "Sid: " << LOG_HEX(payload.at(1)) << "{IsPending}";
                return;
            }
            else
            {
                LOG_ERROR << __FUNCTION__ << "Sid: " << LOG_HEX(payload.at(1)) << "Nrc: " << LOG_HEX(payload.at(2)) << "{IsNegativeResponse}. ";
                partition_sync_table_.AddResponseMessage(sid, conversation_id, payload);
            }
        }
        else
        {
            LOG_ERROR << __FUNCTION__ << "Sid: " << LOG_HEX(payload.at(0)) << "Sub Function: " << LOG_HEX(payload.at(1)) << "{IsPositiveResponse}. ";
        }
        const auto response = partition_sync_table_.GetResponseMessage(sid);
        partition_sync_table_.EraseResponseMessage(sid);
        SendPartitionSyncResponse(conversation_id, response);
    }
}

void PartitionSync::SendTesterPresentMessage(const uint8_t conversation_id, const uint8_t processor_id, const uint16_t sa, const uint16_t ta)
{
    LOG_DEBUG << __FUNCTION__ << "conversation id: " << LOG_HEX(conversation_id) << "processor id: " << LOG_HEX(processor_id) << "sa: " << LOG_HEX(sa) << "ta: " << LOG_HEX(ta);
    {
        const std::unique_lock<std::mutex> locker(mutex_);
        PartitionSyncInfo info{conversation_id, processor_id, sa, ta};
        AddPartitionSyncInfo(info);
    }
    condition_variable_.notify_all();
}

void PartitionSync::PartitionSyncQueue()
{
    std::unique_lock<std::mutex> locker(mutex_);
    while (!exit_requested_.load(std::memory_order_acquire))
    {
        if (PartitionSyncQueueIsEmpty())
        {
            condition_variable_.wait(locker);
            continue;
        }
        if (!PartitionSyncQueueIsEmpty())
        {
            PartitionSyncInfo info;
            GetPartitionSyncInfo(info);
            locker.unlock();
            {
                HandlePartitionSyncInfo(info);
            }
            locker.lock();
        }
    }
}

void PartitionSync::HandlePartitionSyncInfo(const PartitionSyncInfo& info)
{
    const auto conversation_id = std::get<0>(info);
    const auto processor_id = std::get<1>(info);
    const auto source_address = std::get<2>(info);
    const auto target_address = std::get<3>(info);
    UdsMessageExt message = {source_address, target_address, {0x3e, 0x80}};
    message.SetTaType(UdsMessage::TargetAddressType::kFunctional);
    SendPartitionSyncRequest(conversation_id, processor_id, message);
}

void PartitionSync::SendPartitionSyncRequest(const uint8_t conversation_id, const uint8_t processor_id, const uds_transport::UdsMessageExt& message)
{
    if (partition_sync_request_ != nullptr)
    {
        partition_sync_request_(conversation_id, processor_id, message);
    }
}

void PartitionSync::SendPartitionSyncResponse(const uint8_t conversation_id, const uds_transport::ByteVector& payload)
{
    if (partition_sync_response_ != nullptr)
    {
        partition_sync_response_(conversation_id, payload);
    }
}

void PartitionSync::AddPartitionSyncInfo(PartitionSyncInfo info)
{
    sync_queue_.push(std::move(info));
}

void PartitionSync::GetPartitionSyncInfo(PartitionSyncInfo& info)
{
    info = std::move(sync_queue_.front());
    sync_queue_.pop();
}

void PartitionSync::ClearPartitionSyncQueue()
{
    std::queue<PartitionSyncInfo> empty;
    sync_queue_.swap(empty);
}

bool PartitionSync::PartitionSyncQueueIsEmpty()
{
    auto ret = sync_queue_.empty();
    return ret;
}

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
