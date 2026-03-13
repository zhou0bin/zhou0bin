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

#ifndef ASF_DIAG_PROXY_LIB_DOIP_ACTIVATION_LINE_H_
#define ASF_DIAG_PROXY_LIB_DOIP_ACTIVATION_LINE_H_

#include "diag_proxy_app_common.h"

namespace midware
{
namespace diag
{
class DoIPActivationLine
{
public:
    /**
     * \brief Class for an DoIPActivationLine.
     *
     * \param specifier InstanceSpecifier to an PortPrototype of an DiagnosticDoIPActivationLineInterface
     */    
    explicit DoIPActivationLine(const InstanceSpecifier& specifier);
    /**
     * \brief Destructor of class DoIPActivationLine.
     */  
    virtual ~DoIPActivationLine() noexcept;

    /**
     * \brief Called to get the network interface Id(see DoIpNetworkConfiguration.networkInterfaceId) for
     *        which this DoIPActivationLine instance is responsible.
     */  
    virtual std::future<std::uint8_t> GetNetworkInterfaceId() = 0;

    /**
     * \brief Called to get the current activation line state.
     */ 
    virtual std::future<bool> GetActivationLineState() = 0;

    /**
     * \brief Called to update current activation line state.
     */ 
    bool UpdateActivationLineState(bool state);

    /**
     * \brief This Offer will enable the DP to listen to activation line state changes for the given interface.
     * 
     * \return DiagErrorDomain::DiagReporting Errc::kNotOffered There was no Offer called before.
     *         DiagErrorDomain::DiagReporting Errc::kGenericError General error occurred.
     *         DiagErrorDomain::DiagReporting AlreadyOffered This service was already offered.
     */ 
    int Offer();

    /**
     * \brief This StopOffer will disable the provision of activation line state to DP
     */
    void StopOffer();

protected:
    void ChannelAvaliable(const bool avaliable) const;

    void HandleMessage(const uint16_t type, const std::vector<uint8_t>& data);

private:
    DoIPActivationLine(const DoIPActivationLine&) = delete;
    explicit DoIPActivationLine(DoIPActivationLine&&) = delete;
    DoIPActivationLine& operator=(const DoIPActivationLine&) = delete;
    DoIPActivationLine& operator=(DoIPActivationLine&&) = delete;

private:
    std::string specifier_;

    std::string channel_path_;

    bool offer_status_;

};


} // diag
} // midware

#endif  // ASF_DIAG_PROXY_LIB_DOIP_ACTIVATION_LINE_H_
