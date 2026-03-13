
/*****************************************************************************
* @file     vehicle_config_manager_proxy.cpp
* @brief    Vehicle Config Manager Proxy 
* @date     2025/4/22
*****************************************************************************/

#include "proxy/vehicle_config_manager_proxy.h"

#ifdef VEHICLE_CONFIG
namespace asf {
namespace vdi {

void VehicleConfigManagerProxy::Init()
{
    vin_server_socket_.Start();
    DomainSocket::Server::MessageCallback messageCallback = [&](std::string_view msg){
        LOGINFO << "VehicleConfigManagerProxy receive vin data: " << msg.data();
        vin_ = msg;
        vcm_proxy_valid_.store(true);
    };
    vin_server_socket_.SetMessageCallback(messageCallback);
}

} // namespace vdi
} // namespace asf
#endif
