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

#include "common/wheels_timer_executor.h"

namespace asf
{
namespace diag
{
namespace common
{

WheelsTimerExecutor::WheelsTimerExecutor()
    : executor_(6U)
    , diagnostic_message_channel_(nullptr)
    , receiving_channel_(nullptr)
    , arbitration_channel_(nullptr)
    , pending_channel_(nullptr)
    , authentication_channel_(nullptr)
    , target_exemption_channel_(nullptr)
{
#ifdef USE_FS_LIBNET
    diagnostic_message_channel_ = std::make_shared<wheels::TimerChannel>(executor_.GetEventBase());
    receiving_channel_ = std::make_shared<wheels::TimerChannel>(executor_.GetEventBase());
    arbitration_channel_ = std::make_shared<wheels::TimerChannel>(executor_.GetEventBase());
    pending_channel_ = std::make_shared<wheels::TimerChannel>(executor_.GetEventBase());
    authentication_channel_ = std::make_shared<wheels::TimerChannel>(executor_.GetEventBase());
    target_exemption_channel_ = std::make_shared<wheels::TimerChannel>(executor_.GetEventBase());
#else
    diagnostic_message_channel_ = std::make_shared<wheels::TimerChannel>(executor_.getEventBase());
    receiving_channel_ = std::make_shared<wheels::TimerChannel>(executor_.getEventBase());
    arbitration_channel_ = std::make_shared<wheels::TimerChannel>(executor_.getEventBase());
    pending_channel_ = std::make_shared<wheels::TimerChannel>(executor_.getEventBase());
    authentication_channel_ = std::make_shared<wheels::TimerChannel>(executor_.getEventBase());
    target_exemption_channel_ = std::make_shared<wheels::TimerChannel>(executor_.getEventBase());
#endif
}

WheelsTimerExecutor::~WheelsTimerExecutor()
{
    if (diagnostic_message_channel_ != nullptr)
    {
        diagnostic_message_channel_ = nullptr;
    }
    if (receiving_channel_ != nullptr)
    {
        receiving_channel_ = nullptr;
    }
    if (arbitration_channel_ != nullptr)
    {
        arbitration_channel_ = nullptr;
    }
    if (pending_channel_ != nullptr)
    {
        pending_channel_ = nullptr;
    }
    if (authentication_channel_ != nullptr)
    {
        authentication_channel_ = nullptr;
    }
    if (target_exemption_channel_ != nullptr)
    {
        target_exemption_channel_ = nullptr;
    }
}

} /* namespace common */
} /* namespace diag */
} /* namespace asf */
