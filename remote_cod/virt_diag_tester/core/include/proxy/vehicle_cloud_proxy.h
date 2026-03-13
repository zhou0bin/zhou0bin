/*****************************************************************************
* @file     vehicle_proxy.h
* @brief    vehicle proxy
* @author   kevin
* @date     2025/4/22
*****************************************************************************/
#ifndef VD_VC_PROXY_H_
#define VD_VC_PROXY_H_

#include "domain_socket/client.h"
#include "common/log.h"

namespace asf {
namespace vdi {

class VehicleCloudProxy
{
public:
    VehicleCloudProxy()
    {}

    ~VehicleCloudProxy() = default;
    
    void Init();

    void ReportDiagnosisData(const std::string& data, const bool reconnect);
private:
    std::string vcp_client_sock_{"/tmp/vtest_reportdata.sock"};
    DomainSocket::Client vcp_client_socket_{vcp_client_sock_};
    std::mutex send_mutex_;
};

} // namespace vdi
} // namespace asf

#endif // VD_VC_PROXY_H_
