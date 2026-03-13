/*****************************************************************************
* @file     global.h
* @brief    
* @author   kevin
* @date     2025/4/24
*****************************************************************************/

#ifndef VD_GLOBAL_VARIABLE_H_
#define VD_GLOBAL_VARIABLE_H_

#include <iostream>
#include <vector>
#include <string>

#include "auth/auth_state.h"
#include "check/condition_check.h"
#include "proxy/vehicle_cloud_proxy.h"
#include "proxy/vehicle_config_manager_proxy.h"


enum DownloadResult : uint8_t
{
    DownloadSuccessful = 0x00,
    DownloadFailed = 0x01,
    DownloadVerifyFailed = 0x02
};

enum UPloadResult : uint8_t
{
    UploadSuccessful = 0x00,
    UploadFailed = 0x01,
    UploadVerifyFailed = 0x02
};

enum ScriptState : uint8_t
{
    NoTask = 0x01,
    NotExecuted = 0x02,
    Running = 0x03,
    Finish = 0x04
};

struct InteractResponseParameter
{
    int result = -1;
    std::string interact_type = "";
    int selected_index = -1;
    std::vector<std::string> input_value;
};

struct InteractState
{
    std::string protocol_version = "";
    std::string task_id = "";
    bool is_response = false;
    bool is_finish = true;
    int sequence = 0;
    int run_type = 0;
    InteractResponseParameter response_parameter;
};

struct StopTaskState
{
    bool recieve_stop_request = false;
    bool is_finish = true;
    bool stop_successful = false;
    std::string script_name = "";
};

extern InteractState interact_state;
extern StopTaskState stop_task_state;

#ifdef CONDITION_CHECK
extern std::shared_ptr<asf::vdi::DiagConditionCheck> condition_check;
#endif
#ifdef VEHICLE_CONFIG
extern std::shared_ptr<asf::vdi::VehicleConfigManagerProxy> vcm_proxy_instance;
#endif
extern std::shared_ptr<asf::vdi::VehicleCloudProxy> vc_proxy_instance;
/* kevin auth */
extern std::shared_ptr<asf::vdi::ProxyHandleAuthenticationStateImpl> authentication_state_instance;

extern std::atomic<bool> is_in_task;
extern std::atomic<bool> is_in_local_task;
extern std::atomic<bool> is_in_ota_task;
extern std::atomic<bool> is_script_running;
extern std::atomic<bool> is_read_interaction_message_finish;

extern bool global_stop_flag;

#endif // VD_GLOBAL_VARIABLE_H_
