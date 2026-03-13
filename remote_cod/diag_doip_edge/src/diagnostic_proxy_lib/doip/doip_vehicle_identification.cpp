#include "doip_vehicle_identification.h"
#include "diag_libnet_client_channel.h"

#include "log.h"

namespace midware
{
namespace diag
{
using asf::diag::msg_on_neusar_net::DoIpMsgType;

using DoIPVehicleIdentificationChannel = asf::diag::communication::ClientChannel<DoIPVehicleIdentification>;

DoIPVehicleIdentification::DoIPVehicleIdentification(const InstanceSpecifier& specifier)
    : specifier_(specifier)
    , channel_path_("/tmp/ASF_Diag_DoIP_Info_Interface")
    , offer_status_(false)
{
    DoIPVehicleIdentificationChannel::GetInstance().InsertService(specifier_, this);
    DoIPVehicleIdentificationChannel::GetInstance().SetAvaliableNotifier(specifier_,
                                                                         [this](const bool avaliable)
                                                                         { ChannelAvaliable(avaliable); });
}

DoIPVehicleIdentification::~DoIPVehicleIdentification()
{
    try {
        DoIPVehicleIdentificationChannel::GetInstance().EraseService(specifier_);
    } catch(...) {}
}

void DoIPVehicleIdentification::ChannelAvaliable(const bool avaliable) const
{
    if (true == avaliable) {
        if (offer_status_) {
            (void)DoIPVehicleIdentificationChannel::GetInstance().SendOffer(specifier_);
        }
    }
}

void DoIPVehicleIdentification::HandleMessage(const uint16_t type, const std::vector<uint8_t>& data)
{
    if (!(data.size() > 0))
    {
        LOG_ERROR << __FUNCTION__ << "data size is 0. ";
        return;
    }
    LOG_INFO << __FUNCTION__ << "type: " << LOG_HEX(type);
    VinNumber vin;
    if (DoIpMsgType::kGetVin == type)
    {
        auto outputFuture = GetVinNumber();
        vin = outputFuture.get();
    }
    std::vector<uint8_t> buffer{};
    (void)buffer.insert(buffer.cbegin(), vin.VIN.begin(), vin.VIN.end());
    if (DoIPVehicleIdentificationChannel::GetInstance().SendMessage(channel_path_, specifier_, type, buffer) == -1)
    {
        LOG_ERROR << __FUNCTION__ << "SendMessage fail! ";
    }
}

int DoIPVehicleIdentification::Offer()
{
    int ret{0};
    if (!offer_status_) {
        offer_status_ = true;

        ret = DoIPVehicleIdentificationChannel::GetInstance().Offer(channel_path_, specifier_,
                                                                    [this](const uint16_t type, const std::vector<uint8_t>& data)
                                                                    { HandleMessage(type, data); });
    }
    return ret;
}

void DoIPVehicleIdentification::StopOffer()
{
    if (offer_status_) {
        offer_status_ = false;
        DoIPVehicleIdentificationChannel::GetInstance().StopOffer(specifier_);
    }
}

}  // namespace diag
}  // namespace midware
