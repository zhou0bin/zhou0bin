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

#include "dcm/connection/do_ip_udp_info.h"

#include "log.h"

using asf::diag::msg_on_neusar_net::DoIpMsgType;

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

DoIpUdpInfo::DoIpUdpInfo(const std::string& config)
    : doip_skeleton_(std::make_shared<communication::Server>())
    , doip_client_map_()
    , doip_client_map_mutex_()
    , doip_path_com_("/tmp/ASF_Diag_DoIP_Info_Interface")
    , doip_active_line_specifier_("Diag_Doip_activation_line_Interface")
    , doip_vin_specifier_("Diag_Doip_vehicle_identification_Interface")
    , doip_gid_specifier_("Diag_Doip_group_identification_Interface")
    , doip_power_mode_specifier_("Diag_Doip_power_mode_Interface")
    , activation_line_dependent_(false)
    , activation_line_state_(false)
    , activeLinePromise_()
    , network_interface_id_(false)
    , networkInterfaceIdPromise_()
    , vin_()
    , vinPromise_()
    , gid_status_()
    , gidPromise_()
    , power_mode_()
    , powerModePromise_()
    , activeLineTimeout_(doip::kActiveLineTimeout)
    , activeLineTimer_(nullptr)
    , networkInterfaceIdTimeout_(doip::kNetworkInterfaceIdTimeout)
    , networkInterfaceIdTimer_(nullptr)
    , vinTimeout_(2)
    , vinTimer_(nullptr)
    , gidTimeout_(2)
    , gidTimer_(nullptr)
    , powerModeTimeout_(2)
    , powerModeTimer_(nullptr)
    , udp_channel(nullptr)
    , udp_executor_(1)
    , config_(config)
    , exit_requested_(false)
    , mutex_()
    , condition_variable_()
    , thread_()
    , VehicleAnnounceCallback_(nullptr)
{
#ifdef USE_FS_LIBNET
    udp_channel = std::make_shared<wheels::TimerChannel>(udp_executor_.GetEventBase());
#else
    udp_channel = std::make_shared<wheels::TimerChannel>(udp_executor_.getEventBase());
#endif

    activeLineTimer_ = std::make_shared<common::WheelsTimer>(udp_channel, [this](){ this->GetDoIPActivationLineTimeout(); });

    networkInterfaceIdTimer_ = std::make_shared<common::WheelsTimer>(udp_channel, [this](){ this->GetDoIPNetworkInterfaceIdTimeout(); });

    vinTimer_ = std::make_shared<common::WheelsTimer>(udp_channel, [this](){ this->GetDoIPVINTimeout(); });

    gidTimer_ = std::make_shared<common::WheelsTimer>(udp_channel, [this](){ this->GetDoIPGIDTimeout(); });

    powerModeTimer_ = std::make_shared<common::WheelsTimer>(udp_channel, [this](){ this->GetDoIPPowerModeTimeout(); });

}

void DoIpUdpInfo::Initialize()
{
    std::ifstream ifs(config_);
    if (ifs.good())
    {
        rapidjson::IStreamWrapper ist(ifs);
        rapidjson::Document doc;
        (void)doc.ParseStream(ist);
        const rapidjson::Value& doip = doc["DoIp"];
        if (doip.HasMember("IsActivationLineDependent") && doip["IsActivationLineDependent"].IsBool())
        {
            activation_line_dependent_ = doip["IsActivationLineDependent"].GetBool();
        }
    }

    thread_ = std::thread([this]() { TriggerVehicleAnnouncement(); });

}

void DoIpUdpInfo::Start()
{
    doip_skeleton_->RegisterHandleAvaliableCallback([this](const uint64_t client, const bool avaliable)
                                                    { ClientAvaliable(client, avaliable); });
    doip_skeleton_->RegisterHandleMessageCallback([this](const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t> &data)
                                                  { ReceiveProxyMessage(client_handle, type, data); });
    doip_skeleton_->OfferService(doip_path_com_);

    /*
        try {
            auto future_tmp = GetDoIPActivationLine();
            activation_line_state_ = future_tmp.get();
        }
        catch(std::runtime_error& e) {
            activation_line_state_ = false;
            LOG_ERROR << __FUNCTION__ << "runtime_error: " << e.what();
        }
    */

}

void DoIpUdpInfo::Stop()
{
    LOG_INFO << __FUNCTION__ << "in! ";
    try {
        doip_skeleton_->Stop();
    } catch(...) { }
    if (activeLineTimer_ != nullptr)
    {
        activeLineTimer_ = nullptr;
    }
    if (networkInterfaceIdTimer_ != nullptr)
    {
        networkInterfaceIdTimer_ = nullptr;
    }
    if (vinTimer_ != nullptr)
    {
        vinTimer_ = nullptr;
    }
    if (gidTimer_ != nullptr)
    {
        gidTimer_ = nullptr;
    }
    if (powerModeTimer_ != nullptr)
    {
        powerModeTimer_ = nullptr;
    }
    if (udp_channel != nullptr)
    {
        udp_channel = nullptr;
    }
    {
        const std::unique_lock<std::mutex> locker(mutex_);
        exit_requested_.store(true, std::memory_order_relaxed);
    }
    condition_variable_.notify_all();
    if (thread_.joinable())
    {
        thread_.join();
    }
}

void DoIpUdpInfo::ClientAvaliable(const uint64_t client, const bool avaliable)
{
    const std::lock_guard<std::mutex> locker_client{doip_client_map_mutex_};
    if (avaliable == false)
    {
        for(auto it = doip_client_map_.begin(); it != doip_client_map_.end(); )
        {
            if (it->second == client)
            {
                it = doip_client_map_.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

uint64_t DoIpUdpInfo::GetClientBySpecifier(const std::string& specifier)
{
    uint64_t client{0};
    const std::lock_guard<std::mutex> locker_client{doip_client_map_mutex_};
    if (doip_client_map_.find(specifier) != doip_client_map_.end())
    {
        client = doip_client_map_[specifier];
    }
    return client;
}

void DoIpUdpInfo::ReceiveProxyMessage(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& data)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (!(data.size() > 0))
    {
        LOG_ERROR << __FUNCTION__ << "data size is 0. ";
        return;
    }
    if (type == CommMsgType::kOfferMessage)
    {
        std::string specifier{};
        (void)specifier.assign(data.begin(), data.end());
        LOG_DEBUG << __FUNCTION__ << "Offer specifier: " << specifier << " client_handle: " << client_handle;
        const std::lock_guard<std::mutex> locker_offermsg{doip_client_map_mutex_};
        if (doip_client_map_[specifier] != client_handle)
        {
            doip_client_map_[specifier] = client_handle;
        }
    }
    else if (type == CommMsgType::kStopOfferMessage)
    {
        std::string specifier{};
        (void)specifier.assign(data.begin(), data.end());
        const std::lock_guard<std::mutex> locker_stopoffermsg{doip_client_map_mutex_};
        if (doip_client_map_.find(specifier) != doip_client_map_.end())
        {
            (void)doip_client_map_.erase(specifier);
            LOG_DEBUG << __FUNCTION__ << "StopOffer specifier: " << specifier << " client_handle: " << client_handle;
        }
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "doip info type: " << LOG_HEX(type);
        switch (type)
        {
            case DoIpMsgType::kUpdateActivationLineState:
            {
                LOG_DEBUG << __FUNCTION__ << "kUpdateActivationLineState. ";
                const bool activation_line_state = static_cast<bool>(data.at(0));
                if ((activation_line_state == true)
                 && (activation_line_state_ == false)
                 && (activation_line_dependent_ == true))
                {
                    condition_variable_.notify_one();
                }
                activation_line_state_ = activation_line_state;
                LOG_DEBUG << __FUNCTION__ << "activation_line_state: " << activation_line_state_;
                break;
            }
            case DoIpMsgType::kGetActivationLineState:
            {
                LOG_DEBUG << __FUNCTION__ << "kGetActivationLineState. ";
                const bool activation_line_state = static_cast<bool>(data.at(0));
                if (activeLineTimer_->IsRunning())
                {
                    activeLineTimer_->Stop();
                    activation_line_state_ = activation_line_state;
                    try {
                        activeLinePromise_.set_value(true);
                    } catch (...) {}
                    LOG_DEBUG << __FUNCTION__ << "activation_line_state: " << activation_line_state;
                }
                break;
            }
            case DoIpMsgType::kGetNetworkInterfaceId:
            {
                LOG_DEBUG << __FUNCTION__ << "kGetNetworkInterfaceId. ";
                const uint8_t netwrok_interface_id = data.at(0);
                if (networkInterfaceIdTimer_->IsRunning())
                {
                    networkInterfaceIdTimer_->Stop();
                    network_interface_id_ = netwrok_interface_id;
                    try {
                        networkInterfaceIdPromise_.set_value(true);
                    } catch (...) {}
                    LOG_DEBUG << __FUNCTION__ << "netwrok_interface_id: " << netwrok_interface_id;
                }
                break;
            }
            case DoIpMsgType::kGetVin:
            {
                LOG_DEBUG << __FUNCTION__ << "kGetVin. ";
                const std::vector<uint8_t> vin(data);
                if (vinTimer_->IsRunning())
                {
                    vinTimer_->Stop();
                    vin_ = vin;
                    try {
                        vinPromise_.set_value(true);
                    } catch (...) {}
                    LOG_DEBUG << __FUNCTION__ << "vin: " << LOG_RAW_BUFFER(vin_);
                }
                break;
            }
            case DoIpMsgType::kGetGidStatus:
            {
                LOG_DEBUG << __FUNCTION__ << "kGetGidStatus. ";
                if (data.size() != 8)
                {
                    LOG_ERROR << __FUNCTION__ << "gid status size error! size: " << data.size();
                    return;
                }
                const std::vector<uint8_t> gid{data.begin(), data.begin() + 6};
                const uint8_t further_action_req = data.at(6);
                const uint8_t sync_status = data.at(7);
                if (gidTimer_->IsRunning())
                {
                    gidTimer_->Stop();
                    gid_status_.GID = gid;
                    gid_status_.furtherActionReq = further_action_req;
                    gid_status_.syncStatus = sync_status;
                    try {
                        gidPromise_.set_value(true);
                    } catch (...) {}
                    LOG_DEBUG << __FUNCTION__ << "GID: " << LOG_RAW_BUFFER(gid_status_.GID);
                    LOG_DEBUG << __FUNCTION__ << "furtherActionReq: " << LOG_HEX(gid_status_.furtherActionReq) << "syncStatus: " << LOG_HEX(gid_status_.syncStatus);
                }
                break;
            }
            case DoIpMsgType::kGetDoIPPowerMode:
            {
                LOG_DEBUG << __FUNCTION__ << "kGetDoIPPowerMode. ";
                const uint8_t power_mode = data.at(0);
                if (powerModeTimer_->IsRunning())
                {
                    powerModeTimer_->Stop();
                    power_mode_ = static_cast<PowerModeType>(power_mode);
                    try {
                        powerModePromise_.set_value(true);
                    } catch (...) {}
                    LOG_DEBUG << __FUNCTION__ << "PowerMode: " << static_cast<uint8_t>(power_mode_);
                }
                break;
            }
            case DoIpMsgType::kTriggerVehicleAnnouncement:
            {
                LOG_DEBUG << __FUNCTION__ << "kTriggerVehicleAnnouncement";
                const uint8_t id = data.at(0);
                LOG_DEBUG << __FUNCTION__ << "trigger Id: " << id;
                bool status{false};
                if (activation_line_dependent_ == false)
                {
                    condition_variable_.notify_one();
                    status = true;
                }
                const uint8_t result = static_cast<uint8_t>(status); 
                std::vector<uint8_t> buffer{result};
                if (!doip_skeleton_->SendMessage(client_handle, DoIpMsgType::kTriggerVehicleAnnouncement, buffer))
                {
                    LOG_ERROR << __FUNCTION__ << "SendMessage failed! ";
                }
                break;
            }
            default:
            {
                LOG_VERBOSE << __FUNCTION__ << "unknow type: " << type << "client_handle: " << client_handle;
                break;
            }
        }
    }
}

bool DoIpUdpInfo::GetDoIPActivationLine(bool& activation_line)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const std::lock_guard<std::mutex> locker_gdal(doip_active_line_mutex_);
    const auto& key_client = GetClientBySpecifier(doip_active_line_specifier_);
    if (!static_cast<bool>(key_client))
    {
        LOG_ERROR << __FUNCTION__ << "GetClientBySpecifier fail! specifier: " << doip_active_line_specifier_;
        return false;
    }
    const std::vector<uint8_t> buffer{0x01};
    activeLinePromise_ = std::promise<bool>();
    auto activation_line_future = activeLinePromise_.get_future();
    if (activeLineTimer_->IsRunning())
    {
        activeLineTimer_->Stop();
    }
    (void)activeLineTimer_->StartOnce(std::chrono::seconds(activeLineTimeout_));
    (void)doip_skeleton_->SendMessage(key_client, DoIpMsgType::kGetActivationLineState, buffer);
    if (!activation_line_future.get())
    {
        LOG_ERROR << __FUNCTION__ << "Get Activation Line timeout! ";
        return false;
    }
    activation_line = activation_line_state_;
    return true;
}

bool DoIpUdpInfo::GetDoIPNetworkInterfaceId(uint8_t& network_interface_id)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const std::lock_guard<std::mutex> locker_gdnii(doip_active_line_mutex_);
    const auto& key_client = GetClientBySpecifier(doip_active_line_specifier_);
    if (!static_cast<bool>(key_client))
    {
        LOG_ERROR << __FUNCTION__ << "GetClientBySpecifier fail! specifier: " << doip_active_line_specifier_;
        return false;
    }
    const std::vector<uint8_t> buffer{0x01};
    networkInterfaceIdPromise_ = std::promise<bool>();
    auto network_interface_id_future = networkInterfaceIdPromise_.get_future();
    if (networkInterfaceIdTimer_->IsRunning())
    {
        networkInterfaceIdTimer_->Stop();
    }
    (void)networkInterfaceIdTimer_->StartOnce(std::chrono::seconds(networkInterfaceIdTimeout_));
    (void)doip_skeleton_->SendMessage(key_client, DoIpMsgType::kGetNetworkInterfaceId, buffer);
    if (!network_interface_id_future.get())
    {
        LOG_ERROR << __FUNCTION__ << "Get Network Interface Id timeout! ";
        return false;
    }
    network_interface_id = network_interface_id_;
    return true;
}

bool DoIpUdpInfo::GetDoIPVIN(ByteVector& vin)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const std::lock_guard<std::mutex> locker_vin(doip_vin_mutex_);
    const auto& key_client = GetClientBySpecifier(doip_vin_specifier_);
    if (!(static_cast<bool>(key_client)))
    {
        LOG_ERROR << __FUNCTION__ << "GetVINClientBySpecifier fail! specifier: " << doip_vin_specifier_;
        return false;
    }
    const std::vector<uint8_t> buffer{0x01};
    vinPromise_ = std::promise<bool>();
    auto vin_future = vinPromise_.get_future();
    if (vinTimer_->IsRunning())
    {
        vinTimer_->Stop();
    }
    (void)vinTimer_->StartOnce(std::chrono::seconds(vinTimeout_));
    (void)doip_skeleton_->SendMessage(key_client, DoIpMsgType::kGetVin, buffer);
    if (!vin_future.get())
    {
        LOG_ERROR << __FUNCTION__ << "Get VIN timeout! ";
        return false;
    }
    vin = vin_;
    return true;
}

bool DoIpUdpInfo::GetDoIPGID(GidStatus& gid)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const std::lock_guard<std::mutex> locker_gid(doip_gid_mutex_);
    const auto& key_client = GetClientBySpecifier(doip_gid_specifier_);
    if (!(static_cast<bool>(key_client)))
    {
        LOG_ERROR << __FUNCTION__ << "GetGIDClientBySpecifier fail! specifier: " << doip_gid_specifier_;
        return false;
    }
    const std::vector<uint8_t> buffer{0x01};
    gidPromise_ = std::promise<bool>();
    auto gid_future = gidPromise_.get_future();
    if (gidTimer_->IsRunning())
    {
        gidTimer_->Stop();
    }
    (void)gidTimer_->StartOnce(std::chrono::seconds(gidTimeout_));
    (void)doip_skeleton_->SendMessage(key_client, DoIpMsgType::kGetGidStatus, buffer);
    if (!gid_future.get())
    {
        LOG_ERROR << __FUNCTION__ << "GetDoIPGID timeout! ";
        return false;
    }
    gid = gid_status_;
    return true;
}

bool DoIpUdpInfo::GetDoIPPowerMode(PowerModeType& power_mode)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const std::lock_guard<std::mutex> locker_pm(doip_power_mode_mutex_);
    const auto& key_client = GetClientBySpecifier(doip_power_mode_specifier_);
    if (!static_cast<bool>(key_client))
    {
        LOG_ERROR << __FUNCTION__ << "GetPowerModeClientBySpecifier fail! specifier: " << doip_power_mode_specifier_;
        return false;
    }
    const std::vector<uint8_t> buffer{0x01};
    powerModePromise_ = std::promise<bool>();
    auto power_mode_future = powerModePromise_.get_future();
    if (powerModeTimer_->IsRunning())
    {
        powerModeTimer_->Stop();
    }
    (void)powerModeTimer_->StartOnce(std::chrono::seconds(powerModeTimeout_));
    (void)doip_skeleton_->SendMessage(key_client, DoIpMsgType::kGetDoIPPowerMode, buffer);
    if (!power_mode_future.get())
    {
        LOG_ERROR << __FUNCTION__ << "GetDoIPPowerMode timeout! ";
        return false;
    }
    power_mode = power_mode_;
    return true;
}

void DoIpUdpInfo::GetDoIPActivationLineTimeout()
{
    try {
        activeLinePromise_.set_value(false);
    } catch (...) {}
}

void DoIpUdpInfo::GetDoIPNetworkInterfaceIdTimeout()
{
    try {
        networkInterfaceIdPromise_.set_value(0);
    } catch (...) {}
}

void DoIpUdpInfo::GetDoIPVINTimeout()
{
    try {
        vinPromise_.set_value(false);
    } catch (...) {}
}

void DoIpUdpInfo::GetDoIPGIDTimeout()
{
    try {
        gidPromise_.set_value(false);
    } catch (...) {}
}

void DoIpUdpInfo::GetDoIPPowerModeTimeout()
{
    try {
        powerModePromise_.set_value(false);
    } catch (...) {}
}

void DoIpUdpInfo::VehicleAnnouncement()
{
    if (VehicleAnnounceCallback_ != nullptr)
    {
        VehicleAnnounceCallback_();
    }
}

void DoIpUdpInfo::TriggerVehicleAnnouncement()
{
    std::unique_lock<std::mutex> locker(mutex_);
    while (!exit_requested_.load(std::memory_order_relaxed))
    {
        condition_variable_.wait(locker);
        if (!exit_requested_.load(std::memory_order_relaxed))
        {
            VehicleAnnouncement();
        }
    }
}

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
