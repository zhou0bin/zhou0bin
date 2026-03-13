#include "doip_power_mode.h"
#include "diag_libnet_client_channel.h"

#include "log.h"

namespace midware
{
namespace diag
{
using asf::diag::msg_on_neusar_net::DoIpMsgType;

using DoIPPowerModeChannel = asf::diag::communication::ClientChannel<DoIPPowerMode>;

DoIPPowerMode::DoIPPowerMode(const InstanceSpecifier& specifier)
    : specifier_(specifier)
    , channel_path_("/tmp/ASF_Diag_DoIP_Info_Interface")
    , offer_status_(false)
{
    DoIPPowerModeChannel::GetInstance().InsertService(specifier_, this);
    DoIPPowerModeChannel::GetInstance().SetAvaliableNotifier(specifier_,
                                                             [this](const bool avaliable)
                                                             { ChannelAvaliable(avaliable); });
}

DoIPPowerMode::~DoIPPowerMode()
{
    try {
        DoIPPowerModeChannel::GetInstance().EraseService(specifier_);
    } catch(...) {}
}

void DoIPPowerMode::ChannelAvaliable(const bool avaliable) const
{
    if (true == avaliable) {
        if (offer_status_) {
            (void)DoIPPowerModeChannel::GetInstance().SendOffer(specifier_);
        }
    }
}

void DoIPPowerMode::HandleMessage(const uint16_t type, const std::vector<uint8_t>& data)
{
    if (!(data.size() > 0))
    {
        LOG_ERROR << __FUNCTION__ << "data size is 0. ";
        return;
    }
    LOG_INFO << __FUNCTION__ << "type: " << LOG_HEX(type);

    std::vector<uint8_t> buffer{};
    PowerModeType future_mode{PowerModeType::kNotReady};
    if (DoIpMsgType::kGetDoIPPowerMode == type)
    {
        auto outputFuture = GetDoIPPowerMode();
        future_mode = outputFuture.get();
    }
    const uint8_t power_mode = static_cast<uint8_t>(future_mode);
    buffer.push_back(power_mode);
    if (DoIPPowerModeChannel::GetInstance().SendMessage(channel_path_, specifier_, type, buffer) == -1)
    {
        LOG_ERROR << __FUNCTION__ << "SendMessage fail! ";
    }
}

int DoIPPowerMode::Offer()
{
    int ret{0};
    if (!offer_status_) {
        offer_status_ = true;
        ret = DoIPPowerModeChannel::GetInstance().Offer(channel_path_, specifier_,
                                                           [this](const uint16_t type, const std::vector<uint8_t>& data)
                                                           { HandleMessage(type, data); });
    }
    return ret;
}

void DoIPPowerMode::StopOffer()
{
    if (offer_status_) {
        offer_status_ = false;
        DoIPPowerModeChannel::GetInstance().StopOffer(specifier_);
    }
}

}  // namespace diag
}  // namespace midware
