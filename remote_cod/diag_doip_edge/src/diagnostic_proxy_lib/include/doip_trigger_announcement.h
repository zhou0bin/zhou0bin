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

#ifndef ASF_DIAG_PROXY_LIB_DOIP_TRIGGER_ANNOUNCMENT_APP_H_
#define ASF_DIAG_PROXY_LIB_DOIP_TRIGGER_ANNOUNCMENT_APP_H_

#include "diag_proxy_app_common.h"

namespace midware
{
namespace diag
{
class DoIPTriggerVehicleAnnouncement 
{
public:
    /**
     * \brief Get DoIPTriggerVehicleAnnouncement interface from DP.
     */   
    static DoIPTriggerVehicleAnnouncement& GetDoIPTriggerVehicleAnnouncement();

    /**
     * \brief Called by application to trigger DP sending out vehicle announcements on the given networkinterface Id.
     */
    bool TriggerVehicleAnnouncement(uint8_t networkInterfaceId);

protected:
    void ChannelAvaliable(const bool avaliable) const;

    void HandleMessage(const uint16_t type, const std::vector<uint8_t>& data);

private:
    explicit DoIPTriggerVehicleAnnouncement();
    virtual ~DoIPTriggerVehicleAnnouncement() noexcept;

    DoIPTriggerVehicleAnnouncement(const DoIPTriggerVehicleAnnouncement&) = delete;
    explicit DoIPTriggerVehicleAnnouncement(DoIPTriggerVehicleAnnouncement&&) = delete;
    DoIPTriggerVehicleAnnouncement& operator=(const DoIPTriggerVehicleAnnouncement&) = delete;
    DoIPTriggerVehicleAnnouncement& operator=(DoIPTriggerVehicleAnnouncement&&) = delete;

private:
    std::string specifier_;

    std::string channel_path_;

    bool offer_status_;

    bool report_result_;

    std::condition_variable condition_variable_;

    std::mutex mutex_;

};

} // diag
} // midware

#endif  // ASF_DIAG_PROXY_LIB_DOIP_TRIGGER_ANNOUNCMENT_APP_H_
