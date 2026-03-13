#include "uds_processor_serial.h"

#include "uds_common.h"

#include "log.h"

namespace midware
{
namespace diag
{

enum UdsResponseType : uint8_t
{
    kUnknown = 0,
    kPending,
    kPositive,
    kNegative
};

UdsProcessorSerial::UdsProcessorSerial(UdsClientChannel& channel, const uint16_t diagnostic_address)
    : UdsProcessorBase(channel, diagnostic_address)
    , p2_(500)
    , p2_star_(3000)
    , max_num_of_pending_(0)
    , receving_address_(0U)
    , receving_sid_(0U)
    , mutex_()
    , mutex_cv_()
    , condition_variable_()
    , response_queue_()
{
}

UdsProcessorSerial::~UdsProcessorSerial()
{
    try {
        {
            const std::unique_lock<std::mutex> locker_cv(mutex_cv_);
            ClearUdsResponseQueue();
        }
        condition_variable_.notify_all();
    } catch (...) {
    }
}

void UdsProcessorSerial::SetTimeoutPeriod(const int p2, const int p2_star, const int max_num_of_pending)
{
    p2_.store(p2, std::memory_order_relaxed);
    p2_star_.store(p2_star, std::memory_order_relaxed);
    max_num_of_pending_.store(max_num_of_pending, std::memory_order_relaxed);
}

UdsResult UdsProcessorSerial::UdsRequest(const uint16_t address, const std::vector<uint8_t>& request)
{
    LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(address);
    UdsResult result {UdsResult::kRequestFailed};
    if (SendData(address, request))
    {
        result = UdsResult::kRequestSuccess;
    }
    return result;
}

UdsResult UdsProcessorSerial::UdsRequest(const uint16_t address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response)
{
    LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(address);
    const std::lock_guard<std::mutex> locker(mutex_);
    return UdsSendAndRecv(address, address, request, response);
}

UdsResult UdsProcessorSerial::UdsSendAndRecv(const uint16_t target_address, const uint16_t target_ecu_address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response)
{
    LOG_INFO << __FUNCTION__ << "address: " << LOG_HEX(target_address);
    std::unique_lock<std::mutex> locker_cv(mutex_cv_);
    UdsResult result {UdsResult::kRequestFailed};
    ClearUdsResponseQueue();
    receving_address_.store(target_ecu_address, std::memory_order_relaxed);
    receving_sid_.store(request.at(0), std::memory_order_relaxed);
    const auto p2 = p2_.load();
    const auto p2_star = p2_star_.load();
    const auto max_num_of_pending = max_num_of_pending_.load();
    auto expiry_point =  std::chrono::steady_clock::now() + std::chrono::milliseconds(p2);
    if (SendData(target_address, request))
    {
        result = UdsResult::kRequestSuccess;
        uint16_t num_of_pending {0};
        bool finish {false};
        while (!finish)
        {
            const std::cv_status wait_status = condition_variable_.wait_until(locker_cv, expiry_point);
            if (wait_status == std::cv_status::timeout)
            {
                if (!SuppressPositiveResponse(request))
                {
                    LOG_ERROR << __FUNCTION__ << "response timout! ";
                    result = UdsResult::kResponseTimeout;
                }
                finish = true;
            }
            else
            {
                uint8_t type{UdsResponseType::kUnknown};
                std::vector<uint8_t> uds;
                CheckUdsResponse(type, uds);
                if (uds.empty())
                {
                    LOG_WARN << __FUNCTION__ << "response is empty! ";
                    continue;
                }
                switch (type)
                {
                case UdsResponseType::kPending:
                {
                    expiry_point = std::chrono::steady_clock::now() + std::chrono::milliseconds(p2_star);
                    num_of_pending++;
                    if ((max_num_of_pending == 0) || (num_of_pending <= max_num_of_pending))
                    {
                        if ((num_of_pending != 0) && (num_of_pending % 10 == 0))
                        {
                            LOG_ERROR << __FUNCTION__ << "pending! num: " << num_of_pending;
                        }
                    }
                    else
                    {
                        LOG_ERROR << __FUNCTION__ << "pending override max num! num: " << num_of_pending;
                        result = UdsResult::kPendingOverrideMaxNum;
                        finish = true;
                    }
                    break;
                }
                case UdsResponseType::kPositive:
                {
                    response = std::move(uds);
                    result = UdsResult::kPositiveResponse;
                    finish = true;
                    break;
                }
                case UdsResponseType::kNegative:
                {
                    response = std::move(uds);
                    result = UdsResult::kNegativeResponse;
                    finish = true;
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
        }
    }
    receving_address_.store(0x0000, std::memory_order_relaxed);
    receving_sid_.store(0x00, std::memory_order_relaxed);
    return result;
}

void UdsProcessorSerial::UdsResponse(const uint16_t address, const std::vector<uint8_t>& data)
{
    LOG_INFO << __FUNCTION__ << "address: " << LOG_HEX(address);
    {
        const std::unique_lock<std::mutex> locker_cv(mutex_cv_);
        HandleUdsResponse(address, data);
    }
    condition_variable_.notify_all();
}

void UdsProcessorSerial::HandleUdsResponse(const uint16_t address, const std::vector<uint8_t>& data)
{
    const auto receving_address = receving_address_.load(std::memory_order_relaxed);
    const auto receving_sid = receving_sid_.load(std::memory_order_relaxed);
    if (address != receving_address)
    {
        LOG_ERROR << __FUNCTION__ << "response sa: " << LOG_HEX(address) << "mismatched: " << LOG_HEX(receving_address);
        return;
    }
    if (data.size() < 1)
    {
        LOG_ERROR << __FUNCTION__ << "response size error! size: " << data.size();
        return;
    }
    uint8_t sid = data.at(0);
    if (sid == static_cast<uint8_t>(receving_sid | 0x40))
    {
        AddUdsResponse(data);
        LOG_DEBUG << __FUNCTION__ << "positive response! sid: " << LOG_HEX(data.at(0));
    }
    else if (sid == kUdsNackSid)
    {
        if ((data.size() != 3))
        {
            LOG_ERROR << __FUNCTION__ << "response size error! size: " << data.size();
            return;
        }
        if (data.at(1) == receving_sid)
        {
            AddUdsResponse(data);
            LOG_DEBUG << __FUNCTION__ << "negative response! nrc: " << LOG_HEX(data.at(2));
        }
        else
        {
            LOG_ERROR << __FUNCTION__ << "response sid: " << LOG_HEX(data.at(1)) << "mismatched: " << LOG_HEX(receving_sid);
        }
    }
    else
    {
        LOG_ERROR << __FUNCTION__ << "response sid: " << LOG_HEX(data.at(0)) << "mismatched: " << LOG_HEX(receving_sid);
    }
}

void UdsProcessorSerial::CheckUdsResponse(uint8_t& type, std::vector<uint8_t>& data)
{
    while (!UdsResponseQueueIsEmpty())
    {
        GetUdsResponse(data);
        if ((data.size() == 3) && (data.at(0) == kUdsNackSid))
        {
            if (data.at(2) == kUdsNackPending)
            {
                type = UdsResponseType::kPending;
            }
            else
            {
                type = UdsResponseType::kNegative;
                break;
            }
        }
        else
        {
            type = UdsResponseType::kPositive;
            break;
        }
    }
}

bool UdsProcessorSerial::SuppressPositiveResponse(const std::vector<uint8_t>& data)
{
    bool suppress{false};
    if (!(data.size() > 0))
    {
        LOG_DEBUG << "data size error! size: " << data.size();
        return suppress;
    }
    for (auto &i : kServiceWithSubfunctionSet)
    {
        if (i == data.at(0U))
        {
            if ((data.size() > 1U) && ((data.at(1U) & 0x80U) == 0x80U))
            {
                LOG_DEBUG << __FUNCTION__ << "suppress pos rsp msg! sid: " << LOG_HEX(data.at(0U)) << "sub function: " << LOG_HEX(data.at(1U));
                suppress = true;
                break;
            }
        }
    }
    return suppress;
}

void UdsProcessorSerial::AddUdsResponse(std::vector<uint8_t> data)
{
    response_queue_.push(std::move(data));
}

void UdsProcessorSerial::GetUdsResponse(std::vector<uint8_t>& data)
{
    data = std::move(response_queue_.front());
    response_queue_.pop();
}

void UdsProcessorSerial::ClearUdsResponseQueue()
{
    std::queue<std::vector<uint8_t> > tmp;
    response_queue_.swap(tmp);
}

bool UdsProcessorSerial::UdsResponseQueueIsEmpty()
{
    return response_queue_.empty();
}

} // diag
} // midware
