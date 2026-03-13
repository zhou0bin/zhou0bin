/*****************************************************************************
* @file     diag_client_interface.h
* @brief    
* @author   kevin
* @date     2025/4/24
*****************************************************************************/

#ifndef VD_OTA_Interaction_H_
#define VD_OTA_Interaction_H_

#include <cstdint>
#include <cstdlib>
#include <random>
#include <atomic>
#include <time.h>
#include <chrono>

#include "3rdparty/diag_script_parsing.h"
#include "check/condition_check.h"
#include "diag/diag_file.h"
#include "common/common.h"
#include "common/json.h"
#include "common/log.h"
#include "common/system_cmd.h"
#include "global/global.h"

#ifdef OTA_INTERACTION
namespace asf {
namespace vdi {

enum OtaRequestAck : uint8_t
{
    Success = 0x00,
    LastTaskNotOver = 0x01,
    DecompressZipFailed = 0x02,
    ScriptNotExist = 0x03,
    UnKnown = 0x04
};

enum ScriptRunState : uint8_t
{
    ExecuteFinish = 0x00,
    ScriptRunning = 0x01,
    ExecuteFailed = 0x02,
    ConditionNotAllow = 0x03,
    UnknowError = 0x04
};

class OtaInteraction : public ara::com::ota_vtap_interaction::skeleton::OTA_VTAP_InteractionSkeleton
{
public:
    using Skeleton = ara::com::ota_vtap_interaction::skeleton::OTA_VTAP_InteractionSkeleton;

    OtaInteraction(ara::com::InstanceIdentifier instance, ara::com::MethodCallProcessingMode mode)
        : Skeleton(instance, mode)
    {}

    OtaInteraction() = default;
    ~OtaInteraction() = default;

    virtual ::ara::core::Future<OTAGetVTAPScriptRunStateOutput> OTAGetVTAPScriptRunState(
        const ::ara::com::vsomeip::OTAGetVTAPScriptRunState& OTARequestScriptRunState) override;

    virtual ::ara::core::Future<OTAStartVTAPRunScriptOutput> OTAStartVTAPRunScript(
        const ::ara::com::vsomeip::ReqOTAStartVTAPRunScript& ReqOTAStartVTAPRunScript) override;

    void Init();

    void Release();

private:
    void OtaTaskProcess();

    // void MaintainOtaTaskStatus();

    bool IsScriptFileExist(std::string scriptFilePath);

    ScriptProcess* script_process_instance = ScriptProcess::GetInstance();
    SendData data;
    SystemCmd file_opreation_instance;
    DiagFile diag_file_instance;
    // std::atomic<bool> is_task_finish{true};
    std::atomic<uint8_t> run_state{ScriptRunState::UnknowError};
    std::string script_name = "";
};

} // namespace vdi
} // namespace asf
#endif

#endif // VD_OTA_Interaction_H_
