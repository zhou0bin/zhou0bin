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

#ifndef DCM_ROUTE_ROUTING_TIMER_H_
#define DCM_ROUTE_ROUTING_TIMER_H_

#include "common/wheels_timer.h"
#include "common/wheels_timer_executor.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class Routing;

class RoutingTimer
{
public:
    using Ptr = std::shared_ptr<RoutingTimer>;

public:
    /**
     * \brief Constructor for RoutingTimer. 
     */
    explicit RoutingTimer(Routing& routing, const uint16_t address, const int p2, const int p2_star, const int transmission_delay);
    virtual ~RoutingTimer() noexcept;

    void StartReceving(const uint8_t sid, const bool suppress);
    void StopReceving(const uint8_t sid);

    void CheckDiagnosticAck();
    void CheckDiagnosticNack();
    void CheckDiagnosticMsg(const uint8_t sid);
    void CheckResponsePending(const uint8_t sid);

protected:
    void SetRecevingInfo(const uint8_t sid, const bool suppress);
    uint8_t GetRecevingSid();
    bool GetRecevingSuppress();

    void RecevingFinish();
    void RecevingTimeout();

private:
    explicit RoutingTimer(RoutingTimer&&) = delete;
    RoutingTimer(const RoutingTimer&) = delete;
    RoutingTimer& operator=(const RoutingTimer&) = delete;
    RoutingTimer& operator=(RoutingTimer&&) = delete;

private:
    Routing& routing_;
    uint16_t address_;

    uint16_t diagnostic_message_timeout_;
    uint16_t p2_;
    uint16_t p2_star_;
    uint16_t p6_;
    uint16_t p6_star_;

    uint8_t sid_;
    bool suppress_;

    std::mutex mutex_;

    std::shared_ptr<common::WheelsTimer> diagnostic_message_timer_;
    std::shared_ptr<common::WheelsTimer> receiving_timer_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif /* DCM_ROUTE_ROUTING_TIMER_H_ */
