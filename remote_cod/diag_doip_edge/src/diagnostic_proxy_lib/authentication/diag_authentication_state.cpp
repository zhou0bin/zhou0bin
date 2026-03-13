#include "diag_authentication_state.h"
#include "diag_libnet_client_channel.h"

#include "log.h"

namespace midware
{
namespace diag
{
using asf::diag::msg_on_neusar_net::AuthMsgType;

using DiagAuthenticationStateChannel = asf::diag::communication::ClientChannel<DiagAuthenticationState>;

DiagAuthenticationState::DiagAuthenticationState()
    : channel_path_("/tmp/ASF_Diag_Authentication_Interface")
    , specifier_("Diag_Authentication_Status_Interface")
    , offer_status_(false)
{
    DiagAuthenticationStateChannel::GetInstance().InsertService(specifier_, this);
    DiagAuthenticationStateChannel::GetInstance().SetAvaliableNotifier(specifier_,
                                                                       [this](const bool avaliable)
                                                                       { ChannelAvaliable(avaliable); });
}

DiagAuthenticationState::~DiagAuthenticationState()
{
    try {
        DiagAuthenticationStateChannel::GetInstance().EraseService(specifier_);
    } catch(...) {}
}

int DiagAuthenticationState::Offer()
{
    int ret{0};
    if (!offer_status_) {
        offer_status_ = true;
        ret = DiagAuthenticationStateChannel::GetInstance().Offer(channel_path_, specifier_,
                                                                  [this](const uint16_t type, const std::vector<uint8_t> &data)
                                                                  { HandleMessage(type, data); });
    }
    return ret;
}

void DiagAuthenticationState::StopOffer()
{
    if (offer_status_) {
        offer_status_ = false;
        DiagAuthenticationStateChannel::GetInstance().StopOffer(specifier_);
    }
}

void DiagAuthenticationState::ChannelAvaliable(const bool avaliable) const
{
    if (true == avaliable) {
        if (offer_status_) {
            (void)DiagAuthenticationStateChannel::GetInstance().SendOffer(specifier_);
        }
    }
}

void DiagAuthenticationState::HandleMessage(const uint16_t type, const std::vector<uint8_t>& data)
{
    if (!offer_status_) {
        LOG_ERROR << __FUNCTION__ << "no offer! ";
        return;
    }
    if (!(data.size() > 0)) {
        LOG_ERROR << __FUNCTION__ << "data size is 0. ";
        return;
    }
    LOG_INFO << __FUNCTION__ << "AuthMsg type: " << LOG_HEX(type);
    if (AuthMsgType::kUpdateAuthStatus == type) {
        LOG_INFO << __FUNCTION__ << "AuthMsg data: " << LOG_HEX(data.at(0));
        DiagAuthenticationStateChanged(data.at(0));
    } else {
        LOG_ERROR << __FUNCTION__ << "unknow type! ";
    }
}

} // diag
} // midware
