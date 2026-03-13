#include "uds_c_processor.h"

#include "uds_common.h"

#include "log.h"

namespace midware
{
namespace diag
{

using asf::diag::msg_on_neusar_net::DiagMsgType;

UdsProcessorCtype::UdsProcessorCtype()
    : channel_(nullptr)
    , initialized_(false)
    , exit_requested_(false)
{
    thread_ = std::thread([this]() { ReceiveMessages(); });
}

UdsProcessorCtype::~UdsProcessorCtype()
{
    try {
        exit_requested_.store(true, std::memory_order_release);
        {
            const std::unique_lock<std::mutex> locker(uds_msg_queue_mutex_);
            ClearUdsMsgQueue();
        }
        condition_variable_.notify_all();
        if (thread_.joinable())
        {
            thread_.join();
        }
    } catch(...) {
    }
}

void UdsProcessorCtype::UdsInitialize(std::string uds_channel)
{
    if (initialized_.load(std::memory_order_acquire))
    {
        LOG_ERROR << __FUNCTION__ << "already initialized! ";
        return;
    }
    initialized_.store(true, std::memory_order_release);

    channel_ = std::make_unique<UdsClientChannel>(uds_channel);
    if (channel_ != nullptr)
    {
        channel_->SetMessageNotifier([this](const uint16_t type, const std::vector<uint8_t> &data)
                                     { ReceiveProxyMessage(type, data); });
    }
}

void UdsProcessorCtype::UdsStart(const uint16_t source_address)
{
    if (channel_ != nullptr)
    {
        channel_->SetAvaliableNotifier(source_address,
                                       [this, source_address](const bool avaliable)
                                       {
                                           if (avaliable == true)
                                           {
                                               channel_->SendOffer(source_address);
                                           }
                                       });
        channel_->Offer(source_address);
    }
}

void UdsProcessorCtype::UdsStop(const uint16_t source_address)
{
    if (channel_ != nullptr)
    {
        channel_->StopOffer(source_address);
    }
}

bool UdsProcessorCtype::UdsRequest(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& data)
{
    std::vector<uint8_t> uds{};
    uds.push_back(GetByte(source_address, 1U));
    uds.push_back(GetByte(source_address, 0U));
    uds.push_back(GetByte(target_address, 1U));
    uds.push_back(GetByte(target_address, 0U));
    (void)uds.insert(uds.cend(), data.begin(), data.end());
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(source_address) << "ta: " << LOG_HEX(target_address) << "size: " << data.size() << "payload: " << LOG_RAW_BUFFER(data);
    if (channel_ != nullptr)
    {
        if (channel_->SendData(DiagMsgType::kDiagnosticMessage, uds))
        {
            LOG_ERROR << __FUNCTION__ << "success! ";
            return true;
        }
    }
    LOG_ERROR << __FUNCTION__ << "failed! ";
    return false;
}

void UdsProcessorCtype::ReceiveProxyMessage(const uint16_t type, const std::vector<uint8_t>& data)
{
    LOG_INFO << __FUNCTION__ << "type: " << LOG_HEX(type);
    if (DiagMsgType::kDiagnosticMessage == type)
    {
        if (data.size() < 5)
        {
            LOG_ERROR << __FUNCTION__ << "data size < 5! ";
            return;
        }
        uint16_t source_address{ 0 };
        uint16_t target_address{ 0 };
        SetByte(source_address, data.at(0), 1);
        SetByte(source_address, data.at(1), 0);
        SetByte(target_address, data.at(2), 1);
        SetByte(target_address, data.at(3), 0);
        const std::vector<uint8_t> payload{data.begin() + 4, data.end()};
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(source_address) << "ta: " << LOG_HEX(target_address) << "size: " << payload.size() << "payload: " << LOG_RAW_BUFFER(payload);
        {
            const std::unique_lock<std::mutex> locker(uds_msg_queue_mutex_);
            try
            {
                UdsMsg msg{source_address, target_address, payload};
                AddUdsMsg(std::move(msg));
            }
            catch(const std::exception& e)
            {
                LOG_ERROR << __FUNCTION__ << "exception: " << e.what();
            }
        }
        condition_variable_.notify_all();
    }
    else if (DiagMsgType::kDiagnosticAck == type)
    {
        LOG_VERBOSE << __FUNCTION__ << "kDiagnosticAck! ";
    }
    else if (DiagMsgType::kDiagnosticNack == type)
    {
        LOG_VERBOSE << __FUNCTION__ << "kDiagnosticNack! ";
    }
    else
    {
        LOG_ERROR << __FUNCTION__ << "unknow type! type: " << LOG_HEX(type);
    }
}

void UdsProcessorCtype::ReceiveMessages()
{
    std::unique_lock<std::mutex> locker(uds_msg_queue_mutex_);
    while (!exit_requested_.load(std::memory_order_acquire))
    {
        if (UdsMsgQueueIsEmpty())
        {
            condition_variable_.wait(locker);
            continue;
        }
        if (!UdsMsgQueueIsEmpty())
        {
            UdsMsg msg;
            GetUdsMsg(msg);
            const auto source_address = std::get<0>(msg);
            const auto target_address = std::get<1>(msg);
            const auto payload = std::get<2>(msg);
            locker.unlock();
            {
                HandleMessage(source_address, target_address, payload);
            }
            locker.lock();
        }
    }
}

void UdsProcessorCtype::HandleMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (uds_callback_map_[target_address] != nullptr)
    {
        uds_callback_map_[target_address](source_address, target_address, payload);
    }
    LOG_INFO << __FUNCTION__ << "out! ";
}

bool UdsProcessorCtype::UdsMsgQueueIsEmpty()
{
    auto ret = uds_msg_queue_.empty();
    return ret;
}

void UdsProcessorCtype::ClearUdsMsgQueue()
{
    std::queue<UdsMsg> empty;
    uds_msg_queue_.swap(empty);
}

void UdsProcessorCtype::AddUdsMsg(const UdsMsg& msg)
{
    uds_msg_queue_.push(msg);
}

void UdsProcessorCtype::GetUdsMsg(UdsMsg& msg)
{
    msg = std::move(uds_msg_queue_.front());
    uds_msg_queue_.pop();
}

} // diag
} // midware
