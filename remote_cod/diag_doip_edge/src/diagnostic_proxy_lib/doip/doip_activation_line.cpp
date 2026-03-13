#include "doip_activation_line.h"
#include "diag_libnet_client_channel.h"

#include "log.h"

namespace midware
{
namespace diag
{
using asf::diag::msg_on_neusar_net::DoIpMsgType;

using DoIPActivationLineChannel = asf::diag::communication::ClientChannel<DoIPActivationLine>;

DoIPActivationLine::DoIPActivationLine(const InstanceSpecifier& specifier)
    : specifier_(specifier)
    , channel_path_("/tmp/ASF_Diag_DoIP_Info_Interface")
    , offer_status_(false)
{
    DoIPActivationLineChannel::GetInstance().InsertService(specifier_, this);
    DoIPActivationLineChannel::GetInstance().SetAvaliableNotifier(specifier_,
                                                                  [this](const bool avaliable)
                                                                  { ChannelAvaliable(avaliable); });
}

DoIPActivationLine::~DoIPActivationLine()
{
    try {
        DoIPActivationLineChannel::GetInstance().EraseService(specifier_);
    } catch(...) {}
}

void DoIPActivationLine::ChannelAvaliable(const bool avaliable) const
{
    if (true == avaliable) {
        if (offer_status_) {
            (void)DoIPActivationLineChannel::GetInstance().SendOffer(specifier_);
        }
    }
}

void DoIPActivationLine::HandleMessage(const uint16_t type, const std::vector<uint8_t>& data)
{
    if (!(data.size() > 0))
    {
        LOG_ERROR << __FUNCTION__ << "data size is 0. ";
        return;
    }
    LOG_INFO << __FUNCTION__ << "type: " << LOG_HEX(type);
    std::vector<uint8_t> buffer{};
    if (DoIpMsgType::kGetNetworkInterfaceId == type) 
    {
        auto outputFuture = GetNetworkInterfaceId();
        const auto network_interface_id = outputFuture.get();
        buffer.push_back(network_interface_id);
    }
    else if (DoIpMsgType::kGetActivationLineState == type) 
    {
        auto outputFuture = GetActivationLineState();
        const auto active_line_state = outputFuture.get();
        buffer.push_back(active_line_state);
    }
    else
    {
        // donothing
    }
    if (DoIPActivationLineChannel::GetInstance().SendMessage(channel_path_, specifier_, type, buffer) == -1)
    {
        LOG_ERROR << __FUNCTION__ << "SendMessage fail! ";
    }
}

bool DoIPActivationLine::UpdateActivationLineState(bool state)
{
    LOG_INFO << __FUNCTION__ << "state:" << state;
    const auto activation_line_state = static_cast<uint8_t>(state);
    std::vector<uint8_t> buffer{activation_line_state};
    if (DoIPActivationLineChannel::GetInstance().SendMessage(channel_path_, specifier_, DoIpMsgType::kUpdateActivationLineState, buffer) == -1)
    {
        LOG_ERROR << __FUNCTION__ << "SendMessage fail! ";
        return false;
    }
    return true;
}

int DoIPActivationLine::Offer()
{
    int ret{0};
    if (!offer_status_) {
        offer_status_ = true;
        ret = DoIPActivationLineChannel::GetInstance().Offer(channel_path_, specifier_,
                                                             [this](const uint16_t type, const std::vector<uint8_t>& data)
                                                             { HandleMessage(type, data); });
    }
    return ret;
}

void DoIPActivationLine::StopOffer()
{
    if (offer_status_) {
        offer_status_ = false;
        DoIPActivationLineChannel::GetInstance().StopOffer(specifier_);
    }
}

}  // namespace diag
}  // namespace midware
