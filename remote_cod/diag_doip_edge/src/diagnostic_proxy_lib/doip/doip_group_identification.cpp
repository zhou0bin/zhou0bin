#include "doip_group_identification.h"
#include "diag_libnet_client_channel.h"

#include "log.h"

namespace midware
{
namespace diag
{
using asf::diag::msg_on_neusar_net::DoIpMsgType;

using DoIPGroupIdentificationChannel = asf::diag::communication::ClientChannel<DoIPGroupIdentification>;

DoIPGroupIdentification::DoIPGroupIdentification(const InstanceSpecifier& specifier)
    : specifier_(specifier)
    , channel_path_("/tmp/ASF_Diag_DoIP_Info_Interface")
    , offer_status_(false)
{
    DoIPGroupIdentificationChannel::GetInstance().InsertService(specifier_, this);
    DoIPGroupIdentificationChannel::GetInstance().SetAvaliableNotifier(specifier_,
                                                                       [this](const bool avaliable)
                                                                       { ChannelAvaliable(avaliable); });
}

DoIPGroupIdentification::~DoIPGroupIdentification()
{
    try {
        DoIPGroupIdentificationChannel::GetInstance().EraseService(specifier_);
    } catch(...) {}
}

void DoIPGroupIdentification::ChannelAvaliable(const bool avaliable) const
{
    if (true == avaliable) {
        if (offer_status_) {
            (void)DoIPGroupIdentificationChannel::GetInstance().SendOffer(specifier_);
        }
    }
}

void DoIPGroupIdentification::HandleMessage(const uint16_t type, const std::vector<uint8_t>& data)
{
    if (!(data.size() > 0))
    {
        LOG_ERROR << __FUNCTION__ << "data size is 0. ";
        return;
    }
    LOG_INFO << __FUNCTION__ << "type: " << LOG_HEX(type);
    GidStatus status;
    if (DoIpMsgType::kGetGidStatus == type)
    {
        auto outputFuture = GetGidStatus();
        status = outputFuture.get();
    }
    if (status.GID.size() != 6)
    {
        LOG_ERROR << __FUNCTION__ << "gid size error! size: " << status.GID.size();
        return;
    }
    std::vector<uint8_t> buffer{status.GID};
    buffer.push_back(status.furtherActionReq);
    buffer.push_back(status.syncStatus);
    if (DoIPGroupIdentificationChannel::GetInstance().SendMessage(channel_path_, specifier_, type, buffer) == -1)
    {
        LOG_ERROR << __FUNCTION__ << "SendMessage fail! ";
    }
}

int DoIPGroupIdentification::Offer()
{
    int ret{0};
    if (!offer_status_) {
        offer_status_ = true;
        ret = DoIPGroupIdentificationChannel::GetInstance().Offer(channel_path_, specifier_,
                                                                  [this](const uint16_t type, const std::vector<uint8_t>& data)
                                                                  { HandleMessage(type, data); });
    }
    return ret;
}

void DoIPGroupIdentification::StopOffer()
{
    if (offer_status_) {
        offer_status_ = false;
        DoIPGroupIdentificationChannel::GetInstance().StopOffer(specifier_);
    }
}

}  // namespace diag
}  // namespace midware
