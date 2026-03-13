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

#include "dcm/arbitration/arbitration.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace arbitration
{

Arbitration::Arbitration(const std::string& config)
    : config_(config)
    , arbitration_enable_(false)
    , arbitration_timeout_(5000)
    , p2_(50)
    , p2_star_(2000)
    , arbitrate_map_()
    , arbitrate_cache_map_()
    , arbitrate_timer_map_()
    , arbitrate_subscribe_map_()
    , arbitrate_mutex_()
    , arbitrate_cache_mutex_()
    , arbitrate_timer_mutex_()
    , mutex_()
    , exit_requested_(false)
    , condition_variable_()
    , arbitrate_address_queue_()
{
    thread_ = std::thread([this]() { HandleArbitrateAddressQueue(); });
}

void Arbitration::Initialize()
{
    std::ifstream ifs(config_);
    if (ifs.good())
    {
        rapidjson::IStreamWrapper ist(ifs);
        rapidjson::Document doc;
        (void)doc.ParseStream(ist);

        const rapidjson::Value& diag_arbitration = doc["DiagnosticArbitration"];
        if ((diag_arbitration.HasMember("ArbitrationEnable") && diag_arbitration["ArbitrationEnable"].IsBool())
         && (diag_arbitration.HasMember("ArbitrationTime") && diag_arbitration["ArbitrationTime"].IsInt()))
        {
            arbitration_enable_ = diag_arbitration["ArbitrationEnable"].GetBool();
            arbitration_timeout_ = diag_arbitration["ArbitrationTime"].GetInt();
        }

        const rapidjson::Value& timings = doc["P2Timings"];
        if ((timings.HasMember("p2") && timings["p2"].IsInt())
         && (timings.HasMember("p2_star") && timings["p2_star"].IsInt()))
        {
            p2_ = timings["p2"].GetInt();
            p2_star_ = timings["p2_star"].GetInt();
        }
    }
}

void Arbitration::Deinitialize()
{
    exit_requested_.store(true, std::memory_order_release);
    {
        const std::unique_lock<std::mutex> locker(mutex_);
        ClearArbitrateAddressQueue();
    }
    condition_variable_.notify_all();
    if (thread_.joinable())
    {
        thread_.join();
    }
}

uint8_t Arbitration::Arbitrate(const uint16_t arbitrate_address, const uint16_t address, const uint8_t priority)
{
    LOG_INFO << __FUNCTION__ << "Physically! ";
    const uint8_t result = GetArbitrateResult(arbitrate_address, address, priority);
    LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(arbitrate_address) << "ArbitrateResult: " << result << "(1: Pass, 2: Reject, 3: Pending)";
    if (result == ArbitrateResult::kArbitratePass)
    {
        ArbitrateAddressWorking(arbitrate_address);
    }
    return result;
}

uint8_t Arbitration::Arbitrate(const uint16_t arbitrate_address)
{
    LOG_INFO << __FUNCTION__ << "Functional! ";
    const uint8_t result = GetArbitrateResult();
    LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(arbitrate_address) << "ArbitrateResult: " << result << "(1: Pass, 2: Reject, 3: Pending)";
    if (result == ArbitrateResult::kArbitrateReject)
    {
        LOG_ERROR << __FUNCTION__ << "Arbitrate reject! In diagnostic, reject functionally addressed diagnostic! ";
    }
    return result;
}

uint8_t Arbitration::GetArbitrateResult(const uint16_t arbitrate_address, const uint16_t address, const uint8_t priority)
{
    const std::lock_guard<std::mutex> locker_a(arbitrate_mutex_);
    if (arbitrate_map_.find(arbitrate_address) != arbitrate_map_.end())
    {
        if (arbitrate_map_[arbitrate_address] == ArbitrateState::kArbitrateWorking)
        {
            if (arbitrate_info_map_.find(arbitrate_address) != arbitrate_info_map_.end())
            {
                const auto working_address = std::get<0>(arbitrate_info_map_[arbitrate_address]);
                const auto working_priority = std::get<1>(arbitrate_info_map_[arbitrate_address]);
                if (working_priority > priority)
                {
                    arbitrate_info_map_[arbitrate_address] = std::make_tuple(address, priority);
                    return ArbitrateResult::kArbitratePending;
                }
                else
                {
                    LOG_ERROR << __FUNCTION__ << "Target address: " << LOG_HEX(arbitrate_address) << "in working, reject message. "
                        << "working tester address: " << LOG_HEX(working_address) << "priority: " << working_priority
                        << "request tester address: " << LOG_HEX(address) << "priority: " << priority;
                    return ArbitrateResult::kArbitrateReject;
                }
            }
        }
        else if (arbitrate_map_[arbitrate_address] == ArbitrateState::kArbitrateKeeping)
        {
            if (arbitrate_info_map_.find(arbitrate_address) != arbitrate_info_map_.end())
            {
                const auto keeping_address = std::get<0>(arbitrate_info_map_[arbitrate_address]);
                const auto keeping_priority = std::get<1>(arbitrate_info_map_[arbitrate_address]);
                if (keeping_priority > priority)
                {
                    arbitrate_info_map_[arbitrate_address] = std::make_tuple(address, priority);
                    return ArbitrateResult::kArbitratePass;
                }
                else if (keeping_priority == priority)
                {
                    if (keeping_address == address)
                    {
                        return ArbitrateResult::kArbitratePass;
                    }
                    else
                    {
                        LOG_ERROR << __FUNCTION__ << "Target address: " << LOG_HEX(arbitrate_address) << "in keeping, reject message. "
                            << "working tester address: " << LOG_HEX(keeping_address) << "priority: " << keeping_priority
                            << "request tester address: " << LOG_HEX(address) << "priority: " << priority;
                        return ArbitrateResult::kArbitrateReject;
                    }
                }
                else
                {
                    LOG_ERROR << __FUNCTION__ << "Target address: " << LOG_HEX(arbitrate_address) << "in keeping, reject message. "
                        << "working tester address: " << LOG_HEX(keeping_address) << "priority: " << keeping_priority
                        << "request tester address: " << LOG_HEX(address) << "priority: " << priority;
                    return ArbitrateResult::kArbitrateReject;
                }
            }
        }
        else
        {
            const auto arbitrate_info = std::make_tuple(address, priority);
            arbitrate_info_map_[arbitrate_address] = arbitrate_info;
            return ArbitrateResult::kArbitratePass;
        }
        LOG_ERROR << __FUNCTION__ << "Arbitrate info error! address: " << LOG_HEX(arbitrate_address);
        return ArbitrateResult::kArbitratePass;
    }
    return ArbitrateResult::kArbitratePass;
}

uint8_t Arbitration::GetArbitrateResult()
{
    uint8_t result{ArbitrateResult::kArbitratePass};
    {
        const std::lock_guard<std::mutex> locker_a(arbitrate_mutex_);
        std::unique_lock<std::mutex> locker(mutex_);
        for (auto &i : arbitrate_map_)
        {
            if (i.second == ArbitrateState::kArbitrateWorking)
            {
                result = ArbitrateResult::kArbitrateReject;
                break;
            }
        }
    }
    return result;
}

void Arbitration::ArbitrateInitial(const uint16_t arbitrate_address)
{
    const std::lock_guard<std::mutex> locker_a(arbitrate_mutex_);
    arbitrate_map_[arbitrate_address] = ArbitrateState::kArbitrateInitial;
}

void Arbitration::ArbitrateStart(const uint16_t arbitrate_address)
{
    const std::lock_guard<std::mutex> locker_a(arbitrate_mutex_);
    if (arbitrate_map_.find(arbitrate_address) != arbitrate_map_.end())
    {
        arbitrate_map_[arbitrate_address] = ArbitrateState::kArbitrateWorking;
    }
}

void Arbitration::ArbitrateFinish(const uint16_t arbitrate_address)
{
    const std::lock_guard<std::mutex> locker_a(arbitrate_mutex_);
    if (arbitrate_map_.find(arbitrate_address) != arbitrate_map_.end())
    {
        arbitrate_map_[arbitrate_address] = ArbitrateState::kArbitrateKeeping;
    }
}

void Arbitration::ArbitrateRelease(const uint16_t arbitrate_address)
{
    const std::lock_guard<std::mutex> locker_a(arbitrate_mutex_);
    if (arbitrate_map_.find(arbitrate_address) != arbitrate_map_.end())
    {
        arbitrate_map_[arbitrate_address] = ArbitrateState::kArbitrateInitial;
    }
}

void Arbitration::ArbitrateAddressWorking(const uint16_t arbitrate_address)
{
    LOG_ERROR << __FUNCTION__ << "address: " << LOG_HEX(arbitrate_address) << "Arbitrate Working! ";
    ArbitrateStart(arbitrate_address);
}

void Arbitration::ArbitrateAddressKeeping(const uint16_t arbitrate_address)
{
    LOG_ERROR << __FUNCTION__ << "address: " << LOG_HEX(arbitrate_address) << "Arbitrate Keeping! ";
    StartArbitrationTimer(arbitrate_address);
    ArbitrateFinish(arbitrate_address);
}

void Arbitration::ArbitrateAddressRelease(const uint16_t arbitrate_address)
{
    LOG_ERROR << __FUNCTION__ << "address: " << LOG_HEX(arbitrate_address) << "Arbitrate Initial! ";
    ArbitrateRelease(arbitrate_address);
}

void Arbitration::ArbitrateAddressPending(const uint16_t arbitrate_address)
{
    LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(arbitrate_address);
    if (SendPendingMessage(arbitrate_address))
    {
        StartP2StarTimer(arbitrate_address);
    }
}

void Arbitration::ArbitrateAddressFinish(const uint16_t arbitrate_address)
{
    LOG_DEBUG << __FUNCTION__ << "address: " << LOG_HEX(arbitrate_address);
    if (!HasCacheMessage(arbitrate_address))
    {
        ArbitrateAddressKeeping(arbitrate_address);
        return;
    }
    LOG_ERROR << __FUNCTION__ << "HasCacheMessage. address: " << LOG_HEX(arbitrate_address);
    {
        const std::unique_lock<std::mutex> locker(mutex_);
        AddArbitrateAddress(arbitrate_address);
    }
    condition_variable_.notify_all();
}

void Arbitration::HandleArbitrateAddressQueue()
{
    std::unique_lock<std::mutex> locker(mutex_);
    while (!exit_requested_.load(std::memory_order_acquire))
    {
        if (ArbitrateAddressQueueIsEmpty())
        {
            condition_variable_.wait(locker);
            continue;
        }
        if (!ArbitrateAddressQueueIsEmpty())
        {
            const uint16_t address = GetArbitrateAddress();
            locker.unlock();
            {
                if (!SendCacheMessage(address))
                {
                    ArbitrateAddressKeeping(address);
                }
            }
            locker.lock();
        }
    }
}

bool Arbitration::ArbitrateAddressQueueIsEmpty()
{
    return arbitrate_address_queue_.empty();
}

void Arbitration::ClearArbitrateAddressQueue()
{
    std::queue<uint16_t> empty;
    arbitrate_address_queue_.swap(empty);
}

void Arbitration::AddArbitrateAddress(const uint16_t address)
{
    arbitrate_address_queue_.push(address);
}

uint16_t Arbitration::GetArbitrateAddress()
{
    uint16_t address = arbitrate_address_queue_.front();
    arbitrate_address_queue_.pop();
    return address;
}

void Arbitration::Subscribe(const uint16_t arbitrate_address, const SubscribeFunc func)
{
    ArbitrateInitial(arbitrate_address);
    CreateArbitrationTimer(arbitrate_address);
    arbitrate_subscribe_map_[arbitrate_address] = func;
}

void Arbitration::HandleCacheMessage(const uint16_t arbitrate_address, const uint8_t type, const ArbitrateCacheMsg& message)
{
    const auto cid = std::get<0>(message);
    const auto msg = std::get<1>(message);
    if (arbitrate_subscribe_map_[arbitrate_address] != nullptr)
    {
        arbitrate_subscribe_map_[arbitrate_address](arbitrate_address, type, cid, msg);
    }
}

void Arbitration::CacheMessage(const uint16_t arbitrate_address, const ArbitrateCacheMsg& message)
{
    std::unique_lock<std::mutex> locker_ac(arbitrate_cache_mutex_);
    const auto it(arbitrate_cache_map_.find(arbitrate_address));
    if (it != arbitrate_cache_map_.end())
    {
        const auto msg = it->second;
        (void)arbitrate_cache_map_.erase(arbitrate_address);
        (void)arbitrate_cache_map_.insert(ArbitrateCacheMap::value_type(arbitrate_address, message));
        locker_ac.unlock();
        HandleCacheMessage(arbitrate_address, ArbitrateMessageCtrl::kArbitratRequestBusy, msg);
    }
    else
    {
        (void)arbitrate_cache_map_.insert(ArbitrateCacheMap::value_type(arbitrate_address, message));
    }
}

bool Arbitration::HasCacheMessage(const uint16_t arbitrate_address)
{
    std::unique_lock<std::mutex> locker_ac(arbitrate_cache_mutex_);
    const auto it(arbitrate_cache_map_.find(arbitrate_address));
    if (it != arbitrate_cache_map_.end())
    {
        return true;
    }
    return false;
}

bool Arbitration::SendCacheMessage(const uint16_t arbitrate_address)
{
    std::unique_lock<std::mutex> locker_ac(arbitrate_cache_mutex_);
    const auto it(arbitrate_cache_map_.find(arbitrate_address));
    if (it != arbitrate_cache_map_.end())
    {
        const auto msg = it->second;
        (void)arbitrate_cache_map_.erase(arbitrate_address);
        locker_ac.unlock();
        HandleCacheMessage(arbitrate_address, ArbitrateMessageCtrl::kArbitratRequestSend, msg);
        return true;
    }
    return false;
}

bool Arbitration::SendPendingMessage(const uint16_t arbitrate_address)
{
    std::unique_lock<std::mutex> locker_ac(arbitrate_cache_mutex_);
    const auto it(arbitrate_cache_map_.find(arbitrate_address));
    if (it != arbitrate_cache_map_.end())
    {
        const auto msg = it->second;
        locker_ac.unlock();
        HandleCacheMessage(arbitrate_address, ArbitrateMessageCtrl::kArbitratRequestPending, msg);
        return true;
    }
    return false;
}

void Arbitration::ArbitrateCacheMessage(const uint16_t arbitrate_address, const ArbitrateCacheMsg& message)
{
    LOG_ERROR << __FUNCTION__ << "address: " << LOG_HEX(arbitrate_address) << "Arbitrate Pending! ";
    CacheMessage(arbitrate_address, message);
    StartP2Timer(arbitrate_address);
}

void Arbitration::StartArbitrationTimer(const uint16_t arbitrate_address)
{
    const auto arbitration_timer = GetArbitrationTimer(arbitrate_address);
    if (arbitration_timer != nullptr)
    {
        arbitration_timer->StartArbitrationTimer();
    }
}

void Arbitration::StartP2Timer(const uint16_t arbitrate_address)
{
    const auto arbitration_timer = GetArbitrationTimer(arbitrate_address);
    if (arbitration_timer != nullptr)
    {
        arbitration_timer->StartP2Timer();
    }
}

void Arbitration::StartP2StarTimer(const uint16_t arbitrate_address)
{
    const auto arbitration_timer = GetArbitrationTimer(arbitrate_address);
    if (arbitration_timer != nullptr)
    {
        arbitration_timer->StartP2StarTimer();
    }
}

void Arbitration::CreateArbitrationTimer(const uint16_t arbitrate_address)
{
    const std::lock_guard<std::mutex> locker_at(arbitrate_timer_mutex_);
    uint16_t key{arbitrate_address};
    ArbitrateTimerMap::iterator it(arbitrate_timer_map_.find(key));
    if (it == arbitrate_timer_map_.end()) {
        (void)arbitrate_timer_map_.insert({key, std::make_shared<ArbitrationTimer>(*this, arbitrate_address, arbitration_timeout_, p2_, p2_star_)});
    }
}

ArbitrationTimer::Ptr Arbitration::GetArbitrationTimer(const uint16_t arbitrate_address)
{
    const std::lock_guard<std::mutex> locker_at(arbitrate_timer_mutex_);
    uint16_t key{arbitrate_address};
    ArbitrateTimerMap::iterator it(arbitrate_timer_map_.find(key));
    if (it != arbitrate_timer_map_.end()) {
        return it->second;
    }
    return nullptr;
}

} /* namespace arbitration */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
