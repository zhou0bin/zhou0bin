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

#ifndef ASF_DIAG_PROXY_LIB_DIAG_AUTHENTICATION_STATE_H_
#define ASF_DIAG_PROXY_LIB_DIAG_AUTHENTICATION_STATE_H_

#include "diag_proxy_app_common.h"

namespace midware
{
namespace diag
{

class DiagAuthenticationState
{
public:
    explicit DiagAuthenticationState();
    virtual ~DiagAuthenticationState() noexcept;

    int Offer();

    void StopOffer();

    /**
     * \brief authentication state from DP.
     */
    virtual void DiagAuthenticationStateChanged(const uint8_t value) = 0;

protected:
    void ChannelAvaliable(const bool avaliable) const;

    void HandleMessage(const uint16_t type, const std::vector<uint8_t>& data);

private:
    DiagAuthenticationState(DiagAuthenticationState&) = delete;
    explicit DiagAuthenticationState(DiagAuthenticationState&&) = delete;
    DiagAuthenticationState& operator=(DiagAuthenticationState&) = delete;
    DiagAuthenticationState& operator=(DiagAuthenticationState&&) = delete;

private:
    std::string channel_path_;

    std::string specifier_;

    bool offer_status_;

};

} // diag
} // midware

#endif  // ASF_DIAG_PROXY_LIB_DIAG_AUTHENTICATION_STATE_H_
