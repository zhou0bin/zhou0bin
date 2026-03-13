/*****************************************************************************
* @file     global.cpp
* @brief    Global
* @date     2025/4/24
*****************************************************************************/

#include "global/global.h"

InteractState interact_state;
StopTaskState stop_task_state;

std::atomic<bool> is_in_task{false};
std::atomic<bool> is_in_local_task{false};
std::atomic<bool> is_in_ota_task{false};
std::atomic<bool> is_script_running{false};
std::atomic<bool> is_read_interaction_message_finish{true};

std::shared_ptr<asf::vdi::VehicleCloudProxy> vc_proxy_instance = std::make_shared<asf::vdi::VehicleCloudProxy>();

#ifdef VEHICLE_CONFIG
std::shared_ptr<asf::vdi::VehicleConfigManagerProxy> vcm_proxy_instance = std::make_shared<asf::vdi::VehicleConfigManagerProxy>();
#endif
#ifdef CONDITION_CHECK
std::shared_ptr<asf::vdi::DiagConditionCheck> condition_check = std::make_shared<asf::vdi::DiagConditionCheck>();
#endif
/* kevin auth */
std::shared_ptr<asf::vdi::ProxyHandleAuthenticationStateImpl> authentication_state_instance = std::make_shared<asf::vdi::ProxyHandleAuthenticationStateImpl>();

bool global_stop_flag = false;