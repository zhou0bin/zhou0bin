#include "uds_processor_parallel.h"

namespace midware
{
namespace diag
{

UdsProcessorParallel::UdsProcessorParallel(UdsClientChannel& channel, const uint16_t diagnostic_address)
    : UdsProcessorBase(channel, diagnostic_address)
    , exit_requested_(false)
    , mutex_()
    , mutex_cv_()
    , condition_variable_()
    , uds_msg_queue_()
    , callback_(nullptr)
{
    thread_ = std::thread([this]() { ReceiveMessages(); });
}

UdsProcessorParallel::~UdsProcessorParallel()
{
    exit_requested_.store(true, std::memory_order_release);
    try {
        {
            const std::unique_lock<std::mutex> locker_cv_(mutex_cv_);
            ClearUdsMsgQueue();
        }
        condition_variable_.notify_all();
        if (thread_.joinable())
        {
            thread_.join();
        }
    } catch (...) {
    }
}

bool UdsProcessorParallel::UdsRequest(const uint16_t address, const std::vector<uint8_t>& data)
{
    LOG_INFO << __FUNCTION__ << "address: " << LOG_HEX(address);
    const std::lock_guard<std::mutex> locker(mutex_);
    return SendData(address, data);
}

void UdsProcessorParallel::UdsResponse(const uint16_t address, const std::vector<uint8_t>& data)
{
    LOG_INFO << __FUNCTION__ << "address: " << LOG_HEX(address);
    {
        const std::unique_lock<std::mutex> locker_cv_(mutex_cv_);
        UdsMsg msg{address, data};
        AddUdsMsg(msg);
    }
    condition_variable_.notify_all();
}

void UdsProcessorParallel::ReceiveMessages()
{
    std::unique_lock<std::mutex> locker_cv_(mutex_cv_);
    while (!exit_requested_.load(std::memory_order_acquire))
    {
        if (UdsMsgQueueIsEmpty())
        {
            condition_variable_.wait(locker_cv_);
            continue;
        }
        if (!UdsMsgQueueIsEmpty())
        {
            UdsMsg msg;
            GetUdsMsg(msg);
            const auto address = std::get<0>(msg);
            const auto data = std::get<1>(msg);
            locker_cv_.unlock();
            {
                HandleMessage(address, data);
            }
            locker_cv_.lock();
        }
    }
}

void UdsProcessorParallel::HandleMessage(const uint16_t address, const std::vector<uint8_t>& data)
{
    if (callback_ != nullptr)
    {
        callback_(address, data);
    }
}

void UdsProcessorParallel::AddUdsMsg(UdsMsg msg)
{
    uds_msg_queue_.push(std::move(msg));
}

void UdsProcessorParallel::GetUdsMsg(UdsMsg& msg)
{
    msg = std::move(uds_msg_queue_.front());
    uds_msg_queue_.pop();
}

void UdsProcessorParallel::ClearUdsMsgQueue()
{
    std::queue<UdsMsg> empty;
    uds_msg_queue_.swap(empty);
}

bool UdsProcessorParallel::UdsMsgQueueIsEmpty()
{
    auto ret = uds_msg_queue_.empty();
    return ret;
}

} // diag
} // midware
