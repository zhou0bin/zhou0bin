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

#ifndef SOURCE_DCM_CONNECTION_DO_IP_UDP_INFO_H_
#define SOURCE_DCM_CONNECTION_DO_IP_UDP_INFO_H_

#include "dcm/connection/do_ip_constants.h"

#include "common/wheels_timer.h"

#include "diag_proxy_common.h"

#include "diag_libnet_common.h"

#include "communication/diag_libnet_server.h"

// #include "ara/core/future.h"
// #include "ara/core/promise.h"
// #include "ara/core/map.h"
#include <future>
#include <map>

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

class DoIpUdpInfo
{
public:
    using VehicleAnnounceCallback = std::function<void ()>;
public:
    ///
    /// @brief Constructor for a new DoIpUdpInfo.
    ///
    /// @param[in] config The config path.
    ///
    explicit DoIpUdpInfo(const std::string& config);
    explicit DoIpUdpInfo(DoIpUdpInfo&&) = delete;
    DoIpUdpInfo(DoIpUdpInfo&) = delete;
    DoIpUdpInfo& operator=(DoIpUdpInfo&) = delete;
    DoIpUdpInfo& operator=(DoIpUdpInfo&&) = delete;
    virtual ~DoIpUdpInfo() = default;

    virtual void Initialize();
    virtual void Start();
    virtual void Stop();

    void RegisterVehicleAnnounceCallback(const VehicleAnnounceCallback Callback)
    {
        VehicleAnnounceCallback_ = Callback;
    }

    bool GetDoIPActivationLine(bool& activation_line);
    bool GetDoIPNetworkInterfaceId(uint8_t& network_interface_id);
    bool GetDoIPVIN(ByteVector& vin);
    bool GetDoIPGID(GidStatus& gid);
    bool GetDoIPPowerMode(PowerModeType &power_mode);

private:
    void ReceiveProxyMessage(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& data);
    void ClientAvaliable(const uint64_t client, const bool avaliable);
    uint64_t GetClientBySpecifier(const std::string& specifier);
    void GetDoIPActivationLineTimeout();
    void GetDoIPNetworkInterfaceIdTimeout();
    void GetDoIPVINTimeout();
    void GetDoIPGIDTimeout();
    void GetDoIPPowerModeTimeout();

    void VehicleAnnouncement();
    void TriggerVehicleAnnouncement();

private:
    std::shared_ptr<communication::Server> doip_skeleton_;
    std::map<std::string , uint64_t> doip_client_map_;
    std::mutex doip_client_map_mutex_;
    const std::string doip_path_com_;
    const std::string doip_active_line_specifier_;
    const std::string doip_vin_specifier_;
    const std::string doip_gid_specifier_;
    const std::string doip_power_mode_specifier_;

    std::mutex doip_active_line_mutex_;
    std::mutex doip_vin_mutex_;
    std::mutex doip_gid_mutex_;
    std::mutex doip_power_mode_mutex_;

    bool activation_line_dependent_;
    bool activation_line_state_;
    std::promise<bool> activeLinePromise_;

    uint8_t network_interface_id_;
    std::promise<bool> networkInterfaceIdPromise_;

    std::vector<uint8_t> vin_;
    std::promise<bool> vinPromise_;

    GidStatus gid_status_;
    std::promise<bool> gidPromise_;

    PowerModeType power_mode_;
    std::promise<bool> powerModePromise_;

    int activeLineTimeout_;
    std::shared_ptr<common::WheelsTimer> activeLineTimer_;

    int networkInterfaceIdTimeout_;
    std::shared_ptr<common::WheelsTimer> networkInterfaceIdTimer_;

    int vinTimeout_;
    std::shared_ptr<common::WheelsTimer> vinTimer_;

    int gidTimeout_;
    std::shared_ptr<common::WheelsTimer> gidTimer_;

    int powerModeTimeout_;
    std::shared_ptr<common::WheelsTimer> powerModeTimer_;

    std::shared_ptr<wheels::TimerChannel> udp_channel;

    wheels::IOThreadPoolExecutor udp_executor_;

    std::string config_;
    std::atomic_bool exit_requested_;
    std::mutex mutex_;
    std::condition_variable condition_variable_;
    std::thread thread_;

    VehicleAnnounceCallback VehicleAnnounceCallback_;

};

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
#endif  // SOURCE_DCM_CONNECTION_DO_IP_UDP_INFO_H_
