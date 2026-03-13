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

#ifndef ASF_DIAG_PROXY_LIB_DIAG_AUTHENTICATION_MODE_H_
#define ASF_DIAG_PROXY_LIB_DIAG_AUTHENTICATION_MODE_H_

#include "diag_proxy_app_common.h"

namespace midware
{
namespace diag
{

class DiagAuthenticationMode 
{
public:
    explicit DiagAuthenticationMode();
    virtual ~DiagAuthenticationMode() noexcept;

    int Offer();

    void StopOffer();

    /**
     * \brief set authentication mode to DP.
     */
    bool SetDiagAuthenticationMode(const uint8_t value);

    /**
     * \brief get authentication mode from DP.
     */
    virtual void GetDiagAuthenticationMode(uint8_t &value) = 0;

protected:
    void ChannelAvaliable(const bool avaliable) const;

    void HandleMessage(const uint16_t type, const std::vector<uint8_t>& data);

private:
    DiagAuthenticationMode(const DiagAuthenticationMode&) = delete;
    explicit DiagAuthenticationMode(DiagAuthenticationMode&&) = delete;
    DiagAuthenticationMode& operator=(const DiagAuthenticationMode&) = delete;
    DiagAuthenticationMode& operator=(DiagAuthenticationMode&&) = delete;

private:
    std::string channel_path_;

    std::string specifier_;

    bool offer_status_;

    std::mutex mutex_;

};

} // diag
} // midware

#endif  // ASF_DIAG_PROXY_LIB_DIAG_AUTHENTICATION_MODE_H_
