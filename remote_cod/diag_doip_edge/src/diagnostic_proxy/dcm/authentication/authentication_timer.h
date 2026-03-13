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

#ifndef DCM_AUTHENTICATION_AUTHENTICATION_TIMER_H_
#define DCM_AUTHENTICATION_AUTHENTICATION_TIMER_H_

#include "common/wheels_timer.h"
#include "common/wheels_timer_executor.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace authentication
{

class Authentication;

class AuthenticationTimer
{
public:
    using Ptr = std::shared_ptr<AuthenticationTimer>;

public:
    /**
     * \brief Constructor for AuthenticationTimer. 
     */
    explicit AuthenticationTimer(Authentication& authentication, const int timeout);
    virtual ~AuthenticationTimer() noexcept;

    void StartAuthenticationTimer();

    void StopAuthenticationTimer();

    void StartTargetExemptionTimer();

    void StopTargetExemptionTimer();

protected:
    void HandleAuthenticationTimeout();
    void HandleTargetExemptionTimeout();

private:
    Authentication& authentication_;
    int authentication_timeout_;

    std::shared_ptr<common::WheelsTimer> authentication_timer_;
    std::shared_ptr<common::WheelsTimer> target_exemption_timer_;

};

} /* namespace authentication */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif /* DCM_ROUTE_AUTHENTICATION_TIMER_H_ */
