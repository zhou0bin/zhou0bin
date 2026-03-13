/*****************************************************************************
* @file     condition_check.cpp
* @brief    Condition Check
* @date     2025/4/22
*****************************************************************************/

#include "check/condition_check.h"
#include "global/global.h"

#ifdef CONDITION_CHECK
namespace asf {
namespace vdi {

//InteractState state;
//StopTaskState stop_task_state;

void DiagConditionCheck::VCPInternetStateInterfaceAvailabilityCallback(ara::com::ServiceHandleContainer<InterenetStateProxy::HandleType> handles,
                                                                       ara::com::FindServiceHandle findServiceHandle)
{
    for (auto& it : handles) {
        LOGINFO << "DiagConditionCheck::VCPInternetStateInterfaceAvailabilityCallback: InstanceId " << static_cast<uint16_t>(it.GetInstanceId().getInstanceId());
    }
    if (handles.size() > 0) {
        if (interenet_state_proxy == nullptr) {
            interenet_state_proxy = std::make_shared<InterenetStateProxy>(handles[0]);
            LOGINFO << "DiagConditionCheck::VCPInternetStateInterfaceAvailabilityCallback: create handle with instance: " << static_cast<uint16_t>(interenet_state_proxy->GetHandle().GetInstanceId().getInstanceId());
        }
    }
    else {
        LOGERROR << "DiagConditionCheck::VCPInternetStateInterfaceAvailabilityCallback: interenet state proxy is null.";
        if(interenet_state_proxy != nullptr) {
            interenet_state_proxy->PrivateMQTTStatus.Unsubscribe();
            interenet_state_proxy->PrivateMQTTStatus.UnsetReceiveHandler();
        }
        interenet_state_proxy = nullptr;
    }
}

#if 0
void DiagConditionCheck::NmWrapperInterfaceProxyAvailabilityCallback(ara::com::ServiceHandleContainer<NmWrapperProxy::HandleType> handles,
                                                                     ara::com::FindServiceHandle findServiceHandle)
{
    for (auto& it : handles) {
        LOGINFO << "DiagConditionCheck::NmWrapperInterfaceProxyAvailabilityCallback: InstanceId " << static_cast<uint16_t>(it.GetInstanceId());
    }
    if (handles.size() > 0) {
        if (nmwrapper_proxy == nullptr) {
            nmwrapper_proxy = std::make_shared<NmWrapperProxy>(handles[0]);
            LOGINFO << "DiagConditionCheck::NmWrapperInterfaceProxyAvailabilityCallback: create handle with instance: "
                    << static_cast<uint16_t>(nmwrapper_proxy->GetHandle().GetInstanceId());
        }
    }
    else {
        LOGERROR << "DiagConditionCheck::NmWrapperInterfaceProxyAvailabilityCallback: NmWrapperProxy proxy is null.";
        nmwrapper_proxy = nullptr;
    }
}

void DiagConditionCheck::PS_OTAModeStatusAvailabilityCallback(ara::com::ServiceHandleContainer<OtaModeProxy::HandleType> handles,
                                                              ara::com::FindServiceHandle findServiceHandle)
{
    for (auto& it : handles) {
        LOGINFO << "DiagConditionCheck::PS_OTAModeStatusAvailabilityCallback: InstanceId " << static_cast<uint16_t>(it.GetInstanceId().getInstanceId());
    }

    if (handles.size() > 0) {
        if (ota_mode_proxy == nullptr) {
            ota_mode_proxy = std::make_shared<OtaModeProxy>(handles[0]);
            LOGINFO << "DiagConditionCheck::PS_OTAModeStatusAvailabilityCallback: create handle with instance: " << static_cast<uint16_t>(ota_mode_proxy->GetHandle().GetInstanceId());
        }
    }
    else {
        LOGERROR << "DiagConditionCheck::PS_OTAModeStatusAvailabilityCallback: ota mode proxy is null";
        if (ota_mode_proxy != nullptr) {
            ota_mode_proxy->EnOTAModeStatus.Unsubscribe();
            ota_mode_proxy->EnOTAModeStatus.UnsetReceiveHandler();
        }
        ota_mode_proxy = nullptr;
    }
}

void DiagConditionCheck::IEPowerGroupProxyAvailabilityCallback(ara::com::ServiceHandleContainer<IEPowerGroupProxy::HandleType> handles,
                                                               ara::com::FindServiceHandle findServiceHandle)
{
    for (auto& it : handles) {
        LOGINFO << "DiagConditionCheck::IEPowerGroupProxyAvailabilityCallback: InstanceId " << static_cast<uint16_t>(it.GetInstanceId());
    }
    if (handles.size() > 0) {
        if (iepowergroup_proxy == nullptr) {
            iepowergroup_proxy = std::make_shared<IEPowerGroupProxy>(handles[0]);
            LOGINFO << "DiagConditionCheck::IEPowerGroupProxyAvailabilityCallback: create handle with instance: " << static_cast<uint16_t>(iepowergroup_proxy->GetHandle().GetInstanceId());
        }
    }
    else {
        LOGERROR << "DiagConditionCheck::IEPowerGroupProxyAvailabilityCallback: iepowergroup_proxy is null.";
        if (iepowergroup_proxy != nullptr) {
            iepowergroup_proxy->EnALLPowerSt.Unsubscribe();
            iepowergroup_proxy->EnALLPowerSt.UnsetReceiveHandler();
        }
        iepowergroup_proxy = nullptr;
    }
}

void DiagConditionCheck::MproxySetterServiceProxyAvailabilityCallback(ara::com::ServiceHandleContainer<MproxySetterServiceProxy::HandleType> handles,
                                                                      ara::com::FindServiceHandle findServiceHandle)
{
    for (auto& it : handles) {
        LOGINFO << "DiagConditionCheck::MproxySetterServiceProxyAvailabilityCallback: InstanceId " << static_cast<uint16_t>(it.GetInstanceId());
    }
    if (handles.size() > 0) {
        if (mproxysetterservice_proxy == nullptr) {
            mproxysetterservice_proxy = std::make_shared<MproxySetterServiceProxy>(handles[0]);
            LOGINFO << "DiagConditionCheck::MproxySetterServiceProxyAvailabilityCallback: create handle with instance: " << static_cast<uint16_t>(mproxysetterservice_proxy->GetHandle().GetInstanceId());
        }
    }
    else {
        LOGERROR << "DiagConditionCheck::MproxySetterServiceProxyAvailabilityCallback: mproxysetterservice_proxy is null.";
        mproxysetterservice_proxy = nullptr;
    }
}
#endif

void DiagConditionCheck::Init()
{
    // 初始化联网状态服务
    LOGINFO << "DiagConditionCheck::Init: interenet proxy is finding service...";
#ifdef NEUSAR_AP_R2111
    InterenetStateProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<InterenetStateProxy::HandleType> handles, ara::com::FindServiceHandle handler)
        {
            DiagConditionCheck::VCPInternetStateInterfaceAvailabilityCallback(std::move(handles), handler);
        }, ara::core::InstanceSpecifier(ara::com::vcpintrenetstateinterface::App_CCU_VCCSInternetStateInterface_Sts::instance_specifier_RPort_VCCSInternetStateInterface));
#else
    InterenetStateProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<InterenetStateProxy::HandleType> handles, ara::com::FindServiceHandle handler)
        {
            DiagConditionCheck::VCPInternetStateInterfaceAvailabilityCallback(std::move(handles), handler);
        });
#endif
    #if 0
    // 初始化ota模式服务
    LOGINFO << "DiagConditionCheck::Init: ota mode proxy is finding service...";
    OtaModeProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<OtaModeProxy::HandleType> handles, ara::com::FindServiceHandle handler)
        {
            DiagConditionCheck::PS_OTAModeStatusAvailabilityCallback(std::move(handles), handler);
        });

    // 初始化请求网络
    LOGINFO << "DiagConditionCheck::Init: NmWrapperProxy proxy is finding service...";

    NmWrapperProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<NmWrapperProxy::HandleType> handles, ara::com::FindServiceHandle handler)
        {
            DiagConditionCheck::NmWrapperInterfaceProxyAvailabilityCallback(std::move(handles), handler);
        });

    // 初始化智能配电电源组代理
    LOGINFO << "DiagConditionCheck::Init: IEPowerGroupProxy is finding service...";
    IEPowerGroupProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<IEPowerGroupProxy::HandleType> handles, ara::com::FindServiceHandle handler)
        {
            DiagConditionCheck::IEPowerGroupProxyAvailabilityCallback(std::move(handles), handler);
        });
    LOGINFO << "DiagConditionCheck::Init: MproxySetterServiceProxy is finding service...";

    MproxySetterServiceProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<MproxySetterServiceProxy::HandleType> handles, ara::com::FindServiceHandle handler)
        {
            DiagConditionCheck::MproxySetterServiceProxyAvailabilityCallback(std::move(handles), handler);
        });

    if (GetVehicleAddr() != 0) {
        LOGERROR << "DiagConditionCheck::Init: parse diagaddr.json failed,get defalut diag address";
        GetDefaultVehicleAddr();
    }
    #endif
}

bool DiagConditionCheck::IsInternetState()
{
    if (interenet_state_proxy == nullptr) {
        LOGERROR << "DiagConditionCheck::IsInternetState: internet state proxy is null, can not get internet state!";
        return false;
    }

    if (global_stop_flag) {
        return false;
    }

    auto future = interenet_state_proxy->PrivateMQTTStatus.Get();
    std::chrono::time_point<std::chrono::steady_clock> deadline =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(5000);
#ifdef NEUSAR_AP_R2111
    if (future.wait_until(deadline) != ara::core::future_status::ready) {
        LOGERROR << "DiagConditionCheck::IsInternetState: request took too long time,quit.";
        return false;
    }
#else
    if (future.wait_until(deadline) != decltype(future)::Status::kReady) {
        LOGERROR << "DiagConditionCheck::IsInternetState: request took too long time,quit.";
        return false;
    }
#endif
    else {
        auto result = future.GetResult();
        if (result.HasValue()) {
            /*
            0x00=success
            0x01=failed
            0x02=other
            */
            auto value = result.Value();
            if (value == 0x00) {
                LOGINFO << "DiagConditionCheck::IsInternetState: private mqtt is connected.";
                return true;
            }
            else {
                LOGERROR << "DiagConditionCheck::IsInternetState: private mqtt is not connected,state: " << value;
                return false;
            }
        }
        else {
            auto error = result.Error();
            LOGERROR << "DiagConditionCheck::IsInternetState: error code: " << error.Value();
            return false;
        }
    }
}

#if 0
bool DiagConditionCheck::IsOtaMode()
{
    if (ota_mode_proxy == nullptr) {
        LOGERROR << "DiagConditionCheck::IsOtaMode: OTA mode proxy is null,can not get ota mode!";
        return false;
    }

    if (global_stop_flag) {
        return false;
    }

    auto future = ota_mode_proxy->EnOTAModeStatus.Get();
    std::chrono::time_point<std::chrono::steady_clock> deadline =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(5000);
#ifdef NEUSAR_AP_R2111
    if (future.wait_until(deadline) != ara::core::future_status::ready) {
        LOGERROR << "DiagConditionCheck::IsOtaMode: request took too long time,quit.";
        return false;
    }
#else
    if (future.wait_until(deadline) != decltype(future)::Status::kReady) {
        LOGERROR << "DiagConditionCheck::IsOtaMode: request took too long time,quit.";
        return false;
    }
#endif
    else {
        auto result = future.GetResult();
        if (result.HasValue()) {
            /*
            0x0=NonOTA Mode
            0x1=OTA Mode
            0x2=Invalid Value
            0x3=Reserved
            */
            auto value = result.Value();
            if (value == ara::com::vsomeip::Enum_OTAModeStatus::OTAMode) {
                LOGINFO << "DiagConditionCheck::IsOtaMode: in ota mode.";
                return true;
            }
            else {
                LOGINFO << "DiagConditionCheck::IsOtaMode: not in ota mode";
                return false;
            }
        }
        else {
            auto error = result.Error();
            LOGERROR << "DiagConditionCheck::IsOtaMode: error code: " << error.Value();
            return false;
        }
    }
}

void DiagConditionCheck::NetworkRequestCall(asf::nm::vsomeip::AppID app_id, asf::nm::vsomeip::PncType pnv_type,
                                            asf::nm::vsomeip::NmStatusType nm_status_type)
{
    if (nmwrapper_proxy == nullptr) {
        LOGERROR << "DiagConditionCheck::NetworkRequestCall: network request proxy is null, can not request network!";
        return;
    }

    if (global_stop_flag) {
        return;
    }

    auto networkfuture = nmwrapper_proxy->NetworkRequest(app_id, pnv_type, nm_status_type);
    std::chrono::time_point<std::chrono::steady_clock> deadline =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(5000);
#ifdef NEUSAR_AP_R2111
    if (networkfuture.wait_until(deadline) != ara::core::future_status::ready) {
        LOGERROR << "DiagConditionCheck::NetworkRequestCall: request took too long time,quit.";
    }
#else
    if (networkfuture.wait_until(deadline) != decltype(networkfuture)::Status::kReady) {
        LOGERROR << "DiagConditionCheck::NetworkRequestCall: request took too long time,quit.";
    }
#endif
    else {
        auto r = networkfuture.GetResult();
        if (r.HasValue()) {
            auto myMethod_output = r.Value();
            LOGINFO << "DiagConditionCheck::NetworkRequestCall: PncNetworkCurrentResult " << static_cast<uint8_t>(myMethod_output.PncNetworkCurrentResult);
        }
        else {
            auto error = r.Error(); // ErrorCode
            LOGERROR << "DiagConditionCheck::NetworkRequestCall: error code: " << error.Value();
        }
    }
}

bool DiagConditionCheck::GetIEPowerState()
{
    if (iepowergroup_proxy == nullptr) {
        LOGERROR << "DiagConditionCheck::GetIEPowerState: IE power proxy is null,can not get IE power state!";
        return false;
    }

    if (global_stop_flag) {
        return false;
    }

    auto future = iepowergroup_proxy->EnALLPowerSt.Get();
    std::chrono::time_point<std::chrono::steady_clock> deadline =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(5000);
#ifdef NEUSAR_AP_R2111
    if (future.wait_until(deadline) != ara::core::future_status::ready) {
        LOGERROR << "DiagConditionCheck::GetIEPowerState: request took too long time,quit!";
        return false;
    }
#else
    if (future.wait_until(deadline) != decltype(future)::Status::kReady) {
        LOGERROR << "DiagConditionCheck::GetIEPowerState: request took too long time,quit!";
        return false;
    }
#endif
    else {
        auto result = future.GetResult();
        if (result.HasValue()) {
            return true;
        }
        else {
            auto error = result.Error();
            LOGERROR << "DiagConditionCheck::GetIEPowerState: error code: " << error.Value();
            return false;
        }
    }
}

bool DiagConditionCheck::SetIEPowerState(uint16_t Duration, bool active)
{
    if (iepowergroup_proxy == nullptr) {
        LOGERROR << "DiagConditionCheck::SetIEPowerState: IE power proxy is null,can not set IE power!";
        return false;
    }

    if (global_stop_flag) {
        return false;
    }

    ::ara::com::vsomeip::ALLPowerSt St{};
    St.fill({0U, false, 0U});

    for (size_t i = 0; i < diag_addr_vector.size(); i++) {
        if(i == size_t(116)) {
            LOGERROR << "DiagConditionCheck::SetIEPowerState: address size is over 116.";
            break;
        }
        LOGINFO << "DiagConditionCheck::SetIEPowerState: address " << diag_addr_vector[i];

        St[i].ecuID = diag_addr_vector[i];
        St[i].active = active;
        St[i].duration = Duration;
    }
    for (size_t i = diag_addr_vector.size(); i < size_t(116); i++) {
        LOGINFO << "DiagConditionCheck::SetIEPowerState: address 0";
        St[i].ecuID = static_cast<uint16_t>(0);
        St[i].active = 0;
        St[i].duration = static_cast<uint16_t>(0);
    }
    LOGINFO << "DiagConditionCheck::SetIEPowerState: diag address ipds request size: " << diag_addr_vector.size();

    auto future = iepowergroup_proxy->EnALLPowerSt.Set(St);
    std::chrono::time_point<std::chrono::steady_clock> deadline =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
#ifdef NEUSAR_AP_R2111
    if (future.wait_until(deadline) != ara::core::future_status::ready) {
        LOGERROR << "DiagConditionCheck::SetIEPowerState: request took too long time,quit!";
        return false;
    }
#else
    if (future.wait_until(deadline) != decltype(future)::Status::kReady) {
        LOGERROR << "DiagConditionCheck::SetIEPowerState: request took too long time,quit!";
        return false;
    }
#endif
    else {
        auto result = future.GetResult();
        if (result.HasValue()) {
            LOGINFO << "DiagConditionCheck::SetIEPowerState: Set IE power state ok.";
            return true;
        }
        else {
            auto error = result.Error();
            LOGERROR << "DiagConditionCheck::SetIEPowerState: error code: " << error.Value();
            return false;
        }
    }
}

bool DiagConditionCheck::SetVTState(uint8_t VTst)
{
    if (mproxysetterservice_proxy == nullptr) {
        LOGERROR << "DiagConditionCheck::SetVTState: Set VT state proxy is null,can not set VirtualTester state!";
        return false;
    }

    if (global_stop_flag) {
        return false;
    }

    auto future = mproxysetterservice_proxy->VirtualTesterSt.Set(VTst);
    std::chrono::time_point<std::chrono::steady_clock> deadline =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(5000);
#ifdef NEUSAR_AP_R2111
    if (future.wait_until(deadline) != ara::core::future_status::ready) {
        LOGERROR << "DiagConditionCheck::SetVTState: request took too long time,quit!";
        return false;
    }
#else
    if (future.wait_until(deadline) != decltype(future)::Status::kReady) {
        LOGERROR << "DiagConditionCheck::SetVTState: request took too long time,quit!";
        return false;
    }
#endif
    else {
        auto result = future.GetResult();
        if (result.HasValue()) {
            LOGINFO << "DiagConditionCheck::SetVTState: Set VT state ok.";
            return true;
        }
        else {
            auto error = result.Error();
            LOGERROR << "DiagConditionCheck::SetVTState: error code: " << error.Value();
            return false;
        }
    }
}

int DiagConditionCheck::GetVehicleAddr()
{
    Json::Reader reader;
    Json::Value root;
    Json::Value data_array;
    std::string diag_addr_str;
    int ret = 0;

    if (!ReadJsonFile(diag_addr_str)) {
        LOGERROR << "DiagConditionCheck::GetVehicleAddr: read diagAddr.json failed.";
        ret = -1;
        return ret;
    }

    if (!reader.parse(diag_addr_str,root)) {
        LOGERROR << "DiagConditionCheck::GetVehicleAddr: json parse error.";
        ret = -1;
        return ret;
    }

    data_array = root["address"];
    if (!data_array.isArray()) {
        LOGERROR << "DiagConditionCheck::GetVehicleAddr: address is not array.";
        ret = -1;
        return ret;
    }

    for(int q = 0; q < static_cast<int>(data_array.size()); q++) {
        Json::Value address = data_array[q];
        std::string myValueStr = address.asString();
        std::string subStr;
        int pos = myValueStr.find("0x");
        if (pos < 0) {
            pos = myValueStr.find("0X");
        }
        if (pos >= 0 && ((pos + 2) < static_cast<int>(myValueStr.length()))) {
            subStr = myValueStr.substr(pos + 2, myValueStr.length());
            int Tohex = 0x0000;
            sscanf(subStr.c_str(), "%x", &Tohex);
            diag_addr_vector.push_back(static_cast<uint16_t>(Tohex));
        }
    }
    std::set<uint16_t> s(diag_addr_vector.begin(),diag_addr_vector.end());
    diag_addr_vector.assign(s.begin(),s.end());

    return ret;
}

void DiagConditionCheck::GetDefaultVehicleAddr()
{
    diag_addr_vector.clear();
    diag_addr_vector.push_back(0x765);
    diag_addr_vector.push_back(0x602);
    diag_addr_vector.push_back(0x705);
    diag_addr_vector.push_back(0x764);
    diag_addr_vector.push_back(0x603);
    diag_addr_vector.push_back(0x762);
    diag_addr_vector.push_back(0x71c);
    diag_addr_vector.push_back(0x70b);
    diag_addr_vector.push_back(0x761);
    diag_addr_vector.push_back(0x727);
    diag_addr_vector.push_back(0x7f1);
    diag_addr_vector.push_back(0x714);
    diag_addr_vector.push_back(0x634);
    diag_addr_vector.push_back(0x710);
    diag_addr_vector.push_back(0x767);
    diag_addr_vector.push_back(0x72d);
    diag_addr_vector.push_back(0x5b);
    diag_addr_vector.push_back(0x48);
    diag_addr_vector.push_back(0x74f);
    diag_addr_vector.push_back(0x7e3);
    diag_addr_vector.push_back(0x721);
    diag_addr_vector.push_back(0x732);
    diag_addr_vector.push_back(0x47);
    diag_addr_vector.push_back(0x62);
    diag_addr_vector.push_back(0x71);
    diag_addr_vector.push_back(0x4d);
    diag_addr_vector.push_back(0x60);
    diag_addr_vector.push_back(0x61);
    diag_addr_vector.push_back(0x770);
    diag_addr_vector.push_back(0x5f);
    diag_addr_vector.push_back(0x684);
    diag_addr_vector.push_back(0x687);
    diag_addr_vector.push_back(0x771);
    diag_addr_vector.push_back(0x74e);
    diag_addr_vector.push_back(0x774);
    diag_addr_vector.push_back(0x75b);
    diag_addr_vector.push_back(0x775);
    diag_addr_vector.push_back(0x47);
    diag_addr_vector.push_back(0x65);
    diag_addr_vector.push_back(0x773);
    diag_addr_vector.push_back(0x72);
    diag_addr_vector.push_back(0x735);
    diag_addr_vector.push_back(0x73d);
    diag_addr_vector.push_back(0x751);
    diag_addr_vector.push_back(0x755);
    diag_addr_vector.push_back(0x748);
    diag_addr_vector.push_back(0x71f);
    diag_addr_vector.push_back(0x750);
    diag_addr_vector.push_back(0x772);
    diag_addr_vector.push_back(0x74c);
    diag_addr_vector.push_back(0x722);
    diag_addr_vector.push_back(0x72b);
    diag_addr_vector.push_back(0x730);
    diag_addr_vector.push_back(0x712);
    diag_addr_vector.push_back(0x713);
    diag_addr_vector.push_back(0x74a);
    diag_addr_vector.push_back(0x73f);
    diag_addr_vector.push_back(0x731);
    diag_addr_vector.push_back(0x742);
    diag_addr_vector.push_back(0x73e);
    diag_addr_vector.push_back(0x740);
    diag_addr_vector.push_back(0x715);
    diag_addr_vector.push_back(0x737);
    diag_addr_vector.push_back(0x716);
    diag_addr_vector.push_back(0x70e);
    diag_addr_vector.push_back(0x7E2);
    diag_addr_vector.push_back(0x761);
    diag_addr_vector.push_back(0x70B);
    diag_addr_vector.push_back(0x7E6);
    diag_addr_vector.push_back(0x764);
    diag_addr_vector.push_back(0x71A);
    diag_addr_vector.push_back(0x7E5);
    diag_addr_vector.push_back(0x605);
    diag_addr_vector.push_back(0x7E4);
    diag_addr_vector.push_back(0x762);
    diag_addr_vector.push_back(0x727);
    diag_addr_vector.push_back(0x7F1);
    diag_addr_vector.push_back(0x714);
    diag_addr_vector.push_back(0x7E7);
    diag_addr_vector.push_back(0x710);
    diag_addr_vector.push_back(0x7E0);
    diag_addr_vector.push_back(0x600);
    diag_addr_vector.push_back(0x72D);
    diag_addr_vector.push_back(0x731);
    diag_addr_vector.push_back(0x740);
    diag_addr_vector.push_back(0x73E);
    diag_addr_vector.push_back(0x56);
    diag_addr_vector.push_back(0x57);
    diag_addr_vector.push_back(0x5B);
    diag_addr_vector.push_back(0x48);
    diag_addr_vector.push_back(0x01);
    diag_addr_vector.push_back(0x02);
    diag_addr_vector.push_back(0x74F);
    diag_addr_vector.push_back(0x7E3);
    diag_addr_vector.push_back(0x738);
    diag_addr_vector.push_back(0x721);
    diag_addr_vector.push_back(0x732);
    diag_addr_vector.push_back(0x60);
    diag_addr_vector.push_back(0x62);
    diag_addr_vector.push_back(0x71);
    diag_addr_vector.push_back(0x4D);
    diag_addr_vector.push_back(0x7DF);

    std::set<uint16_t> s(diag_addr_vector.begin(),diag_addr_vector.end());
    diag_addr_vector.assign(s.begin(),s.end());
}

bool DiagConditionCheck::ReadJsonFile(std::string& jsonStr)
{
    auto result = OpenFileStorage__NeuSAR_AsfVirtualDiagnosticTester_PRPort_FS;
    if (!result) {
        LOGERROR << "DiagConditionCheck::ReadJsonFile: openfs failed.";
        return false;
    }

    auto db = std::move(result).Value();
    auto openRst = db->OpenFileReadWrite(DIAG_ADDR_FILE);
    if (!openRst) {
        LOGINFO << "DiagConditionCheck::ReadJsonFile: OpenFileReadWrite failed";
        auto resetRet = db->ResetFile(DIAG_ADDR_FILE);
        if (!resetRet.HasValue()) {
            LOGERROR << "DiagConditionCheck::ReadJsonFile: resetRet diagaddr.json failed.";
            return false;
        }
        else {
            LOGINFO << "DiagConditionCheck::ReadJsonFile: resetRet diagaddr.json success.";
        }
        openRst = db->OpenFileReadWrite(DIAG_ADDR_FILE);
        if (!openRst.HasValue()) {
            LOGERROR << "DiagConditionCheck::ReadJsonFile: open diagaddr.json 2nd failed.";
            return false;
        }
    }
    auto db1 = std::move(openRst).Value();
    db1->seek(0, ara::per::BasicOperations::SeekDirection::kEnd);
    auto keysize = db1->tell();
    LOGINFO << "DiagConditionCheck::ReadJsonFile: filesize " << static_cast<int>(keysize);

    db1->seek(0);
    char *readch = new char[static_cast<size_t>(keysize) + 1]{0};
    db1->read(ara::core::Span<char>(readch, keysize));

    jsonStr = readch;

    delete[] readch;
    return true;
}
#endif
} // namespace vdi
} // namespace asf
#endif
