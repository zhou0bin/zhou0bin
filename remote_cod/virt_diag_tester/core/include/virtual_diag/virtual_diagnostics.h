/*****************************************************************************
* @file     virtual_diagnostics.h
* @brief    
* @author   kevin
* @date     2025/4/21
*****************************************************************************/
#ifndef VD_VIRTUAL_DIAGNOSTICS_H_
#define VD_VIRTUAL_DIAGNOSTICS_H_

#include <string>
#include <memory>
#include <thread>
#include <sys/un.h>

class DM_DiagClientInterf;
class LocalDiagClient;
class OtaInteraction;
class InteractiveService;

namespace asf {
namespace vdi {

bool GetGlobalStop();

void GlobalStop();

class VirtualDiagnostics
{
public:
    VirtualDiagnostics()
        : interact_file_("")
        , local_diag_service_init_flag_(false)
        , is_in_factory_mode_(false)
    {}
    
    ~VirtualDiagnostics() { Close(); }

    void Init();

    void Close();

    void Release();

private:
#if 0
    void BuildInteractionService();
#endif
#ifdef CONDITION_CHECK
    void MaintainNetwork();
#endif

#ifdef REQUEST_PUBLIC_KEY
    void RequestPublickey();
#endif
#ifdef LOCAL_DIAG_CLIENT
    void DynamicLocalDiagService();
#endif
#ifdef USING_MODEL
    void InitScriptParseFloder();
#endif

    void InitVehicleData();

    void SendDiagData(const std::string& diagData);

    bool GetVinData(std::string &vinData);

    time_t GetTimeStamp();

    std::string CreateTaskId();

    std::shared_ptr<DM_DiagClientInterf> dm_diag_client_instance_;

    std::shared_ptr<LocalDiagClient> dm_local_diag_client_instance_;

#ifdef OTA_INTERACTION
    std::shared_ptr<OtaInteraction> ota_interaction_instance_;
#endif

#if 0
    std::shared_ptr<InteractiveService> interactive_service_instance_;
#endif
    std::string interact_file_;
    struct sockaddr_un sock_;

#ifdef CONDITION_CHECK
    std::thread network_req_thread_;
#endif
#ifdef REQUEST_PUBLIC_KEY
    std::thread request_public_key_thread_;
#endif
#ifdef LOCAL_DIAG_CLIENT
    std::thread handle_local_diag_service_thread_
#endif
    std::thread script_interact_thread_;

    bool local_diag_service_init_flag_;
    bool is_in_factory_mode_;
};

} // namespace vdi
} // namesapce asf

#endif // VD_VIRTUAL_DIAGNOSTICS_H_