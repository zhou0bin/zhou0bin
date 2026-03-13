#include "diag_authentication_mode.h"
#include "diag_libnet_client_channel.h"

#include "log.h"

namespace midware
{
namespace diag
{
using asf::diag::msg_on_neusar_net::AuthMsgType;

using DiagAuthenticationModeChannel = asf::diag::communication::ClientChannel<DiagAuthenticationMode>;

DiagAuthenticationMode::DiagAuthenticationMode()
    : channel_path_("/tmp/ASF_Diag_Authentication_Interface")
    , specifier_("Diag_Authentication_Mode_Interface")
    , offer_status_(false)
    , mutex_()
{
    DiagAuthenticationModeChannel::GetInstance().InsertService(specifier_, this);
    DiagAuthenticationModeChannel::GetInstance().SetAvaliableNotifier(specifier_,
                                                                        [this](const bool avaliable)
                                                                        { ChannelAvaliable(avaliable); });

}

DiagAuthenticationMode::~DiagAuthenticationMode()
{
    try {
        DiagAuthenticationModeChannel::GetInstance().EraseService(specifier_);
    } catch(...) {}
}

int DiagAuthenticationMode::Offer()
{
    int ret{0};
    if (!offer_status_) {
        offer_status_ = true;
        ret = DiagAuthenticationModeChannel::GetInstance().Offer(channel_path_, specifier_,
                                                                 [this](const uint16_t type, const std::vector<uint8_t> &data)
                                                                 { HandleMessage(type, data); });
    }
    return ret;
}

void DiagAuthenticationMode::StopOffer()
{
    if (offer_status_) {
        offer_status_ = false;
        DiagAuthenticationModeChannel::GetInstance().StopOffer(specifier_);
    }
}

void DiagAuthenticationMode::ChannelAvaliable(const bool avaliable) const
{
    if (true == avaliable) {
        if (offer_status_) {
            (void)DiagAuthenticationModeChannel::GetInstance().SendOffer(specifier_);
        }
    }
}

void DiagAuthenticationMode::HandleMessage(const uint16_t type, const std::vector<uint8_t>& data)
{
    if (!offer_status_) {
        LOG_ERROR << __FUNCTION__ << "no offer! ";
        return;
    }
    if (!(data.size() > 0)) {
        LOG_ERROR << __FUNCTION__ << "data size is 0. ";
        return;
    }
    LOG_INFO << __FUNCTION__ << "type: " << LOG_HEX(type) << "data: " << LOG_HEX(data.at(0));
    std::vector<uint8_t> buffer{};
    if (AuthMsgType::kGetAuthMode == type) {
        uint8_t value{0U};
        GetDiagAuthenticationMode(value);
        LOG_INFO << __FUNCTION__ << "AuthMode: " << LOG_HEX(value);
    } else {
        LOG_ERROR << __FUNCTION__ << "unknow type! ";
        return;
    }
    if (DiagAuthenticationModeChannel::GetInstance().SendMessage(channel_path_, specifier_, type, buffer) == -1) {
        LOG_ERROR << __FUNCTION__ << "SendMessage fail! ";
    }
}

bool DiagAuthenticationMode::SetDiagAuthenticationMode(const uint8_t value)
{
    if (!offer_status_) {
        LOG_ERROR << __FUNCTION__ << "no offer! ";
        return false;
    }
    LOG_INFO << __FUNCTION__ << "value: " << LOG_HEX(value);
    std::unique_lock<std::mutex> locker(mutex_);
    const std::vector<uint8_t> buffer{value};
    if (DiagAuthenticationModeChannel::GetInstance().SendMessage(channel_path_, specifier_, AuthMsgType::kUpdateAuthMode, buffer) == -1) {
        return false;
    }
    return true;
}

} // diag
} // midware
