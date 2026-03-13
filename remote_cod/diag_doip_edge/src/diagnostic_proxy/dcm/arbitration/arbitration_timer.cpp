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

#include "dcm/arbitration/arbitration_timer.h"

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

ArbitrationTimer::ArbitrationTimer(Arbitration& arbitration, const uint16_t address, const int timeout, const int p2, const int p2_star)
    : arbitration_(arbitration)
    , arbitration_address_(address)
    , arbitration_timeout_(timeout)
    , p2_(50)
    , p2_star_(2000)
    , arbitration_timer(nullptr)
    , pending_timer_(nullptr)
{
    const auto nrc_0x78 = static_cast<float>(p2_star) * 0.3;
    p2_ = static_cast<uint16_t>(p2);
    p2_star_ = static_cast<uint16_t>(nrc_0x78);

    const auto arbitration_channel_ = common::WheelsTimerExecutor::GetInstance().GetArbitrationChannel();
    const auto pending_channel = common::WheelsTimerExecutor::GetInstance().GetPendingChannel();

    arbitration_timer = std::make_shared<common::WheelsTimer>(arbitration_channel_, [this](){ this->ArbitrationTimeout(); });
    pending_timer_ = std::make_shared<common::WheelsTimer>(pending_channel, [this](){ this->PendingTimeout(); });

}

ArbitrationTimer::~ArbitrationTimer()
{
    if (arbitration_timer != nullptr)
    {
        arbitration_timer = nullptr;
    }
    if (pending_timer_ != nullptr)
    {
        pending_timer_ = nullptr;
    }
}

void ArbitrationTimer::StartArbitrationTimer()
{
    if (arbitration_timer->IsRunning())
    {
        arbitration_timer->Stop();
    }
    (void)arbitration_timer->StartOnce(std::chrono::milliseconds(arbitration_timeout_));
}

void ArbitrationTimer::StopArbitrationTimer()
{
    if (arbitration_timer->IsRunning())
    {
        arbitration_timer->Stop();
    }
}

void ArbitrationTimer::StartP2Timer()
{
    if (pending_timer_->IsRunning())
    {
        pending_timer_->Stop();
    }
    (void)pending_timer_->StartOnce(std::chrono::milliseconds(p2_));
}

void ArbitrationTimer::StartP2StarTimer()
{
    if (pending_timer_->IsRunning())
    {
        pending_timer_->Stop();
    }
    (void)pending_timer_->StartOnce(std::chrono::milliseconds(p2_star_));
}

void ArbitrationTimer::StopPendingTimer()
{
    if (pending_timer_->IsRunning())
    {
        pending_timer_->Stop();
    }
}

void ArbitrationTimer::ArbitrationTimeout()
{
    arbitration_.ArbitrateAddressRelease(arbitration_address_);
}

void ArbitrationTimer::PendingTimeout()
{
    arbitration_.ArbitrateAddressPending(arbitration_address_);
}

} /* namespace arbitration */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
