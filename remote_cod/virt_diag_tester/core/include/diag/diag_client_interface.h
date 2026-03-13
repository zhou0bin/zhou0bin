#ifndef VD_DM_DIAG_CLIENT_INTERF_H
#define VD_DM_DIAG_CLIENT_INTERF_H

#include <cstdint>
#include <cstdlib>
#include <random>
#include <atomic>
#include <time.h>
#include <chrono>

#include "3rdparty/interactive_interface.h"
#include "3rdparty/diag_script_parsing.h"
#include "3rdparty/a02gh_pn_code.h"
#include "3rdparty/a66_pn_code.h"
#include "common/json.h"
#include "common/log.h"
#include "common/system_cmd.h"
#include "global/global.h"

#include "auth/auth_state.h"
#include "check/check_json_key.h"
#include "check/condition_check.h"
#include "diag/diag_file.h"
#include "diag/file_adapter.h"

#include "common/common.h"
#include "domain_socket/server.h" 

class DM_DiagClientInterf
{
public:
    using CheckJsonKey = asf::vdi::CheckJsonKey;

    DM_DiagClientInterf() = default;
    ~DM_DiagClientInterf() = default;

    std::string receiveDiagData(const std::string receiveDiagnosisData);

    void Init();

    void Release();

private:
    void RemoteDiagTaskProcess(std::string signalingContent, std::string protocolVersion, std::string taskId, std::string packageName,
                               std::string packageUrl, std::string resultUrl, int packageUrlSsl, int resultUrlSsl,
                               u_int taskTimeout, bool interactScript, bool stateUpload, u_int uploadPeriod,
                               std::string encryptFileSha256,size_t package_size, std::string packageSha256,
                               std::string publicKeySha256, std::string key0, std::string signBase64, 
                               int excutionMode, int taskPriority, int taskPeriod, Json::Value conditionCheckValue);
#if 0
    void PresetDiagTaskProcessForCould(std::string signalingContent, std::string protocolVersion, std::string taskId, std::string packageName,
                                       std::string resultUrl, int resultUrlSsl, u_int taskTimeout, bool interactScript,
                                       bool stateUpload, u_int uploadPeriod, std::string signBase64);

    void StopDiag(std::string signalingContent, std::string signBase64, std::string protocolVersion, std::string taskId,
                  std::string packageName, u_int taskTimeout);
#endif
    bool CheckSigalingSign(std::string signalingContent, std::string signBase64,DiagFile &diagFile);

    bool CheckPublicKeySha256(std::string publicKeySha256);

    void WriteVehicleData(const std::string& encryptVehicleDataBase64, const std::string& key0);

    void ModifyParameters(std::string signalingContent, std::string signBase64,
                          std::string protocolVersion, std::string taskId,Json::Value scriptRunCondition);

    void ModifyDiagEngineTimeParameters(std::string signalingContent, std::string signBase64, std::string protocolVersion,
                                        std::string taskId, u_int p2Client, u_int p2StarClient);

    void DeleteLocalDiagFile(std::string signalingContent, std::string signBase64, std::string protocolVersion,
                             std::string taskId, u_int fileType, std::string fileName);

    void UpdateLocalDiagFile(std::string signalingContent, std::string signBase64, std::string protocolVersion,
                             std::string taskId, std::string packageUrl, int packageUrlSsl, u_int fileType);

    void QueryFileInf(std::string signalingContent, std::string signBase64, std::string protocolVersion,
                      std::string taskId, u_int fileType, std::string packageName);

    void ReceiveAndSavePublicKey(std::string protocolVersion, std::string taskId, std::string publicKey, std::string publicKeySha256,
                                 bool pkiSign, std::string pkiSignValue, std::string pkiSigner);

    void SendInteractResponseParameter(std::string signalingContent, std::string signBase64, std::string protocolVersion,
                                       std::string taskId, std::string packageName,Json::Value interactResponseParameter);

    void Pn2EcuName(std::string &pnStr, std::string &ecuStr);

    void SendDiagData(const std::string diagData);

    std::string CombineSignalingContent(std::string messageType, std::string protocolVersion,
                                   long timeStamp, std::string taskId, std::string customParameter);

    std::string CreateJsonResponse(int messageType, std::string protocolVersion, std::string taskId,Json::Value customParameter);

    uint8_t DiagFileDownload(const std::string& diagFileName, const std::string& fileUrl, int authMode);

    uint8_t DiagScriptRunResultUpload(const std::string& fileName, const std::string& resultPackageNameAndPath, const std::string& resultURL, int authMode);

    time_t GetTimeStamp();
    uint8_t authentication_state = midware::diag::AuthenticationState::kInitial;
    ScriptProcess* script_process_instance = ScriptProcess::GetInstance();
    // SendData data;
    SystemCmd file_opreation_instance;
    CheckJsonKey check_json_key_instance;
    std::atomic<bool> process_stop_request_completed{false};
    std::atomic<bool> script_stop_success_flag{false};
    std::atomic<bool> is_received_stop_task_signal{false};
    std::atomic<ScriptState> remote_task_script_state{ScriptState::NoTask};
    //asf::dm_diagclientinterf::skeleton::DM_DiagClientInterfSkeleton* my_skeleton;

    void ProcessEventOrPeriodDiagTaskResult(const ReceiveData& recv_data);
    bool ProcessDiagTaskResult(ErrorType process_result, std::string script_path, bool& is_report_launch_result, std::string protocolVersion, std::string taskId, bool& process_end);
    void PackAndEncryptAndUploadDiagResult(std::string script_result_path, std::string packageName, std::string resultUrl, int resultUrlSsl, std::string protocolVersion, std::string taskId);

    std::string vcp_server_sock_{"/tmp/vtest_cmd.sock"};
    std::string idcu_server_sock_{"/tmp/DiagTaskUserAuthRequest.sock"};
    std::string idcu_client_sock_{"/tmp/DiagTaskUserAuthResponse.sock"};
	DomainSocket::Server vcp_server_socket_{vcp_server_sock_};
    DomainSocket::Client idcu_client_socket_{idcu_server_sock_};
    DomainSocket::Server idcu_server_socket_{idcu_client_sock_};
    
    struct st_DiagTaskUserAuthRequest{
        std::string taskid;   //诊断任务标识
        std::string taskdesc; //诊断任务描述
    };

    struct st_DiagTaskUserAuthResponse
    {
        std::string taskid;  //诊断任务标识
        uint8_t auth_code; //用户授权状态码：0表示此任务禁止执行、1表示此任务确认执行、2表示本次上电周期所有任务均全部禁止执行、3表示本次上电周期所有任务均全部确认执行
    };

    std::string SerializeDiagTaskUserAuthRequest(const st_DiagTaskUserAuthRequest& request);
    st_DiagTaskUserAuthResponse DeserializeDiagTaskUserAuthResponse(const std::string& response);

    std::map<std::string, uint8_t> idcu_response_map_{};
    std::mutex idcu_connect_mutex_;
    std::atomic<uint8_t> user_auth_code_{0xff};
};

#endif //DM_DIAG_CLIENT_INTERF_H_
