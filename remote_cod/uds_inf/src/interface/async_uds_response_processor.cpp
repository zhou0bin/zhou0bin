#include "async_uds_response_processor.h"

#include "uds_common.h"

#include "log.h"

namespace midware
{
namespace diag
{

AsyncResponseProcessor::AsyncResponseProcessor(const uint16_t diagnostic_address)
    : diagnostic_address_(diagnostic_address)
    , uds_0x10_response_callback_map_()
    , uds_0x27_response_callback_map_()
    , uds_0x11_response_callback_map_()
    , uds_0x28_response_callback_map_()
    , uds_0x3E_response_callback_map_()
    , uds_0x22_response_callback_map_()
    , uds_0x2E_response_callback_map_()
    , uds_0x31_response_callback_map_()
    , uds_0x34_response_callback_map_()
    , uds_0x35_response_callback_map_()
    , uds_0x36_response_callback_map_()
    , uds_0x37_response_callback_map_()
    , uds_0x38_response_callback_map_()
    , uds_0x2900_response_callback_map_()
    , uds_0x2901_response_callback_map_()
    , uds_0x2902_response_callback_map_()
    , uds_0x2903_response_callback_map_()
    , uds_0x2904_response_callback_map_()
    , uds_0x2908_response_callback_map_()
    , uds_0x14_response_callback_map_()
    , uds_0x1901_response_callback_map_()
    , uds_0x1902_response_callback_map_()
    , uds_0x1903_response_callback_map_()
    , uds_0x1904_response_callback_map_()
    , uds_0x1906_response_callback_map_()
    , uds_0x190A_response_callback_map_()
    , uds_0x85_response_callback_map_()
    , negative_response_callback_map_()
    , mutex_()
{
}

void AsyncResponseProcessor::RegisterUds0x10Callback(const Uds0x10ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x10_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x27Callback(const Uds0x27ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x27_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x22Callback(const Uds0x22ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x22_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x2ECallback(const Uds0x2EResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x2E_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x31Callback(const Uds0x31ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x31_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x11Callback(const Uds0x11ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x11_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x28Callback(const Uds0x28ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x28_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x3ECallback(const Uds0x3EResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x3E_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x34Callback(const Uds0x34ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x34_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x35Callback(const Uds0x35ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x35_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x36Callback(const Uds0x36ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x36_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x37Callback(const Uds0x37ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x37_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x38Callback(const Uds0x38ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x38_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x2900Callback(const Uds0x2900ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x2900_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x2901Callback(const Uds0x2901ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x2901_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x2902Callback(const Uds0x2902ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x2902_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x2903Callback(const Uds0x2903ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x2903_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x2904Callback(const Uds0x2904ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x2904_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x2908Callback(const Uds0x2908ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x2908_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x14Callback(const Uds0x14ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x14_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x1901Callback(const Uds0x1901ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x1901_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x1902Callback(const Uds0x1902ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x1902_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x1903Callback(const Uds0x1903ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x1903_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x1904Callback(const Uds0x1904ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x1904_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x1906Callback(const Uds0x1906ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x1906_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x190ACallback(const Uds0x190AResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x190A_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterUds0x85Callback(const Uds0x85ResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    uds_0x85_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::RegisterNegativeCallback(const NegativeResponseCallback fun, const uint16_t address)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << ((fun == nullptr) ? "register nullptr! " : "register! ");
    const std::lock_guard<std::mutex> locker(mutex_);
    negative_response_callback_map_[address] = fun;
}

void AsyncResponseProcessor::HandleUds0x10Response(const uint16_t address, struct Uds0x10ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x10_response_callback_map_.find(address) != uds_0x10_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x10_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x10_response_callback_map_.find(kDefaultAddress) != uds_0x10_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x10_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x27Response(const uint16_t address, struct Uds0x27ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x27_response_callback_map_.find(address) != uds_0x27_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x27_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x27_response_callback_map_.find(kDefaultAddress) != uds_0x27_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x27_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x11Response(const uint16_t address, struct Uds0x11ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x11_response_callback_map_.find(address) != uds_0x11_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x11_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x11_response_callback_map_.find(kDefaultAddress) != uds_0x11_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x11_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x28Response(const uint16_t address, struct Uds0x28ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x28_response_callback_map_.find(address) != uds_0x28_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x28_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x28_response_callback_map_.find(kDefaultAddress) != uds_0x28_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x28_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x3EResponse(const uint16_t address, struct Uds0x3EResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x3E_response_callback_map_.find(address) != uds_0x3E_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x3E_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x3E_response_callback_map_.find(kDefaultAddress) != uds_0x3E_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x3E_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x22Response(const uint16_t address, struct Uds0x22ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x22_response_callback_map_.find(address) != uds_0x22_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x22_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x22_response_callback_map_.find(kDefaultAddress) != uds_0x22_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x22_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x2EResponse(const uint16_t address, struct Uds0x2EResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x2E_response_callback_map_.find(address) != uds_0x2E_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2E_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x2E_response_callback_map_.find(kDefaultAddress) != uds_0x2E_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2E_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x31Response(const uint16_t address, struct Uds0x31ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x31_response_callback_map_.find(address) != uds_0x31_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x31_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x31_response_callback_map_.find(kDefaultAddress) != uds_0x31_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x31_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x34Response(const uint16_t address, struct Uds0x34ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x34_response_callback_map_.find(address) != uds_0x34_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x34_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x34_response_callback_map_.find(kDefaultAddress) != uds_0x34_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x34_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x35Response(const uint16_t address, struct Uds0x35ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x35_response_callback_map_.find(address) != uds_0x35_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x35_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x35_response_callback_map_.find(kDefaultAddress) != uds_0x35_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x35_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x36Response(const uint16_t address, struct Uds0x36ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x36_response_callback_map_.find(address) != uds_0x36_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x36_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x36_response_callback_map_.find(kDefaultAddress) != uds_0x36_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x36_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x37Response(const uint16_t address, struct Uds0x37ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x37_response_callback_map_.find(address) != uds_0x37_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x37_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x37_response_callback_map_.find(kDefaultAddress) != uds_0x37_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x37_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x38Response(const uint16_t address, struct Uds0x38ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x38_response_callback_map_.find(address) != uds_0x38_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x38_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x38_response_callback_map_.find(kDefaultAddress) != uds_0x38_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x38_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x2900Response(const uint16_t address, struct Uds0x29ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x2900_response_callback_map_.find(address) != uds_0x2900_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2900_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x2900_response_callback_map_.find(kDefaultAddress) != uds_0x2900_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2900_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x2901Response(const uint16_t address, struct Uds0x2901ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x2901_response_callback_map_.find(address) != uds_0x2901_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2901_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x2901_response_callback_map_.find(kDefaultAddress) != uds_0x2901_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2901_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x2902Response(const uint16_t address, struct Uds0x2902ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x2902_response_callback_map_.find(address) != uds_0x2902_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2902_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x2902_response_callback_map_.find(kDefaultAddress) != uds_0x2902_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2902_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x2903Response(const uint16_t address, struct Uds0x2903ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x2903_response_callback_map_.find(address) != uds_0x2903_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2903_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x2903_response_callback_map_.find(kDefaultAddress) != uds_0x2903_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2903_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x2904Response(const uint16_t address, struct Uds0x29ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x2904_response_callback_map_.find(address) != uds_0x2904_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2904_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x2904_response_callback_map_.find(kDefaultAddress) != uds_0x2904_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2904_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x2908Response(const uint16_t address, struct Uds0x29ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x2908_response_callback_map_.find(address) != uds_0x2908_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2908_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x2908_response_callback_map_.find(kDefaultAddress) != uds_0x2908_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x2908_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x14Response(const uint16_t address, struct Uds0x14ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x14_response_callback_map_.find(address) != uds_0x14_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x14_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x14_response_callback_map_.find(kDefaultAddress) != uds_0x14_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x14_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x1901Response(const uint16_t address, struct Uds0x1901ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x1901_response_callback_map_.find(address) != uds_0x1901_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x1901_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x1901_response_callback_map_.find(kDefaultAddress) != uds_0x1901_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x1901_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x1902Response(const uint16_t address, struct Uds0x1902ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x1902_response_callback_map_.find(address) != uds_0x1902_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x1902_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x1902_response_callback_map_.find(kDefaultAddress) != uds_0x1902_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x1902_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x1903Response(const uint16_t address, struct Uds0x1903ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x1903_response_callback_map_.find(address) != uds_0x1903_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x1903_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x1903_response_callback_map_.find(kDefaultAddress) != uds_0x1903_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x1903_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x1904Response(const uint16_t address, struct Uds0x1904ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x1904_response_callback_map_.find(address) != uds_0x1904_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x1904_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x1904_response_callback_map_.find(kDefaultAddress) != uds_0x1904_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x1904_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x1906Response(const uint16_t address, struct Uds0x1906ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x1906_response_callback_map_.find(address) != uds_0x1906_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x1906_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x1906_response_callback_map_.find(kDefaultAddress) != uds_0x1906_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x1906_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x190AResponse(const uint16_t address, struct Uds0x190AResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x190A_response_callback_map_.find(address) != uds_0x190A_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x190A_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x190A_response_callback_map_.find(kDefaultAddress) != uds_0x190A_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x190A_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleUds0x85Response(const uint16_t address, struct Uds0x85ResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (uds_0x85_response_callback_map_.find(address) != uds_0x85_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x85_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (uds_0x85_response_callback_map_.find(kDefaultAddress) != uds_0x85_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            uds_0x85_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

void AsyncResponseProcessor::HandleNegativeResponse(const uint16_t address, struct NegativeResponseStruct& response)
{
    try
    {
        const std::lock_guard<std::mutex> locker(mutex_);
        if (negative_response_callback_map_.find(address) != negative_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            negative_response_callback_map_[address](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
        if (negative_response_callback_map_.find(kDefaultAddress) != negative_response_callback_map_.end())
        {
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback in! ";
            negative_response_callback_map_[kDefaultAddress](address, response);
            LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "callback out! ";
            return;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(address) << "try callback error! " << e.what();
    }
}

}
}
