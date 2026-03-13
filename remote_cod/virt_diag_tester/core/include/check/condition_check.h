/*****************************************************************************
* @file     vehicle_proxy.h
* @brief    vehicle proxy
* @author   kevin
* @date     2025/4/22
*****************************************************************************/

#ifndef VD_DIAG_CONDITION_CHECK_H_
#define VD_DIAG_CONDITION_CHECK_H_

#include <iostream>
#include <set>
#include <fstream>
#include <cassert>
#include <vector>
#include <string>

#include "common/common.h"
#include "common/log.h"
#include "common/json.h"

#ifdef CONDITION_CHECK
#define DIAG_ADDR_FILE "diagAddr.json"

namespace asf {
namespace vdi {

class DiagConditionCheck
{
public:
    using InterenetStateProxy = ara::com::vcpintrenetstateinterface::proxy::App_CCU_VCCSInternetStateInterface_StsProxy;
    #if 0
    using OtaModeProxy = ara::com::ps_otamodestatus::proxy::PS_OTAModeStatusProxy;
    using NmWrapperProxy = asf::nm::proxy::NmWrapperInterfaceProxy;
    using IEPowerGroupProxy = ara::com::intelligentpowergroup::proxy::IntelligentPowerGroupProxy;
    using MproxySetterServiceProxy = ara::com::mproxysetterservice::proxy::MproxySetterServiceProxy;
    #endif

    DiagConditionCheck() = default;

    ~DiagConditionCheck() = default;

    void Init();

    bool IsInternetState();
    #if 0
    bool IsOtaMode();

    bool SetIEPowerState(uint16_t Duration, bool active);

    bool GetIEPowerState();

    bool SetVTState(uint8_t VTst); //0x00=VirtualTester idle, 0x01=VirtualTester activaion

    void NetworkRequestCall(asf::nm::vsomeip::AppID app_id,asf::nm::vsomeip::PncType pnv_type,
                            asf::nm::vsomeip::NmStatusType nm_status_type);
    
    // kevin 获取OTA状态
    std::shared_ptr<OtaModeProxy> ota_mode_proxy;
    std::shared_ptr<NmWrapperProxy> nmwrapper_proxy;
    std::shared_ptr<IEPowerGroupProxy> iepowergroup_proxy;
    std::shared_ptr<MproxySetterServiceProxy> mproxysetterservice_proxy;
    #endif
    std::shared_ptr<InterenetStateProxy> interenet_state_proxy;

private:
    void VCPInternetStateInterfaceAvailabilityCallback(ara::com::ServiceHandleContainer<InterenetStateProxy::HandleType> handles,
                                                       ara::com::FindServiceHandle findServiceHandle);

    #if 0
    void PS_OTAModeStatusAvailabilityCallback(ara::com::ServiceHandleContainer<OtaModeProxy::HandleType> handles,
                                              ara::com::FindServiceHandle findServiceHandle);

    void NmWrapperInterfaceProxyAvailabilityCallback(ara::com::ServiceHandleContainer<NmWrapperProxy::HandleType> handles,
                                                     ara::com::FindServiceHandle findServiceHandle);

    void IEPowerGroupProxyAvailabilityCallback(ara::com::ServiceHandleContainer<IEPowerGroupProxy::HandleType> handles,
                                               ara::com::FindServiceHandle findServiceHandle);

    void MproxySetterServiceProxyAvailabilityCallback(ara::com::ServiceHandleContainer<MproxySetterServiceProxy::HandleType> handles,
                                                      ara::com::FindServiceHandle findServiceHandle);

    void GetDefaultVehicleAddr();

    int GetVehicleAddr();

    bool ReadJsonFile(std::string& jsonStr);

    std::vector<uint16_t> diag_addr_vector;
    #endif
};

} // namespace vdi
} // namespace asf

#endif

#endif // VD_DIAG_CONDITION_CHECK_H_
