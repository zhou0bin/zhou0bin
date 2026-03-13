/*****************************************************************************
* @file     local_diag_client.h
* @brief    
* @author   kevin
* @date     2025/4/23
*****************************************************************************/
#ifndef VD_DM_LOCAL_DIAG_CLIENT_H_
#define VD_DM_LOCAL_DIAG_CLIENT_H_

#ifdef LOCAL_DIAG_CLIENT
#include "3rdparty/diag_script_parsing.h"

#include "diag/file_adapter.h"

#include "global/global.h"

#include "common/system_cmd.h"
#include "common/log.h"
#include "common/common.h"

class LocalDiagClient : public LocalDiagClientSkeleton
{
public:
    LocalDiagClient(ara::com::InstanceIdentifier instance, ara::com::MethodCallProcessingMode mode)
        : LocalDiagClientSkeleton(instance, mode)
    {}

    LocalDiagClient() = default;
    ~LocalDiagClient() = default;

    virtual ara::core ::Future<receiveLocalDiagDataOutput> receiveLocalDiagData(const ::ara::com::vsomeip::DIAGNOSISDOWNDATA&
                                                                                receiveLocalDiagnosisData) override;

    void Init();

    void Release();

    static std::shared_ptr<LocalDiagClient> GetInstance();

private:
    void OnBoardDiagTaskProcess(std::string protocolVersion, std::string taskId, std::string packageName, std::string packageUrl,
                                std::string resultUrl, u_int taskTimeout, bool interactScript, bool stateUpload,
                                u_int uploadPeriod, std::string encryptFileSha256, std::string key0);

    void PresetDiagTaskProcessForLocal();

    void StopDiag(std::string protocolVersion, std::string taskId, std::string packageName, u_int taskTimeout);

    void SendInteractResponseParameter(std::string protocolVersion, std::string taskId, std::string packageName,
                                       Json::Value interactResponseParameter);

    void ResponseVehicleData(std::string protocolVersion, std::string taskId);

    void ModifyParameters(std::string protocolVersion, std::string taskId, Json::Value scriptRunCondition);

    void ModifyDiagEngineTimeParameters(std::string protocolVersion, std::string taskId, u_int p2Client, u_int p2StarClient);

    void DeleteLocalDiagFile(std::string protocolVersion, std::string taskId, u_int fileType, std::string fileName);

    void UpdateLocalDiagFile(std::string protocolVersion, std::string taskId, std::string packageUrl, u_int fileType);

    void QueryFileInf(std::string protocolVersion, std::string taskId, u_int fileType, std::string packageName);

    uint8_t DiagFileDownload(const std::string& diagFileName, const std::string& fileUrl);

    uint8_t DiagScriptRunResultUpload(const std::string& fileName, const std::string& resultPackageNameAndPath, const std::string& resultURL);

    std::string CreateJsonResponse(int messageType, std::string protocolVersion, std::string taskId, Json::Value customParameter);

    void SendDiagData(const std::string diagData);

    time_t GetTimeStamp();

    SystemCmd file_opreation_instance;
    ScriptProcess* script_process_instance = ScriptProcess::GetInstance();
    // SendData data;
    std::atomic<bool> process_stop_request_completed{false};
    std::atomic<bool> script_stop_success_flag{false};
    std::atomic<bool> is_received_stop_task_signal{false};
    std::atomic<ScriptState> obd_task_script_state{ScriptState::NoTask};
    static std::shared_ptr<LocalDiagClient> my_instance;
    static std::mutex my_mutex;
};
#endif // LOCAL_DIAG_CLIENT

#endif // VD_DM_LOCAL_DIAG_CLIENT_H_
