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

#ifndef SOURCE_DCM_ARBITRATION_ARBITRATION_H_
#define SOURCE_DCM_ARBITRATION_ARBITRATION_H_

#include <cstdint>
#include <functional>
#include <mutex>
#include <tuple>

#include "dcm/arbitration/arbitration_timer.h"

#include "uds_transport/uds_message_ext.h"

#include "diag_proxy_common.h"

// #include "ara/core/map.h"
#include <map>

namespace asf
{
namespace diag
{
namespace dcm
{
namespace arbitration
{

class Arbitration
{
public:

    using ArbitrateMap = std::map<uint16_t, uint8_t>;

    using ArbitrateInfo = std::tuple<uint16_t, uint8_t>;
    using ArbitrateInfoMap = std::map<uint16_t, ArbitrateInfo>;

    using ArbitrateCacheMsg = std::tuple<uint8_t, uds_transport::UdsMessageExt>;
    using ArbitrateCacheMap = std::map<uint16_t, ArbitrateCacheMsg>;
    using ArbitrateTimerMap = std::map<uint16_t, ArbitrationTimer::Ptr>;

    using SubscribeFunc = std::function<void(const uint16_t, const uint8_t, const uint8_t , const uds_transport::UdsMessageExt& )>;
    using ArbitrateSubscribeMap = std::map<uint16_t, SubscribeFunc>;

public:
    explicit Arbitration(const std::string& config);
    virtual ~Arbitration() = default;

    virtual void Initialize();
    virtual void Deinitialize();

    uint8_t Arbitrate(const uint16_t arbitrate_address, const uint16_t address, const uint8_t priority);

    uint8_t Arbitrate(const uint16_t arbitrate_address);

    void ArbitrateAddressRelease(const uint16_t arbitrate_address);

    void ArbitrateAddressPending(const uint16_t arbitrate_address);

    void ArbitrateAddressFinish(const uint16_t arbitrate_address);

    void ArbitrateCacheMessage(const uint16_t arbitrate_address, const ArbitrateCacheMsg& message);

    void Subscribe(const uint16_t arbitrate_address, const SubscribeFunc func);

    inline bool ArbitrationEnable() const
    {
        return arbitration_enable_;
    }

protected:
    uint8_t GetArbitrateResult(const uint16_t arbitrate_address, const uint16_t address, const uint8_t priority);
    uint8_t GetArbitrateResult();

    void ArbitrateInitial(const uint16_t arbitrate_address);
    void ArbitrateStart(const uint16_t arbitrate_address);
    void ArbitrateFinish(const uint16_t arbitrate_address);
    void ArbitrateRelease(const uint16_t arbitrate_address);

    void ArbitrateAddressWorking(const uint16_t arbitrate_address);
    void ArbitrateAddressKeeping(const uint16_t arbitrate_address);

    void HandleArbitrateAddressQueue();
    bool ArbitrateAddressQueueIsEmpty();
    void ClearArbitrateAddressQueue();
    void AddArbitrateAddress(const uint16_t address);
    uint16_t GetArbitrateAddress();

    void HandleCacheMessage(const uint16_t arbitrate_address, const uint8_t type, const ArbitrateCacheMsg& message);
    void CacheMessage(const uint16_t arbitrate_address, const ArbitrateCacheMsg& message);
    bool HasCacheMessage(const uint16_t arbitrate_address);
    bool SendCacheMessage(const uint16_t arbitrate_address);
    bool SendPendingMessage(const uint16_t arbitrate_address);

    void StartArbitrationTimer(const uint16_t arbitrate_address);
    void StartP2Timer(const uint16_t arbitrate_address);
    void StartP2StarTimer(const uint16_t arbitrate_address);

private:
    void CreateArbitrationTimer(const uint16_t arbitrate_address);
    ArbitrationTimer::Ptr GetArbitrationTimer(const uint16_t arbitrate_address);

    explicit Arbitration(Arbitration&&) = delete;
    Arbitration(Arbitration&) = delete;
    Arbitration& operator=(Arbitration&) = delete;
    Arbitration& operator=(Arbitration&&) = delete;

private:
    std::string config_;
    bool arbitration_enable_;
    int arbitration_timeout_;
    int p2_;
    int p2_star_;

    ArbitrateMap arbitrate_map_;
    ArbitrateInfoMap arbitrate_info_map_;
    ArbitrateCacheMap arbitrate_cache_map_;
    ArbitrateTimerMap arbitrate_timer_map_;
    ArbitrateSubscribeMap arbitrate_subscribe_map_;

    std::mutex arbitrate_mutex_;
    std::mutex arbitrate_cache_mutex_;
    std::mutex arbitrate_timer_mutex_;
    std::mutex mutex_;

    std::atomic_bool exit_requested_;
    std::condition_variable condition_variable_;
    std::queue<uint16_t> arbitrate_address_queue_;
    std::thread thread_;

};

} /* namespace arbitration */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_ARBITRATION_ARBITRATION_H_
