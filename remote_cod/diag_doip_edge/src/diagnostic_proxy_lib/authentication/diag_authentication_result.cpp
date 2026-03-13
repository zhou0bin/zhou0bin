#include "diag_authentication_result.h"
#include "diag_libnet_client_channel.h"

#include "log.h"

namespace midware
{
namespace diag
{
using asf::diag::msg_on_neusar_net::AuthMsgType;

using DiagAuthenticationResultChannel = asf::diag::communication::ClientChannel<DiagAuthenticationResult>;

DiagAuthenticationResult::DiagAuthenticationResult()
    : channel_path_("/tmp/ASF_Diag_Authentication_Interface")
    , specifier_("Diag_Authentication_Result_Interface")
    , offer_status_(false)
    , mutex_()
{
    DiagAuthenticationResultChannel::GetInstance().InsertService(specifier_, this);
    DiagAuthenticationResultChannel::GetInstance().SetAvaliableNotifier(specifier_,
                                                                        [this](const bool avaliable)
                                                                        { ChannelAvaliable(avaliable); });

}

DiagAuthenticationResult::~DiagAuthenticationResult()
{
    try {
        DiagAuthenticationResultChannel::GetInstance().EraseService(specifier_);
    } catch(...) {}
}

int DiagAuthenticationResult::Offer()
{
    int ret{0};
    if (!offer_status_) {
        offer_status_ = true;
        ret = DiagAuthenticationResultChannel::GetInstance().Offer(channel_path_, specifier_,
                                                                   [this](const uint16_t type, const std::vector<uint8_t> &data)
                                                                   { HandleMessage(type, data); });
    }
    return ret;
}

void DiagAuthenticationResult::StopOffer()
{
    if (offer_status_) {
        offer_status_ = false;
        DiagAuthenticationResultChannel::GetInstance().StopOffer(specifier_);
    }
}

void DiagAuthenticationResult::ChannelAvaliable(const bool avaliable) const
{
    if (true == avaliable) {
        if (offer_status_) {
            (void)DiagAuthenticationResultChannel::GetInstance().SendOffer(specifier_);
        }
    }
}

void DiagAuthenticationResult::HandleMessage(const uint16_t type, const std::vector<uint8_t>& data)
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
    if (AuthMsgType::kGetAuthResult == type) {
        uint8_t value{0U};
        GetDiagAuthenticationResult(value);
        LOG_INFO << __FUNCTION__ << "AuthResult: " << LOG_HEX(value);
    } else {
        LOG_ERROR << __FUNCTION__ << "unknow type! ";
        return;
    }
    if (DiagAuthenticationResultChannel::GetInstance().SendMessage(channel_path_, specifier_, type, buffer) == -1) {
        LOG_ERROR << __FUNCTION__ << "SendMessage fail! ";
    }
}

bool DiagAuthenticationResult::SetDiagAuthenticationResult(const uint8_t value)
{
    if (!offer_status_) {
        LOG_ERROR << __FUNCTION__ << "no offer! ";
        return false;
    }
    LOG_INFO << __FUNCTION__ << "value: " << LOG_HEX(value);
    std::unique_lock<std::mutex> locker(mutex_);
    const std::vector<uint8_t> buffer{value};
    if (DiagAuthenticationResultChannel::GetInstance().SendMessage(channel_path_, specifier_, AuthMsgType::kUpdateAuthResult, buffer) == -1) {
        return false;
    }
    return true;
}

} // diag
} // midware
