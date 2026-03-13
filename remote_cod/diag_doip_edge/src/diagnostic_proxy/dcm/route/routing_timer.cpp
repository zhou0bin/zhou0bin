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

#include "dcm/route/routing_timer.h"

#include "dcm/route/routing.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

RoutingTimer::RoutingTimer(Routing& routing, const uint16_t address, const int p2, const int p2_star, const int transmission_delay)
    : routing_(routing)
    , address_(address)
    , diagnostic_message_timeout_(2000)
    , p2_(50)
    , p2_star_(2000)
    , p6_(50)
    , p6_star_(2000)
    , sid_(0U)
    , suppress_(false)
    , mutex_()
    , diagnostic_message_timer_(nullptr)
    , receiving_timer_(nullptr)
{
    const auto nrc_0x78 = static_cast<float>(p2_star) * 0.3;
    p2_ = static_cast<uint16_t>(p2);
    p2_star_ = static_cast<uint16_t>(nrc_0x78);
    p6_ = static_cast<uint16_t>(p2 + transmission_delay);
    p6_star_ = static_cast<uint16_t>(p2_star + transmission_delay);

    const auto diagnostic_message_channel = common::WheelsTimerExecutor::GetInstance().GetDiagnosticMessageChannel();
    const auto receiving_channel = common::WheelsTimerExecutor::GetInstance().GetReceivingChannel();

    diagnostic_message_timer_ = std::make_shared<common::WheelsTimer>(diagnostic_message_channel, [this](){ this->RecevingTimeout(); });
    receiving_timer_ = std::make_shared<common::WheelsTimer>(receiving_channel, [this](){ this->RecevingTimeout(); });

}

RoutingTimer::~RoutingTimer()
{
    if (diagnostic_message_timer_ != nullptr)
    {
        diagnostic_message_timer_ = nullptr;
    }
    if (receiving_timer_ != nullptr)
    {
        receiving_timer_ = nullptr;
    }
}

void RoutingTimer::StartReceving(const uint8_t sid, const bool suppress)
{
    SetRecevingInfo(sid, suppress);
    if (receiving_timer_->IsRunning())
    {
        receiving_timer_->Stop();
    }
    if (diagnostic_message_timer_->IsRunning())
    {
        diagnostic_message_timer_->Stop();
    }
    (void)diagnostic_message_timer_->StartOnce(std::chrono::milliseconds(diagnostic_message_timeout_));
}

void RoutingTimer::StopReceving(const uint8_t sid)
{
    if (GetRecevingSid() == sid)
    {
        if (diagnostic_message_timer_->IsRunning())
        {
            diagnostic_message_timer_->Stop();
        }
        if (receiving_timer_->IsRunning())
        {
            receiving_timer_->Stop();
        }
    }
}

void RoutingTimer::CheckDiagnosticAck()
{
    if (diagnostic_message_timer_->IsRunning())
    {
        diagnostic_message_timer_->Stop();
        if (GetRecevingSuppress() == false)
        {
            if (receiving_timer_->IsRunning())
            {
                receiving_timer_->Stop();
            }
            (void)receiving_timer_->StartOnce(std::chrono::milliseconds(p6_));
        }
        else
        {
            RecevingFinish();
        }
    }
}

void RoutingTimer::CheckDiagnosticNack()
{
    if (diagnostic_message_timer_->IsRunning())
    {
        diagnostic_message_timer_->Stop();
        RecevingFinish();
    }
}

void RoutingTimer::CheckDiagnosticMsg(const uint8_t sid)
{
    if (GetRecevingSid() == sid)
    {
        if (receiving_timer_->IsRunning())
        {
            receiving_timer_->Stop();
            RecevingFinish();
        }
    }
}

void RoutingTimer::CheckResponsePending(const uint8_t sid)
{
    if (GetRecevingSid() == sid)
    {
        if (receiving_timer_->IsRunning())
        {
            receiving_timer_->Stop();
            (void)receiving_timer_->StartOnce(std::chrono::milliseconds(p6_star_));
        }
    }
}


void RoutingTimer::SetRecevingInfo(const uint8_t sid, const bool suppress)
{
    const std::lock_guard<std::mutex> locker(mutex_);
    sid_ = sid;
    suppress_ = suppress;
}

uint8_t RoutingTimer::GetRecevingSid()
{
    const std::lock_guard<std::mutex> locker(mutex_);
    return sid_;
}

bool RoutingTimer::GetRecevingSuppress()
{
    const std::lock_guard<std::mutex> locker(mutex_);
    return suppress_;
}

void RoutingTimer::RecevingFinish()
{
    routing_.ProcessorRecevingFinish(address_);
}

void RoutingTimer::RecevingTimeout()
{
    routing_.ProcessorRecevingFinish(address_);
}

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
