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

#include "dcm/authentication/authentication_timer.h"

#include "dcm/authentication/authentication.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace authentication
{

AuthenticationTimer::AuthenticationTimer(Authentication& authentication, const int timeout)
    : authentication_(authentication)
    , authentication_timeout_(timeout)
{
    const auto authentication_channel = common::WheelsTimerExecutor::GetInstance().GetAuthenticationChannel();
    const auto target_exemption_channel = common::WheelsTimerExecutor::GetInstance().GetTargetExemptionChannel();

    authentication_timer_ = std::make_shared<common::WheelsTimer>(authentication_channel, [this](){ this->HandleAuthenticationTimeout(); });
    target_exemption_timer_ = std::make_shared<common::WheelsTimer>(target_exemption_channel, [this](){ this->HandleTargetExemptionTimeout(); });
}

AuthenticationTimer::~AuthenticationTimer()
{
    if (authentication_timer_ != nullptr)
    {
        authentication_timer_ = nullptr;
    }
    if (target_exemption_timer_ != nullptr)
    {
        target_exemption_timer_ = nullptr;
    }
}

void AuthenticationTimer::StartAuthenticationTimer()
{
    if (authentication_timer_->IsRunning())
    {
        authentication_timer_->Stop();
    }
    (void)authentication_timer_->StartOnce(std::chrono::milliseconds(authentication_timeout_));
}

void AuthenticationTimer::StopAuthenticationTimer()
{
    if (authentication_timer_->IsRunning())
    {
        authentication_timer_->Stop();
    }
}

void AuthenticationTimer::StartTargetExemptionTimer()
{
    if (target_exemption_timer_->IsRunning())
    {
        target_exemption_timer_->Stop();
    }
    (void)target_exemption_timer_->StartOnce(std::chrono::milliseconds(authentication_timeout_));
}

void AuthenticationTimer::StopTargetExemptionTimer()
{
    if (target_exemption_timer_->IsRunning())
    {
        target_exemption_timer_->Stop();
    }
}

void AuthenticationTimer::HandleAuthenticationTimeout()
{
    authentication_.AuthenticationTimeout();
}

void AuthenticationTimer::HandleTargetExemptionTimeout()
{
    authentication_.TargetExemptionTimeout();
}

} /* namespace authentication */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
