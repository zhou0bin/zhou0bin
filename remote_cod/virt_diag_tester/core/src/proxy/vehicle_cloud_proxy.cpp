/*****************************************************************************
* @file     vehicle_cloud_proxy.cpp
* @brief    Vehicle Cloud Proxy 
* @date     2025/4/22
*****************************************************************************/

#include "proxy/vehicle_cloud_proxy.h"
#include <thread>

namespace asf {
namespace vdi {

void VehicleCloudProxy::Init()
{
    while(vcp_client_socket_.Connect() == false){
        LOGERROR << "VehicleCloudProxy waiting for connect to vcp server...";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    LOGINFO << "VehicleCloudProxy connect to vcp server success!";
}

void VehicleCloudProxy::ReportDiagnosisData(const std::string& data, const bool reconnect)
{
    //消息处理是同步的，每个客户端连接处理完消息后立即关闭，所以每次发消息之前需要重连。
    if(reconnect){
        bool reconnect_ret = vcp_client_socket_.Connect();
        if(!reconnect_ret){
            LOGERROR << "reconnect to vcp server failed!";
        }
        LOGINFO << "reconnect to vcp server success!";
    }

    std::unique_lock<std::mutex> send_lock(send_mutex_);
    bool send_result = vcp_client_socket_.SendMsg(data);
    if (send_result) {
        LOGINFO << "VehicleCloudProxy send diag data success!";
    } else {
        LOGERROR << "VehicleCloudProxy send diag data failed!";
    }
}

} // namespace vdi
} // namespace asf
