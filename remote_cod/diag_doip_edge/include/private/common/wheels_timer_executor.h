#ifndef ASF_DIAG_PROXY_INCLUDE_COMMON_WHEELS_TIMER_EXECUTOR_H_
#define ASF_DIAG_PROXY_INCLUDE_COMMON_WHEELS_TIMER_EXECUTOR_H_

#include "neusar_wheels/timer/timer_channel.h"
#include "neusar_wheels/timer/timeout.h"
#include "neusar_wheels/executor/io_thread_pool_executor.h"

namespace asf
{
namespace diag
{
namespace common
{

class WheelsTimerExecutor
{
public:
    static WheelsTimerExecutor& GetInstance()
    {
        static WheelsTimerExecutor instance;
        return instance;
    }

    inline std::shared_ptr<wheels::TimerChannel> GetDiagnosticMessageChannel()
    {
        return diagnostic_message_channel_;
    }

    inline std::shared_ptr<wheels::TimerChannel> GetReceivingChannel()
    {
        return receiving_channel_;
    }

    inline std::shared_ptr<wheels::TimerChannel> GetArbitrationChannel()
    {
        return arbitration_channel_;
    }

    inline std::shared_ptr<wheels::TimerChannel> GetPendingChannel()
    {
        return pending_channel_;
    }

    inline std::shared_ptr<wheels::TimerChannel> GetAuthenticationChannel()
    {
        return authentication_channel_;
    }

    inline std::shared_ptr<wheels::TimerChannel> GetTargetExemptionChannel()
    {
        return target_exemption_channel_;
    }

private:
    explicit WheelsTimerExecutor();
    virtual ~WheelsTimerExecutor() noexcept;

private:
    wheels::IOThreadPoolExecutor executor_;

    std::shared_ptr<wheels::TimerChannel> diagnostic_message_channel_;

    std::shared_ptr<wheels::TimerChannel> receiving_channel_;

    std::shared_ptr<wheels::TimerChannel> arbitration_channel_;

    std::shared_ptr<wheels::TimerChannel> pending_channel_;

    std::shared_ptr<wheels::TimerChannel> authentication_channel_;

    std::shared_ptr<wheels::TimerChannel> target_exemption_channel_;

};

} /* namespace common */
} /* namespace diag */
} /* namespace asf */

#endif // ASF_DIAG_PROXY_INCLUDE_COMMON_WHEELS_TIMER_EXECUTOR_H_
