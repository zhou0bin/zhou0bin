#include "uds_test_present.h"

#include "log.h"

namespace midware
{
namespace diag
{

UdsTestPresent::UdsTestPresent()
    : address_(0U)
    , is_response_suppressable_(true)
    , period_(2000)
    , start_test_present_(false)
    , exit_requested_(false)
    , thread_()
{
    thread_ = std::thread([this]() { TestPresent(); });
}

UdsTestPresent::~UdsTestPresent()
{
    exit_requested_.store(true);
    if (start_test_present_.load())
    {
        start_test_present_.store(false);
    }
    condition_variable_.notify_all();
    try {
        if (thread_.joinable())
        {
            thread_.join(); 
        }
    } catch(...) {
    }
}

bool UdsTestPresent::Start(const uint16_t address, const bool is_response_suppressable, const int period)
{
    LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(address) << "is_response_suppressable: " << is_response_suppressable << "period: " << period;
    {
        std::unique_lock<std::mutex> locker(mutex_);
        if (start_test_present_.load() == true)
        {
            LOG_DEBUG << __FUNCTION__ << "already start! ";
            return false;
        }
        else
        {
            if (!SendTestPresent(address, is_response_suppressable))
            {
                LOG_ERROR << __FUNCTION__ << "failed! ";
                return false;
            }
            LOG_INFO << __FUNCTION__ << "success! ";
            address_.store(address);
            is_response_suppressable_.store(is_response_suppressable);
            period_.store(period);
            start_test_present_.store(true);
        }
    }
    condition_variable_.notify_all();
    return true;
}

bool UdsTestPresent::Stop(const uint16_t address)
{
    LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(address);
    {
        std::unique_lock<std::mutex> locker(mutex_);
        if (start_test_present_.load() == false)
        {
            LOG_ERROR << __FUNCTION__ << "not start ";
            return false;
        }
        else
        {
            if ((address_.load() != address))
            {
                LOG_ERROR << __FUNCTION__ << "address: " << LOG_HEX(address) << "mismatched: " << LOG_HEX(address_.load());
                return false;
            }
        }
        LOG_INFO << __FUNCTION__ << "success! ";
        start_test_present_.store(false);
    }
    condition_variable_.notify_all();
    return true;
}

void UdsTestPresent::TestPresent()
{
    std::unique_lock<std::mutex> locker(mutex_);
    while (!exit_requested_.load())
    {
        if (!start_test_present_.load())
        {
            condition_variable_.wait(locker);
            continue;
        }
        if (start_test_present_.load())
        {
            auto expiry_point = period_.load();
            const std::cv_status wait_status = condition_variable_.wait_for(locker, std::chrono::milliseconds(expiry_point));
            if (wait_status == std::cv_status::timeout)
            {
                (void)SendTestPresent(address_.load(), is_response_suppressable_.load());
            }
        }
    }
}

bool UdsTestPresent::SendTestPresent(const uint16_t address, const bool is_response_suppressable)
{
    std::vector<uint8_t> request{0x3e};
    if (is_response_suppressable)
    {
        request.push_back(0x80);
    }
    else
    {
        request.push_back(0x00);
    }
    std::vector<uint8_t> response;
    if (SendTestPresent(address, request, response))
    {
        if (!response.empty())
        {
            LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(address);
        }
        return true;
    }
    return false;
}

bool UdsTestPresent::SendTestPresent(const uint16_t address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response)
{
    if (callback_ != nullptr)
    {
        if (callback_(address, request, response) != static_cast<int>(UdsResult::kRequestFailed))
        {
            return true;
        }
    }
    return false;
}

}
}
