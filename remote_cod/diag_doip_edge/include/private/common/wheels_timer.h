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

#ifndef ASF_DIAG_PROXY_INCLUDE_COMMON_WHEELS_TIMER_H_
#define ASF_DIAG_PROXY_INCLUDE_COMMON_WHEELS_TIMER_H_

#include "neusar_wheels/timer/timer_channel.h"
#include "neusar_wheels/timer/timeout.h"
#include "neusar_wheels/executor/io_thread_pool_executor.h"
#include <sys/time.h>
#include <functional>
#include <memory>
#include <mutex>
#include <atomic>

namespace asf
{
namespace diag
{
namespace common
{

class WheelsTimer
{
public:
    using TimerHandler = std::function<void()>;

public:
    /**
     * \brief Constructor fct. for objects of class WheelsTimer. 
     */
    explicit WheelsTimer(const std::shared_ptr<wheels::TimerChannel> channel, const TimerHandler timer_handler);

    /**
     * \brief Destructor that implicitly stops the timer and joins the timer
     * handler thread.
     */
    virtual ~WheelsTimer() = default;

    /**
     * \brief Start a timer with the specified delay that stops itself after it
     * fires.
     */
    bool StartOnce(const std::chrono::milliseconds duration);

    /**
     * \brief Stop the timer.
     */
    void Stop();

    /**
     * \brief Get Timer remaining time, return milliseconds.
     */
    long GetRemainingTime();

    /**
     * \brief Determine whether the timer is currently running, i.e., will fire at
     * some point in the future.
     */
    inline bool IsRunning()
    {
        const std::lock_guard<std::mutex> locker_is_running(mutex_);
        return running_;
    }

protected:
    inline void IsRunning(const bool running)
    {
        const std::lock_guard<std::mutex> locker_is_running(mutex_);
        running_ = running;
    }

private:
    void TimerOnceFunc();

private:
    std::shared_ptr<wheels::TimerChannel> channel_;

    wheels::Timeout timer_;

    TimerHandler timer_handler_;

    std::mutex mutex_;

    /**
     * \brief Flag to indicate whether the timer is currently active.
     */
    bool running_;

};

} /* namespace common */
} /* namespace diag */
} /* namespace asf */

#endif /* ASF_DIAG_PROXY_INCLUDE_COMMON_WHEELS_TIMER_H_ */
