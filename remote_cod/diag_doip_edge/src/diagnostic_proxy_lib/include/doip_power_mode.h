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

#ifndef ASF_DIAG_PROXY_LIB_DOIP_POWER_MODE_APP_H_
#define ASF_DIAG_PROXY_LIB_DOIP_POWER_MODE_APP_H_

#include "diag_proxy_app_common.h"

namespace midware
{
namespace diag
{
class DoIPPowerMode
{
public:
    /**
     * \brief Class for an DoIPPowerMode.
     *
     * \param specifier InstanceSpecifier to an PortPrototype of an DiagnosticDoIPPowerModeInterface
     */    
    explicit DoIPPowerMode(const InstanceSpecifier& specifier);
    /**
     * \brief Destructor of class DoIPPowerMode.
     */  
    virtual ~DoIPPowerMode() noexcept;

    /**
     * \brief Called to get the current Power Mode for the DoIP protocol.
     */  
    virtual std::future<PowerModeType> GetDoIPPowerMode() = 0;

    /**
     * \brief This Offer will enable the DP to forward request messages to this handler.
     * 
     * \return DiagErrorDomain::DiagReporting Errc::kNotOffered There was no Offer called before.
     *         DiagErrorDomain::DiagReporting Errc::kGenericError General error occurred.
     *         DiagErrorDomain::DiagReporting AlreadyOffered This service was already offered.
     */ 
    int Offer();

    /**
     * \brief This StopOffer will disable the forwaring of request messages from DP.
     */
    void StopOffer();

protected:
    void ChannelAvaliable(const bool avaliable) const;

    void HandleMessage(const uint16_t type, const std::vector<uint8_t>& data);

private:
    DoIPPowerMode(const DoIPPowerMode&) = delete;
    explicit DoIPPowerMode(DoIPPowerMode&&) = delete;
    DoIPPowerMode& operator=(const DoIPPowerMode&) = delete;
    DoIPPowerMode& operator=(DoIPPowerMode&&) = delete;

private:
    std::string specifier_;

    std::string channel_path_;

    bool offer_status_;

};

} // diag
} // midware

#endif  // ASF_DIAG_PROXY_LIB_DOIP_POWER_MODE_APP_H_
