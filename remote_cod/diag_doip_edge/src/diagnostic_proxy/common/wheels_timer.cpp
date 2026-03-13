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

#include "common/wheels_timer.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace common
{

WheelsTimer::WheelsTimer(const std::shared_ptr<wheels::TimerChannel> channel, const TimerHandler timer_handler)
    : channel_(channel)
    , timer_(channel_.get())
    , timer_handler_(timer_handler)
    , mutex_()
    , running_(false)
{

}

bool WheelsTimer::StartOnce(const std::chrono::milliseconds duration)
{
    const std::lock_guard<std::mutex> locker_start_once(mutex_);
    if (!running_)
    {
        // start timer, callback run delay duration
#ifdef USE_FS_LIBNET
        bool start_ststus = timer_.SetExpired(duration);
#else
        bool start_ststus = timer_.setExpired(duration);
#endif
        if (start_ststus)
        {
#ifdef USE_FS_LIBNET
            start_ststus = timer_.Start([this]() { this->TimerOnceFunc(); });
#else
            start_ststus = timer_.start([this]() { this->TimerOnceFunc(); });
#endif
            if (start_ststus)
            {
                running_ = true;
                return true;
            }
            else
            {
                LOG_ERROR << __FUNCTION__ << "start error! ";
            }
        }
        else
        {
            LOG_ERROR << __FUNCTION__ << "setExpired error! ";
        }
    }
    return false;
}

void WheelsTimer::TimerOnceFunc()
{
    IsRunning(false);
    if (timer_handler_ != nullptr)
    {
        timer_handler_();
    }
}

void WheelsTimer::Stop()
{
#ifdef USE_FS_LIBNET
    (void)timer_.Cancel();
#else
    (void)timer_.cancel();
#endif
    IsRunning(false);
}

long WheelsTimer::GetRemainingTime()
{
    long time_grt = -1;
    if (IsRunning())
    {
        struct timespec expired_ts;
        struct timespec now_ts;
#ifdef USE_FS_LIBNET
        (void)wheels::GetTime(&now_ts);
        expired_ts = timer_.GetExpired();
#else
        (void)wheels::getTime(&now_ts);
        expired_ts = timer_.getExpired();
#endif
        const long now_ms = now_ts.tv_sec * 1000 + now_ts.tv_nsec / 1000000;
        const long expired_ms = expired_ts.tv_sec * 1000 + expired_ts.tv_nsec / 1000000;
        time_grt = expired_ms - now_ms;
    }
    return time_grt;
}

} /* namespace common */
} /* namespace diag */
} /* namespace asf */
