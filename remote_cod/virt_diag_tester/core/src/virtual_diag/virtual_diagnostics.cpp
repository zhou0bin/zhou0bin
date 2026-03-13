/*****************************************************************************
* @file     virtual_diagnostics.cpp
* @brief    Virtual Diagnostics 
* @date     2025/4/21
*****************************************************************************/

#include "virtual_diag/virtual_diagnostics.h"
#include "diag/diag_client_interface.h"
#include "diag/local_diag_client.h"
#include "interface/ota_interaction.h"
#include "3rdparty/interactive_interface.h"
#include "common/macro.h"

namespace asf {
namespace vdi {

bool GetGlobalStop()
{
    return global_stop_flag;
}

void GlobalStop()
{
    global_stop_flag = true;
}

void VirtualDiagnostics::Close()
{
#ifdef CONDITION_CHECK
    if (network_req_thread_.joinable()) {
        network_req_thread_.join();
    }
#endif

#ifdef REQUEST_PUBLIC_KEY
    if (request_public_key_thread_.joinable()) {
        request_public_key_thread_.join();
    }
#endif

#ifdef LOCAL_DIAG_CLIENT
    if (handle_local_diag_service_thread_.joinable()) {
        handle_local_diag_service_thread_.join();
    }
#endif

    if (script_interact_thread_.joinable()) {
        script_interact_thread_.join();
    }
}

void VirtualDiagnostics::Init()
{

    vc_proxy_instance->Init();
#ifdef VEHICLE_CONFIG
    //初始化Vin码获取服务调用
    vcm_proxy_instance->Init();
#endif
#ifdef CONDITION_CHECK
    //初始化运行条件检查服务调用
    condition_check->Init();
#endif
    //初始化监听安全认证状态
    authentication_state_instance->Offer();

    dm_diag_client_instance_ = std::make_shared<DM_DiagClientInterf>();

    dm_diag_client_instance_->Init();

#ifdef LOCAL_DIAG_CLIENT
    // 动态化处理8015服务
    // kevin 本地诊断
    dm_local_diag_client_instance_ = LocalDiagClient::GetInstance();
    handle_local_diag_service_thread_ = std::thread(&VirtualDiagnostics::DynamicLocalDiagService, this);
#endif

#ifdef OTA_INTERACTION
    //初始化ota交互110b服务
#ifdef NEUSAR_AP_R2111
    ota_interaction_instance_ = std::make_shared<OtaInteraction>(ara::com::InstanceIdentifier(1, ara::com::internal::MiddlewareType::VSOMEIP_BINDING),
                                ara::com::MethodCallProcessingMode::kEventSingleThread);
#else
    ota_interaction_instance_ = std::make_shared<OtaInteraction>(ara::com::InstanceIdentifier(1),
                                ara::com::MethodCallProcessingMode::kEventSingleThread);
#endif
    ota_interaction_instance_->Init();
#endif
#if 0
    //初始化交互功能socket线程，注册回调函数
    BuildInteractionService();
#endif
#ifdef CONDITION_CHECK
    //初始化网络状态维持接口调用
    MaintainNetwork();
#endif

#ifdef USING_MODEL
    InitScriptParseFloder();
#endif
#if 0
    //初始化vehicle_data.json文件
    InitVehicleData();
#endif
    //初始化公钥
    if(access(KEY_FOLDER, 0) == -1) {
        LOGINFO << "VirtualDiagnostics::Init: /key floder does not exist, creating...";
        mkdir(KEY_FOLDER, 0777);

        if(access(KEY_FOLDER, 0) == 0) {
            LOGINFO << "VirtualDiagnostics::Init: /key create success.";
        }
        else {
            LOGERROR << "VirtualDiagnostics::Init: /key create failed.";
        }
    }
#ifdef REQUEST_PUBLIC_KEY
    // kevin 公钥请求
    request_public_key_thread_ = std::thread(&VirtualDiagnostics::RequestPublickey, this);
#endif
    //初始化远程诊断结果文件大数据目录
    if(access(DATA_DSCR_FLODER, 0) == -1) {
        LOGINFO << "VirtualDiagnostics::Init: " << DATA_DSCR_FLODER << " floder does not exist,creating...";
        mkdir(DATA_DSCR_FLODER, 0777);

        if(access(DATA_DSCR_FLODER, 0) == 0) {
            LOGINFO << "VirtualDiagnostics::Init: " << DATA_DSCR_FLODER << " create successful.";
        }
        else {
            LOGERROR << "VirtualDiagnostics::Init: " << DATA_DSCR_FLODER << " create failed.";
        }
    }
    if(access(BIG_DATA_FLODER, 0) == -1) {
        LOGINFO << "VirtualDiagnostics::Init: " << BIG_DATA_FLODER << " floder does not exist,creating...";
        mkdir(BIG_DATA_FLODER, 0777);

        if(access(BIG_DATA_FLODER, 0) == 0) {
            LOGINFO << "VirtualDiagnostics::Init: " << BIG_DATA_FLODER << " create successful.";
        }
        else {
            LOGERROR << "VirtualDiagnostics::Init: " << BIG_DATA_FLODER << " create failed.";
        }
    }
}

void VirtualDiagnostics::Release()
{
#ifdef LOCAL_DIAG_CLIENT
    // kevin 本地诊断
    if(dm_local_diag_client_instance_ && local_diag_service_init_flag_) {
        dm_local_diag_client_instance_->Release();
    }
#endif

    dm_diag_client_instance_->Release();

#ifdef OTA_INTERACTION
    // ota_interaction_instance_->Release();
#endif
#if 0
    interactive_service_instance_->Close();
#endif

    /* kevin auth */
    authentication_state_instance->StopOffer();

}

#if 0
void VirtualDiagnostics::BuildInteractionService()
{
    std::ifstream socket_file;
    Json::Reader reader;
    Json::Value root;

    socket_file.open(SOCKET_CONFIG_FILE_PATH);
    if (!reader.parse(socket_file, root, false)) {
        LOGERROR << "VirtualDiagnostics::Init: parse interact socket parameters file failed!";
        return;
    }
    else {
        interact_file_ = root["Interact_Pip_File"].asString();
        LOGINFO << "VirtualDiagnostics::Init: open interact socket file,path: " << interact_file_.c_str();
    }
    socket_file.close();
    memset(&sock_, 0, sizeof(sock_));
    sock_.sun_family = AF_UNIX;
    strncpy(sock_.sun_path, interact_file_.c_str(), sizeof(sock_.sun_path)-1);
    interactive_service_instance_ = std::make_shared<InteractiveService>();
    script_interact_thread_ = std::thread(
    [&, this]()
    {
        interactive_service_instance_->Init();
        interactive_service_instance_->BuildService(sock_);
        interactive_service_instance_->SetCallback();
    });
}
#endif

#ifdef CONDITION_CHECK
void VirtualDiagnostics::MaintainNetwork()
{
    #if 0
    LOGINFO << "VirtualDiagnostics::MaintainNetwork: init network request thread...";
    network_req_thread_ = std::thread(
        [this]()
        {
            while (MAINTAIN_NETWORK)
            {
                if (global_stop_flag)
                {
                    return;
                }

                if (is_in_task || is_in_local_task || is_in_ota_task)
                {
                    // keivn condition check 整车网络维持服务
                    condition_check->NetworkRequestCall("VT", asf::nm::vsomeip::PncType::PncGlobal, asf::nm::vsomeip::NmStatusType::FullCom);
                }
                std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
            }
        });
    #endif
}
#endif

#ifdef REQUEST_PUBLIC_KEY
void VirtualDiagnostics::RequestPublickey()
{
    Json::Value public_key_root;
    DiagFile diag_file_instance;
    std::ifstream public_key_file;
    std::ifstream public_key_sha256_file;
    std::vector<uint8_t> public_key;
    std::vector<uint8_t> public_key_sha256;
    std::string public_key_str = "";
    std::string local_public_key_sha256 = "";
    std::string request_json = "";
    bool compare_result = false;
    // bool is_private_mqtt_connected = false;
    bool is_private_mqtt_connected = true;       
    char temp;
    if(access(PUBLIC_KEY_PATH, 0) == 0) {
        public_key_file.open(PUBLIC_KEY_PATH, std::ios::binary | std::ios::in);
        if(public_key_file.is_open()) {
            diag_file_instance.GetCryptoProvider();
            diag_file_instance.PublickeyRead(public_key_str);

            // int begin_pos = static_cast<int>(public_key_str.find("-----BEGIN PUBLIC KEY-----\n"));
            // if(begin_pos >= 0){
            //     public_key_str.erase(begin_pos,27);
            // }
            // int end_pos = static_cast<int>(public_key_str.find("\n-----END PUBLIC KEY-----"));
            // if(begin_pos >= 0){
            //     public_key_str.erase(end_pos);
            // }
            public_key.clear();
            for(auto iter : public_key_str){
                public_key.push_back((uint8_t)iter);
            }

            //计算并且比对公钥的sha256哈希值
            public_key_sha256_file.open(PUBLIC_KEY_SHA256_PATH, std::ios::binary | std::ios::in);
            if(public_key_sha256_file.is_open()) {
                while(public_key_sha256_file.get(temp)) {
                    local_public_key_sha256.push_back(temp);
                }

                public_key_sha256 = diag_file_instance.CalculateFileSha256(public_key);

                compare_result = diag_file_instance.CompareSha256(public_key_sha256,local_public_key_sha256);

                public_key_file.close();
                public_key_sha256_file.close();
            }
            else {
                LOGERROR << "VirtualDiagnostics::RequestPublickey: can not open public key sha256 file.";
                compare_result = false;
            }

            if(compare_result) {
                //sha256值正确
                LOGINFO << "VirtualDiagnostics::RequestPublickey: sha256 value of the public key is verified,do not need to apply for new public key.";
            }
            else {
                //sha256值不正确，向云端请求公钥
                LOGINFO << "VirtualDiagnostics::RequestPublickey: public key sha256 value match failed,requesting public key...";
#ifdef CONDITION_CHECK
                //检查联网状态,联网后向云端请求公钥
                is_private_mqtt_connected = condition_check->IsInternetState();
#endif
                while(is_private_mqtt_connected != true) {
                    if(global_stop_flag) {
                        break;
                    }
                    try {
                        if(is_private_mqtt_connected) {
                            LOGINFO << "VirtualDiagnostics::RequestPublickey: private mqtt is connected.";
                            break;
                        }
#ifdef CONDITION_CHECK
                        // kevin condition check
                        is_private_mqtt_connected = condition_check->IsInternetState();
#endif
                        std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(5));
                    }
                    catch(...) {
                        LOGERROR << "VirtualDiagnostics::RequestPublickey: request public key error.";
                    }
                }

                if(is_private_mqtt_connected) {
                    LOGINFO << "VirtualDiagnostics::RequestPublickey: private mqtt is connected,sending request message...";
                    public_key_root["messageType"] = 1;
                    public_key_root["protocolVersion"] = "1";
                    public_key_root["timestamp"] = GetTimeStamp();
                    public_key_root["taskId"] = CreateTaskId();
                    public_key_root["customParameter"];

                    request_json = public_key_root.toStyledString();
                    LOGINFO << "VirtualDiagnostics::RequestPublickey: create request public key json string: " << request_json.c_str();
                    SendDiagData(request_json);
                }
                else {
                    LOGERROR << "VirtualDiagnostics::RequestPublickey: private mqtt is not connected,can not request public key.";
                }
            }
        }
        else {
            LOGERROR << "VirtualDiagnostics::RequestPublickey: init public key failed,can not open public key file!";
        }
    }
    else {
        LOGINFO << "VirtualDiagnostics::RequestPublickey: public key does not exist,requesting public key...";

#ifdef CONDITION_CHECK
        //检查联网状态,联网后向云端请求公钥
        is_private_mqtt_connected = condition_check->IsInternetState();
#endif
        while(is_private_mqtt_connected != true) {
            if(global_stop_flag) {
                break;
            }
            try {
                if(is_private_mqtt_connected) {
                    LOGINFO << "VirtualDiagnostics::RequestPublickey: private mqtt is connected.";
                    break;
                }
#ifdef CONDITION_CHECK
                // kevin conditiocheck
                is_private_mqtt_connected = condition_check->IsInternetState();
#endif
                std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(5));
            }
            catch(...) {
                LOGERROR << "VirtualDiagnostics::RequestPublickey: request public key error.";
            }
        }

        if(is_private_mqtt_connected) {
            LOGINFO << "VirtualDiagnostics::RequestPublickey: private mqtt is connected,sending request message...";

            public_key_root["messageType"] = 1;
            public_key_root["protocolVersion"] = "1";
            public_key_root["timestamp"] = GetTimeStamp();
            public_key_root["taskId"] = CreateTaskId();
            public_key_root["customParameter"];
            request_json = public_key_root.toStyledString();
            LOGINFO << "VirtualDiagnostics::RequestPublickey: create request public key json string: " << request_json.c_str();
            SendDiagData(request_json);
        }
        else {
            LOGERROR << "VirtualDiagnostics::RequestPublickey: private mqtt is not connected,can not request public key.";
        }
    }
}
#endif

void VirtualDiagnostics::SendDiagData(const std::string& data)
{
    const bool reconnect = false;
    vc_proxy_instance->ReportDiagnosisData(data, reconnect);
}

time_t VirtualDiagnostics::GetTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp = 
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    return tp.time_since_epoch().count();
}

bool VirtualDiagnostics::GetVinData(std::string &data)
{
#ifdef VEHICLE_CONFIG
    //虚拟诊断仪启动后可能向云端获取公钥，创建taskid时获取vin码可能获取不到，因为这时候对端还没有给虚拟诊断仪发送vin码
    //如果一定要获取vin码成功，这里可以改成while循环，一直等待获取vin码成功
    if (vcm_proxy_instance->Valid()){
        if(global_stop_flag) return false;

            data = vcm_proxy_instance->GetVin();
            LOGINFO << "VirtualDiagnostics::GetVinData: get vin: " << data.c_str();
            return true;
    } else {
            LOGERROR << "VirtualDiagnostics::GetVinData: not received vin data!";
            return false;
    }
#else
   return false;
#endif
}

std::string VirtualDiagnostics::CreateTaskId()
{
    Json::Value value;
    Json::Reader reader;
    std::ifstream ifs;
    std::string task_id = "C";
    std::string vin = "";
    std::string timestamp;
    std::string random_num;

    bool get_vin_result = false;
    get_vin_result = GetVinData(vin);
    if(get_vin_result) {
        if(vin == "") {
            vin = "00000000000000000";
            LOGERROR << "VirtualDiagnostics::CreateTaskId: get vin failed.";
        }
        LOGINFO << "VirtualDiagnostics::CreateTaskId: get vin: " << vin.c_str();
    }
    else {
        //vin = "00000000000000000";
        //联调时VIN码先写死
        vin = "VIN00000000000910";
        LOGERROR << "VirtualDiagnostics::CreateTaskId: get vin failed.";
    }
    //vin = "A0220221208057777";

    //获取当前时间戳字符串
    time_t current_time;
    std::string year = "00";
    std::string month = "00";
    std::string day = "00";
    std::string hour = "00";
    std::string min = "00";
    std::string sec = "00";

    time(&current_time);
    tm *tm = localtime(&current_time);
    year = std::to_string(1900 + tm->tm_year);
    if((1 + tm->tm_mon) < 10) {
        month = "0" + std::to_string(1 + tm->tm_mon);
    }
    else {
        month = std::to_string(1 + tm->tm_mon);
    }
    if(tm->tm_mday < 10) {
        day = "0" + std::to_string(tm->tm_mday);
    }
    else {
        day = std::to_string(tm->tm_mday);
    }
    if(tm->tm_hour < 10) {
        hour = "0" + std::to_string(tm->tm_hour);
    }
    else {
        hour = std::to_string(tm->tm_hour);
    }
    if(tm->tm_min < 10) {
        min = "0" + std::to_string(tm->tm_min);
    }
    else {
        min = std::to_string(tm->tm_min);
    }
    if(tm->tm_sec < 10) {
        sec = "0" + std::to_string(tm->tm_sec);
    }
    else {
        sec = std::to_string(tm->tm_sec);
    }
    timestamp = year + month + day + hour + min + sec;
    LOGINFO << "VirtualDiagnostics::CreateTask: get timestamp: " << timestamp.c_str();

    //随机生成4位数字符串
    srand(static_cast<unsigned>(time(nullptr)));
    random_num = std::to_string(1000 + rand()%9000);

    task_id = task_id + vin + timestamp + random_num;
    LOGINFO << "VirtualDiagnostics::CreateTask: get task id: " << task_id.c_str();
    return task_id;
}

#ifdef LOCAL_DIAG_CLIENT
void VirtualDiagnostics::DynamicLocalDiagService()
{
    while(LOCAL_DIAG_CLIENT_RUNNING) {
        //收到退出信号，结束线程
        if(global_stop_flag){
            break;
        }
        // kevin auth 
        LOGDEBUG << "VirtualDiagnostics::DynamicLocalDiagService: authentication state: " << authentication_state_instance->authentication_state
                 << "local diag service init flag: " << local_diag_service_init_flag_;

        if(authentication_state_instance->authentication_state == midware::diag::AuthenticationState::kSuccess ||
           authentication_state_instance->authentication_state == midware::diag::ReportFactoryMode::kFactoryMode){
            if(!local_diag_service_init_flag_) {
                LOGINFO << "VirtualDiagnostics::DynamicLocalDiagService: offer service 0x8015.";

                //初始化8015服务
                dm_local_diag_client_instance_->Init();

                //记录工厂模式以及服务offer情况
                if(authentication_state_instance->authentication_state == midware::diag::ReportFactoryMode::kFactoryMode){
                    is_in_factory_mode_ = true;
                }
                local_diag_service_init_flag_ = true;
            }
        }
        else if(authentication_state_instance->authentication_state == midware::diag::AuthenticationState::kDoipDiagProcessing){
            if(!is_in_factory_mode_){
                if(local_diag_service_init_flag_){
                    if(dm_local_diag_client_instance_){
                        LOGERROR << "VirtualDiagnostics::DynamicLocalDiagService: stop service 0x8015.";
                        dm_local_diag_client_instance_->Release();
                        local_diag_service_init_flag_ = false;
                    }
                }
            }
        }
        else{
            if (authentication_state_instance->authentication_state == midware::diag::AuthenticationState::kInitial ||
                authentication_state_instance->authentication_state == midware::diag::ReportFactoryMode::kInitialMode) {
                LOGDEBUG << "VirtualDiagnostics::DynamicLocalDiagService: authentication status initialization,or exit factory mode.";
                is_in_factory_mode_ = false;
            }

            if (local_diag_service_init_flag_) {
                if(dm_local_diag_client_instance_){
                    LOGERROR << "VirtualDiagnostics::DynamicLocalDiagService: stop service 0x8015.";
                    dm_local_diag_client_instance_->Release();
                    local_diag_service_init_flag_ = false;
                }
            }
        }
        
        std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
    }
}
#endif

void VirtualDiagnostics::InitVehicleData()
{
    SystemCmd file_opreation_instance;
    std::ifstream vehicle_data_file;
    Json::Reader reader;
    Json::Value root;
    int create_dir_result = -1;

    if (access(VEHICLE_DATA_FILE_BACKUP_FOLDER, 0) == -1) {
        LOGINFO << "VirtualDiagnostics::InitVehicleData: /shared/VehicleData/ floder does not exist,creating...";
        create_dir_result = mkdir(VEHICLE_DATA_FILE_BACKUP_FOLDER, 0777);
        if(create_dir_result != 0) {
            LOGERROR << "VirtualDiagnostics::InitVehicleData: create /shared/VehicleData/ floder failed,result: " << create_dir_result;
            LOGERROR << "VirtualDiagnostics::InitVehicleData: create /shared/VehicleData/ floder failed,reason: " << strerror(errno);
            return;
        }
    }

    //当/shared/VehicleData/vehicle_data.json存在时，复制到脚本解析指定目录下
    if (access(VEHICLE_DATA_FILE_BACKUP_PATH, 0) == -1) {
        LOGERROR << "VirtualDiagnostics::InitVehicleData: /shared/VehicleData/vehicle_data.json file does not exist,can not initialize.";
        return;
    }
    else {
        vehicle_data_file.open(VEHICLE_DATA_FILE_BACKUP_PATH);
        if (!reader.parse(vehicle_data_file, root, false)) {
            LOGERROR << "VirtualDiagnostics::InitVehicleData: parse /shared/VehicleData/vehicle_data.json file failed!";
        }
        else {
            file_opreation_instance.deleteFile(VEHICLE_DATA_FILE_PATH);
            file_opreation_instance.copy_file(VEHICLE_DATA_FILE_BACKUP_PATH,VEHICLE_DATA_FILE_FLODER);
            LOGINFO << "VirtualDiagnostics::InitVehicleData: copy vehicle_data.json finish.";
        }
        vehicle_data_file.close();
    }
}

#ifdef USING_MODEL

void VirtualDiagnostics::InitScriptParseFloder()
{

    int create_dir_result = -1;

    if (access(DSCR_FLODER, 0) == -1) {
        LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: /data/DiagnosticScript/ floder does not exist,creating...";
        create_dir_result = mkdir(DSCR_FLODER, 0777);
        if(create_dir_result != 0) {
            LOGERROR << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/ floder failed,result: " << create_dir_result; 
        }
        else{
            LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/ floder successful.";
        }
    }

    if (access(SCRIPT_FLODER, 0) == -1) {
        LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: /data/DiagnosticScript/scriptFile/ floder does not exist,creating...";

        create_dir_result = mkdir(SCRIPT_FLODER, 0777);
        if (create_dir_result != 0) {
            LOGERROR << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/scriptFile/ floder failed,result: " << create_dir_result;
        }
        else {
            LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/scriptFile/ floder successful.";
        }
    }

    if (access(TEMP_SCRIPT_FOLDER, 0) == -1) {
        LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: /data/DiagnosticScript/scriptFile/temporary/ floder does not exist,creating...";

        create_dir_result = mkdir(TEMP_SCRIPT_FOLDER, 0777);
        if (create_dir_result != 0) {
            LOGERROR << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/scriptFile/temporary/ floder failed,result: " << create_dir_result;
        }
        else {
            LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/scriptFile/temporary/ floder successful.";
        }
    }

    if (access(LOCAL_SCRIPT_FILE_FOLDER, 0) == -1) {
        LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: /data/DiagnosticScript/scriptFile/fixed/ floder does not exist,creating...";

        create_dir_result = mkdir(LOCAL_SCRIPT_FILE_FOLDER, 0777);
        if (create_dir_result != 0) {
            LOGERROR << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/scriptFile/fixed/ floder failed,result: " << create_dir_result;
        }
        else {
            LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/scriptFile/fixed/ floder successful.";
        }
    }

    if (access(DIAG_RESULT_FILE_FLODER,0) == -1) {
        LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: /data/DiagnosticScript/result/ floder does not exist,creating...";

        create_dir_result = mkdir(DIAG_RESULT_FILE_FLODER, 0777);
        if (create_dir_result != 0) {
            LOGERROR << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/result/ floder failed,result: " << create_dir_result;
        }
        else {
            LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/result/ floder successful.";
        }
    }

    if (access(VEHICLE_DATA_FILE_FLODER,0) == -1) {
        LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: /data/DiagnosticScript/vehicleData/ floder does not exist,creating...";

        create_dir_result = mkdir(VEHICLE_DATA_FILE_FLODER, 0777);
        if (create_dir_result != 0) {
            LOGERROR << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/vehicleData/ floder failed,result: " << create_dir_result;
        }
        else {
            LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/vehicleData/ floder successful.";
        }
    }

    if (access(VEHICLE_CONFIG_FOLDER, 0) == -1) {
        LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: /data/DiagnosticScript/vehicleConfig/ floder does not exist,creating...";

        create_dir_result = mkdir(VEHICLE_CONFIG_FOLDER, 0777);
        if (create_dir_result != 0) {
            LOGERROR << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/vehicleConfig/ floder failed,result: " << create_dir_result);
        }
        else {
            LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/vehicleConfig/ floder successful.";
        }
    }

    if(access(VEHICLE_VERSION_FOLDER,0) == -1) {
        LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: /data/DiagnosticScript/VehicleVersion/ floder does not exist,creating...";

        create_dir_result = mkdir(VEHICLE_VERSION_FOLDER, 0777);
        if (create_dir_result != 0) {
            LOGERROR << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/VehicleVersion/ floder failed,result: " << create_dir_result;
        }
        else {
            LOGINFO << "VirtualDiagnostics::InitScriptParseFloder:create /data/DiagnosticScript/VehicleVersion/ floder successful.";
        }
    }

    if (access(CONDITION_CHECK_FLODER, 0) == -1) {
        LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: /data/DiagnosticScript/conditioncheck/ floder does not exist,creating...";

        create_dir_result = mkdir(CONDITION_CHECK_FLODER, 0777);
        if (create_dir_result != 0) {
            LOGERROR << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/conditioncheck/ floder failed,result: " << create_dir_result;
        }
        else {
            LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/conditioncheck/ floder successful.";
        }
    }

    if (access(ODX_FLODER, 0) == -1) {
        LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: /data/DiagnosticScript/Odx/ floder does not exist,creating...";

        create_dir_result = mkdir(ODX_FLODER, 0777);
        if (create_dir_result != 0) {
            LOGERROR << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/Odx/floder failed,result: " << create_dir_result;
        }
        else {
            LOGINFO << "VirtualDiagnostics::InitScriptParseFloder: create /data/DiagnosticScript/Odx/ floder successful.";
        }
    }
}
#endif

} // namespace vdi
} // namesapce asf