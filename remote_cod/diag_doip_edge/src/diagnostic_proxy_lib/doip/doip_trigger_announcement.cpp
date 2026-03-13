
#include "doip_trigger_announcement.h"
#include "diag_libnet_client_channel.h"
#include "log.h"

namespace midware
{
namespace diag
{
using asf::diag::msg_on_neusar_net::DoIpMsgType;

using DoIPTriggerVehicleAnnouncementChannel = asf::diag::communication::ClientChannel<DoIPTriggerVehicleAnnouncement>;

DoIPTriggerVehicleAnnouncement& DoIPTriggerVehicleAnnouncement::GetDoIPTriggerVehicleAnnouncement()
{
    static DoIPTriggerVehicleAnnouncement m_instance;
    return m_instance;
}

DoIPTriggerVehicleAnnouncement::DoIPTriggerVehicleAnnouncement()
    : specifier_("Diag_DoIP_Vehicle_Announce_Interface")
    , channel_path_("/tmp/ASF_Diag_DoIP_Info_Interface")
    , offer_status_(true)
    , report_result_(false)
    , condition_variable_()
    , mutex_()
{
    DoIPTriggerVehicleAnnouncementChannel::GetInstance().InsertService(specifier_, this);
    DoIPTriggerVehicleAnnouncementChannel::GetInstance().SetAvaliableNotifier(specifier_,
                                                                              [this](const bool avaliable)
                                                                              { ChannelAvaliable(avaliable); });
    (void)DoIPTriggerVehicleAnnouncementChannel::GetInstance().Offer(channel_path_, specifier_,
                                                                     [this](const uint16_t type, const std::vector<uint8_t>& data)
                                                                     { HandleMessage(type, data); });
}

DoIPTriggerVehicleAnnouncement::~DoIPTriggerVehicleAnnouncement()
{
    try {
        DoIPTriggerVehicleAnnouncementChannel::GetInstance().EraseService(specifier_);
    } catch(...) {}
}

void DoIPTriggerVehicleAnnouncement::ChannelAvaliable(const bool avaliable) const
{
    if (true == avaliable) {
        if (offer_status_) {
            (void)DoIPTriggerVehicleAnnouncementChannel::GetInstance().SendOffer(specifier_);
        }
    }
}

bool DoIPTriggerVehicleAnnouncement::TriggerVehicleAnnouncement(uint8_t networkInterfaceId)
{
    LOG_INFO << __FUNCTION__ << "networkInterfaceId: " << LOG_HEX(networkInterfaceId);
    std::unique_lock<std::mutex> locker(mutex_);
    const std::vector<uint8_t> buffer{networkInterfaceId};
    const int send_result = DoIPTriggerVehicleAnnouncementChannel::GetInstance().SendMessage(channel_path_, specifier_, DoIpMsgType::kTriggerVehicleAnnouncement, buffer);
    if (send_result == 0)
    {
        LOG_INFO << __FUNCTION__ << "report ok! ";
        const auto expiry_point =  std::chrono::steady_clock::now() + std::chrono::seconds(2);
        const std::cv_status wait_status = condition_variable_.wait_until(locker, expiry_point);
        if (wait_status == std::cv_status::timeout)
        {
            LOG_ERROR << __FUNCTION__ << "wait report result, timeout! ";
            return false;
        }
        else
        {
            LOG_INFO << __FUNCTION__ << "wait report result, OK! " << "result: " << report_result_;
            if (report_result_ == false)
            {
                return false;
            }
        }
    }
    else
    {
        LOG_ERROR << __FUNCTION__ << "report failed! ";
        return false;
    }
    return true;
}

void DoIPTriggerVehicleAnnouncement::HandleMessage(const uint16_t type, const std::vector<uint8_t>& data)
{
    if (!(data.size() > 0))
    {
        LOG_ERROR << __FUNCTION__ << "data size is 0. ";
        return;
    }
    const bool result = static_cast<bool>(data.at(0));
    LOG_INFO << __FUNCTION__ << "type: " << LOG_HEX(type) << "result: " << result;
    if (DoIpMsgType::kTriggerVehicleAnnouncement == type)
    {
        LOG_INFO << __FUNCTION__ << "result: " << result;
        report_result_ = result;
        condition_variable_.notify_one();
    }
    else
    {
        LOG_ERROR << __FUNCTION__ << "unknow type! ";
    }
}

} // diag
} // midware
