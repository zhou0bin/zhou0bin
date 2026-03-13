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

#ifndef ASF_DIAG_PROXY_AUTHENTICATION_LIB_AUTHENTICATION_RESULT_H_
#define ASF_DIAG_PROXY_AUTHENTICATION_LIB_AUTHENTICATION_RESULT_H_

#include "diag_proxy_app_common.h"

namespace midware
{
namespace diag
{

class DiagAuthenticationResult 
{
public:
    explicit DiagAuthenticationResult();
    virtual ~DiagAuthenticationResult() noexcept;

    int Offer();

    void StopOffer();

    /**
     * \brief set authentication result to DP.
     */
    bool SetDiagAuthenticationResult(const uint8_t value);

    /**
     * \brief get authentication result from DP.
     */
    virtual void GetDiagAuthenticationResult(uint8_t &value) = 0;

protected:
    void ChannelAvaliable(const bool avaliable) const;

    void HandleMessage(const uint16_t type, const std::vector<uint8_t>& data);

private:
    DiagAuthenticationResult(const DiagAuthenticationResult&) = delete;
    explicit DiagAuthenticationResult(DiagAuthenticationResult&&) = delete;
    DiagAuthenticationResult& operator=(const DiagAuthenticationResult&) = delete;
    DiagAuthenticationResult& operator=(DiagAuthenticationResult&&) = delete;

private:
    std::string channel_path_;

    std::string specifier_;

    bool offer_status_;

    std::mutex mutex_;

};

} // diag
} // midware

#endif  // ASF_DIAG_PROXY_AUTHENTICATION_LIB_AUTHENTICATION_RESULT_H_
