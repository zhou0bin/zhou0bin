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

#ifndef DCM_ARBITRATION_ARBITRATION_TIMER_H_
#define DCM_ARBITRATION_ARBITRATION_TIMER_H_

#include "common/wheels_timer.h"
#include "common/wheels_timer_executor.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace arbitration
{

class Arbitration;

class ArbitrationTimer
{
public:
    using Ptr = std::shared_ptr<ArbitrationTimer>;

public:
    /**
     * \brief Constructor for ArbitrationTimer. 
     */
    explicit ArbitrationTimer(Arbitration& arbitration, const uint16_t address, const int timeout, const int p2, const int p2_star);
    virtual ~ArbitrationTimer() noexcept;

    void StartArbitrationTimer();
    void StopArbitrationTimer();

    void StartP2Timer();
    void StartP2StarTimer();
    void StopPendingTimer();

protected:
    void ArbitrationTimeout();
    void PendingTimeout();

private:
    explicit ArbitrationTimer(ArbitrationTimer&&) = delete;
    ArbitrationTimer(const ArbitrationTimer&) = delete;
    ArbitrationTimer& operator=(const ArbitrationTimer&) = delete;
    ArbitrationTimer& operator=(ArbitrationTimer&&) = delete;

private:
    Arbitration& arbitration_;
    uint16_t arbitration_address_;
    uint16_t arbitration_timeout_;

    uint16_t p2_;
    uint16_t p2_star_;

    std::shared_ptr<common::WheelsTimer> arbitration_timer;
    std::shared_ptr<common::WheelsTimer> pending_timer_;

};

} /* namespace arbitration */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif /* DCM_ROUTE_ARBITRATION_TIMER_H_ */
