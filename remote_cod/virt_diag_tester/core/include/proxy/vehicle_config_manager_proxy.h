/*****************************************************************************
* @file     vehicle_config_manager_proxy.h
* @brief    vehicle config manager proxy
* @author   kevin
* @date     2025/4/22
*****************************************************************************/
#ifndef VD_VCM_PROXY_H_
#define VD_VCM_PROXY_H_

#include "common/common.h"
#include "domain_socket/server.h"
#include "common/log.h"

#ifdef VEHICLE_CONFIG
namespace asf {
namespace vdi {

class VehicleConfigManagerProxy
{
public:
    VehicleConfigManagerProxy()
    : vcm_proxy_valid_(false)
    {}

    ~VehicleConfigManagerProxy() {
        vin_server_socket_.Stop();
    }
    
    void Init();

    bool Valid() { return vcm_proxy_valid_.load(); }

    std::string GetVin() { return vin_; }
private:
    std::atomic<bool> vcm_proxy_valid_;
    std::string vin_server_sock_{"/tmp/VIN.sock"};
    DomainSocket::Server vin_server_socket_{vin_server_sock_};
    std::string vin_ = "";
};

} // namespace vdi
} // namespace asf
#endif

#endif // VD_VCM_PROXY_H_
