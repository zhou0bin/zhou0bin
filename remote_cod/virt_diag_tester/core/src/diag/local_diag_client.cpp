/*****************************************************************************
* @file     vehicle_cloud_proxy.cpp
* @brief    Vehicle Cloud Proxy 
* @date     2025/4/23
*****************************************************************************/
#ifdef LOCAL_DIAG_CLIENT
#include "diag/local_diag_client.h"

#include "common/macro.h"
#include "diag/diag_file.h"
#include "diag/file_adapter.h"

#ifdef HTTP_WRAPPER
using FileAdapter = asf::vdi::FileAdapter;
#endif

std::shared_ptr<LocalDiagClient> LocalDiagClient::my_instance = nullptr;
std::mutex LocalDiagClient::my_mutex;

std::shared_ptr<LocalDiagClient> LocalDiagClient::GetInstance()
{
    std::unique_lock<std::mutex> lock(my_mutex);
    if (my_instance == nullptr) {
#ifdef NEUSAR_AP_R2111
        my_instance = std::make_shared<LocalDiagClient>(ara::com::InstanceIdentifier(1, ara::com::internal::MiddlewareType::VSOMEIP_BINDING),
                                                      ara::com::MethodCallProcessingMode::kEventSingleThread);
#else
        my_instance = std::make_shared<LocalDiagClient>(ara::com::InstanceIdentifier(1),
                                                      ara::com::MethodCallProcessingMode::kEventSingleThread);
#endif
    }
    return my_instance;
}

void LocalDiagClient::Init()
{
    //初始化8015服务filed
    LOGINFO << "init localDiagExcuteResult filed.";
    this->localDiagExcuteResult.Update("");

    //初始化8015服务
    LOGINFO << "Offer service:LocalDiagClient(8015).";
    this->OfferService();
}

void LocalDiagClient::Release()
{
    this->StopOfferService();
}

auto LocalDiagClient::receiveLocalDiagData(const ::ara::com::vsomeip::DIAGNOSISDOWNDATA& receiveLocalDiagnosisData)
                                                           ->decltype(receiveLocalDiagData(receiveLocalDiagnosisData))
{
    LocalDiagClient::receiveLocalDiagDataOutput output;
    Json::Reader reader;
    Json::Reader reader2;
    Json::Value value;
    Json::Value custom_parameter;
    std::string signaling_content = "";
    std::string custom_parameter_string = "";
    DiagFile diag_file_instance;

    //分段打印信令
    int signaling_index;
    int max_log_length = 1000;
    std::string subsection_string;

    signaling_index = static_cast<int>(receiveLocalDiagnosisData.size()) / max_log_length;
    if(static_cast<int>(receiveLocalDiagnosisData.size()) > max_log_length){
        LOGINFO << "get local signaling:";

        for(int i = 1; i <= signaling_index; i++){
            for(int j = max_log_length * (i-1); j < max_log_length * i; j++){
                subsection_string.push_back(receiveLocalDiagnosisData[j]);
            }
            LOGINFO << "index: " << i << " ,content: " << subsection_string.c_str();
            subsection_string.clear();
        }

        if((receiveLocalDiagnosisData.size() % max_log_length) != 0){
            int residual = receiveLocalDiagnosisData.size() % max_log_length;
            for(int k = max_log_length * signaling_index; k < max_log_length * signaling_index + residual; k++){
                subsection_string.push_back(receiveLocalDiagnosisData[k]);
            }
            LOGINFO <<  "index: " << signaling_index + 1 << " ,content: " << subsection_string.c_str();
        } 
    }
    else{
        LOGINFO << "get cloud signaling: " << receiveLocalDiagnosisData.c_str();
    }

    //解析云端信令
    if(reader.parse(receiveLocalDiagnosisData.c_str(), value)){
        //获取信令中的主要key的信息,根据messageType执行相应的操作
        size_t message_type = value["messageType"].asUInt();
        std::string protocol_version = value["protocolVersion"].asString();
        std::string task_id = value["taskId"].asString();
        custom_parameter_string = value["customParameter"].asString();

        LOGINFO <<  "parse local signaling successful,message type: " << message_type << "processing...";

        //messageType = 100，诊断请求
        if(message_type == 100){
            LOGINFO << "message type:100,diagnostic request,processing...";

            if(reader2.parse(custom_parameter_string,custom_parameter)){
                std::string package_name = custom_parameter["packageName"].asString();
                std::string package_url = custom_parameter["packageUrl"].asString();
                std::string result_url = custom_parameter["resultUrl"].asString();
                std::string encrypt_file_sha256 = custom_parameter["encryptFileSha256"].asString();
                std::string key0 = custom_parameter["key0"].asString();
                size_t temp_or_fixed = custom_parameter["tempOrFixed"].asUInt();
                size_t task_timeout = custom_parameter["taskTimeout"].asUInt();
                size_t upload_period = custom_parameter["uploadPeriod"].asUInt();
                bool state_upload = custom_parameter["stateUpload"].asBool();
                bool interact_script = custom_parameter["interactScript"].asBool();

                //如信令中含有scriptParameter，写入到/DiagnosticScript/vehicleData/script_parameter.json
                Json::Value script_parameter = custom_parameter["scriptParameter"];
                if(script_parameter.size() != 0){
                    LOGINFO << "writing script parameter...";

                    std::string script_parameter_string = script_parameter.toStyledString();
                    std::ofstream script_parameter_file;
                    script_parameter_file.open(SCRIPT_PARAMETER_FILE_PATH);
                    if(script_parameter_file.is_open()){
                        script_parameter_file << script_parameter_string;
                        LOGINFO << "writing script parameter finish";
                    }
                    else{
                        LOGERROR << "writing script parameter failed";
                    }
                    script_parameter_file.close();
                }

                //根据tempOrFixed执行云端脚本或本地脚本
                if(temp_or_fixed == 0){
                    std::thread diag_task_process_thread(&LocalDiagClient::OnBoardDiagTaskProcess,this,
                                                         protocol_version,task_id,package_name,package_url,
                                                         result_url,task_timeout,interact_script,state_upload,
                                                         upload_period,encrypt_file_sha256,key0);
                    diag_task_process_thread.detach();
                }
                else{
                    std::thread diag_task_process_thread(&LocalDiagClient::PresetDiagTaskProcessForLocal,this);
                    diag_task_process_thread.detach();
                }
            }
            else{
                LOGERROR << "parse custom parameter failed!";
            }
        }
        //messageType = 103，诊断终止
        else if(message_type == 103){
            LOGINFO << "message type:103,stop diagnostic,processing...";

            if(reader.parse(custom_parameter_string,custom_parameter)){
                std::string package_name = custom_parameter["packageName"].asString();
                size_t task_timeout = custom_parameter["taskTimeout"].asUInt();

                //终止脚本执行操作
                std::thread stop_diag_thread(&LocalDiagClient::StopDiag,this,protocol_version,
                                             task_id,package_name,task_timeout);
                stop_diag_thread.detach();
            }
            else{
                LOGERROR << "parse custom parameter failed!";
            }
        }
        else if(message_type == 106){
            LOGINFO << "message type:106, diag interaction response.";

            if(reader.parse(custom_parameter_string,custom_parameter)){
                //将云端交互信息传递给脚本解析模块
                std::string package_name = custom_parameter["packageName"].asString();
                Json::Value interact_parameter = custom_parameter["interactResponseParameter"];

                std::thread send_interact_response_parameter_thread(&LocalDiagClient::SendInteractResponseParameter,
                                                                    this,protocol_version,task_id,package_name,
                                                                    interact_parameter);
                send_interact_response_parameter_thread.detach();
            }
            else{
                LOGERROR << "parse custom parameter failed!";
            }
        }
        else if(message_type == 108){
            LOGINFO << "message type:108,request vehicle data.";

            std::thread response_vehicle_data_thread(&LocalDiagClient::ResponseVehicleData,this,
                                                     protocol_version,task_id);
            response_vehicle_data_thread.detach();
        }
        //messageType = 200,诊断执行条件参数修改请求
        else if(message_type == 200){
            LOGINFO << "message type:200,modfy condition parameter.";

            if(reader.parse(custom_parameter_string,custom_parameter)){
                Json::Value script_run_condition = custom_parameter["scriptRunCondition"];

                //执行修改操作
                std::thread modify_parameters_thread(&LocalDiagClient::ModifyParameters,this,
                                                     protocol_version,task_id,script_run_condition);
                modify_parameters_thread.detach();
            }
            else{
                LOGERROR << "parse custom parameter failed!";
            }
        }
        //messageType = 202,诊断引擎时间参数修改请求
        else if(message_type == 202){
            LOGINFO << "message type:202,modify diagnostic engine time parameters.";

            if(reader.parse(custom_parameter_string,custom_parameter)){
                size_t p2Client = custom_parameter["diagEngine"]["p2Client"].asUInt();
                size_t p2StarClient = custom_parameter["diagEngine"]["p2StarClient"].asUInt();            

                //执行修改操作
                std::thread modify_engine_time_parameters_thread(&LocalDiagClient::ModifyDiagEngineTimeParameters,
                                                                 this,protocol_version,task_id,p2Client,p2StarClient);
                modify_engine_time_parameters_thread.detach();  
            }
            else{
                LOGERROR << "parse custom parameter failed!";
            }
        }
        //messageType = 400,删除本地脚本
        else if(message_type == 400){
            LOGINFO << "message type:400,delete the local diag file.";

            if(reader.parse(custom_parameter_string,custom_parameter)){
                size_t file_type = custom_parameter["fileType"].asUInt();
                std::string package_name = custom_parameter["packageName"].asString();

                //执行脚本删除操作
                std::thread delete_local_script_package_thread(&LocalDiagClient::DeleteLocalDiagFile,this,
                                                               protocol_version,task_id,file_type,package_name);
                delete_local_script_package_thread.detach();
            }
            else{
                LOGERROR << "parse custom parameter failed!";
            }
        }
        //messageType = 402,新增或更新本地文件
        else if(message_type == 402){
            LOGINFO << "message type:402,update local script or other diag files.";

            if(reader.parse(custom_parameter_string,custom_parameter)){
                std::string package_url = custom_parameter["packageUrl"].asString();
                size_t file_type = custom_parameter["fileType"].asUInt();

                //执行更新文件操作
                std::thread update_local_script_thread(&LocalDiagClient::UpdateLocalDiagFile,this,
                                                       protocol_version,task_id,package_url,file_type);
                update_local_script_thread.detach();
            }
            else{
                LOGERROR << "parse custom parameter failed!";
            }
        }
        //云端查询车端文件信息
        else if(message_type == 500){
            LOGINFO << "message type:500,query file information of the vehicle.";

            if(reader.parse(custom_parameter_string,custom_parameter)){
                size_t file_type = custom_parameter["fileType"].asUInt();
                std::string package_name = custom_parameter["packageName"].asString();

                //执行文件查询操作

                std::thread query_file_Inf_thread(&LocalDiagClient::QueryFileInf,this,
                                                  protocol_version,task_id,file_type,package_name);
                query_file_Inf_thread.detach();
            }
            else{
                LOGERROR << "parse custom parameter failed!";
            }
        }
        else{
            LOGERROR << "the corresponding message type was not identified.";
        }
    }
    else{
        LOGERROR << "parse local signaling failed!";
    }

    //返回ack
    output.receiveLocalDiagnosisResult = "OK";
    decltype(LocalDiagClientSkeleton::receiveLocalDiagData(receiveLocalDiagnosisData))::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

void LocalDiagClient::OnBoardDiagTaskProcess(std::string protocolVersion, std::string taskId, std::string packageName,
                                                std::string packageUrl, std::string resultUrl, u_int taskTimeout,
                                                bool interactScript, bool stateUpload, u_int uploadPeriod,
                                                std::string encryptFileSha256, std::string key0)
{
    Json::Value custom_parameter;
    std::string response_json;
    // bool ota_mode;

    //初始化加密模块调用
    DiagFile diag_file_instance;
    diag_file_instance.GetCryptoProvider();

    /*-----------------------------------------------------任务仲裁流程-----------------------------------------------------*/
    //判断是否有任务执行中，当任务执行中时，上报massageType101，拒绝该任务
    if(is_in_local_task){
        LOGERROR << "script parsing module is busy,can not perform this task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 229;
        custom_parameter["message"];

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
        return;
    }
    else{
        LOGINFO << "script parsing module is idle,can perform this task.";
    }

    //设置任务状态
    is_in_local_task = true;
    obd_task_script_state = ScriptState::NotExecuted;
    /* kevin
    //仲裁OTA模式
    ota_mode = condition_check->IsOtaMode();
    if(ota_mode){
        //ota模式中，仲裁远程任务结束，messageType = 101
        LOGERROR << "in ota mode,stop remote task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 226;
        custom_parameter["message"] = "in ota mode";

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);

        is_in_local_task = false;
        obd_task_script_state = ScriptState::NoTask;
        return;
    }
    else{
        LOGINFO << "not in ota mode.";
    }
    */
    if(is_received_stop_task_signal){
        LOGERROR << "receive stop signal,quit task.";
        obd_task_script_state = ScriptState::NoTask;
        is_in_local_task = false;
        is_received_stop_task_signal = false;
        return;
    }
    /*-----------------------------------------------------任务仲裁流程-----------------------------------------------------*/

    /*----------------------------------------------------脚本包下载流程----------------------------------------------------*/
    u_int download_time = 1;
    uint8_t script_download_result = 0x01;
    std::string package_path = "";

    package_path = package_path + TEMP_STORAGE_FOLDER + packageName + ".tar.gz";
    if(access(package_path.c_str(),0) == 0){
        LOGERROR << "script package exist,deleting...";
        file_opreation_instance.deleteFile(package_path);
    }
    file_opreation_instance.deleteDir(TEMP_SCRIPT_FOLDER,false);

    script_download_result = LocalDiagClient::DiagFileDownload(packageName,packageUrl);
    while(script_download_result != DownloadResult::DownloadSuccessful){
        if(download_time == 3){
            LOGERROR << "download retry 2 times,failed.";
            break;
        }

        LOGERROR << "download failed,retry...";
        file_opreation_instance.deleteFile(package_path);
        script_download_result = LocalDiagClient::DiagFileDownload(packageName,packageUrl);
        download_time++;
    }

    //下载失败响应，messageType = 101
    custom_parameter.clear();
    if(script_download_result != DownloadResult::DownloadSuccessful){
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 204;
        custom_parameter["message"] = "script package download failed.";

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);

        is_in_local_task = false;
        obd_task_script_state = ScriptState::NoTask;
        return;
    }
    //下载成功响应，messageType = 101
    else{
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 203;
        custom_parameter["message"] = "";

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
    }

    if(is_received_stop_task_signal){
        LOGERROR << "receive stop signal,quit task.";
        obd_task_script_state = ScriptState::NoTask;
        is_in_local_task = false;
        is_received_stop_task_signal = false;
        return;
    }
    /*----------------------------------------------------脚本包下载流程----------------------------------------------------*/

    /*----------------------------------------------------解密脚本包流程----------------------------------------------------*/
    std::string key1 = "";
    bool compare_result;
    std::vector<uint8_t> encrypt_file_sha256;
    std::vector<uint8_t> encrypt_file;
    std::vector<uint8_t> package_merge;
    std::vector<uint8_t> package_sig;
    std::vector<uint8_t> package;
    std::ifstream download_file;
    std::ofstream decrypt_file;
    size_t index = 0;
    char temp;

    //计算加密包的encryptFileSha256，并进行比对
    download_file.open(package_path, std::ios::binary | std::ios::in);
    if(download_file.is_open()){
        while(download_file.get(temp))
        {
            encrypt_file.push_back((uint8_t)temp);
            index++;
        }
        LOGINFO <<  "open encrypt package successful,size: " << " bytes.";
    }
    else{
        LOGERROR << "open encrypt package failed.";
    }
    download_file.close();

    encrypt_file_sha256 = diag_file_instance.CalculateFileSha256(encrypt_file);
    compare_result = diag_file_instance.CompareSha256(encrypt_file_sha256,encryptFileSha256);

    //比对sha256成功
    if(compare_result){
        LOGINFO << "encrypt file sha256 value is correct.";
    }
    //比对sha256失败，messageType = 101
    else{
        LOGERROR << "encrypt file sha256 value is incorrect!";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 206;
        custom_parameter["message"] = "compare encrypt file sha256 failed.";

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);

        is_in_local_task = false;
        obd_task_script_state = ScriptState::NoTask;
        return;
    }

    //AES解密获得script-package.merge
    key1 = diag_file_instance.ConvertKey(key0);
    #if 0
    #if (defined USING_AY5G) || (defined USING_A19G1) || (defined USING_AY2G)
    package_merge = diag_file_instance.DecryptFileByAES(encrypt_file,key1,key0);
    #else
    std::string encrypt_file_string(encrypt_file.begin(), encrypt_file.end());
    package_merge = diag_file_instance.DecryptFileByAES(encrypt_file_string,key1);
    #endif
    #endif

    //文件提取
    diag_file_instance.ExtractFile(package_merge,package_sig,package);
    LOGINFO <<  "get dencrypt package,size: " << package.size() << " bytes.";

    //输出解密后的原始脚本包文件
    index = 0;
    decrypt_file.open(package_path);
    if(decrypt_file.is_open()){
        for(auto iter : package){
            index++;
            decrypt_file << iter;
        }

        //解密成功，messageType = 101
        LOGINFO <<  "get original package,size: " << index << " bytes.";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 205;
        custom_parameter["message"];

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
    }
    else{
        //解密失败，messageType = 101
        LOGERROR << "error,open package failed.";
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 206;
        custom_parameter["message"] = "";

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);

        is_in_local_task = false;
        obd_task_script_state = ScriptState::NoTask;
        return;
    }
    decrypt_file.close();

    if(is_received_stop_task_signal){
        LOGERROR << "receive stop signal,quit task.";
        obd_task_script_state = ScriptState::NoTask;
        is_received_stop_task_signal = false;
        is_in_local_task = false;
        return;
    }
    /*----------------------------------------------------解密脚本包流程----------------------------------------------------*/

    /*----------------------------------------------------解压脚本包流程----------------------------------------------------*/
    std::string temp_script_path = TEMP_SCRIPT_FOLDER + packageName;
    diag_file_instance.DecompressAndDistributeFile(packageName,TempScriptPackage);

    if(access(temp_script_path.c_str(),0) == 0){
        LOGINFO << "package is decompressed successful.";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 211;
        custom_parameter["message"];

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
    }
    else{
        LOGERROR << "package is decompressed failed!";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 212;
        custom_parameter["message"];

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);

        is_in_local_task = false;
        obd_task_script_state = ScriptState::NoTask;
        return;
    }

    if(is_received_stop_task_signal){
        LOGERROR << "receive stop signal,quit task.";
        obd_task_script_state = ScriptState::NoTask;
        is_received_stop_task_signal = false;
        is_in_local_task = false;
        return;
    }
    /*----------------------------------------------------解压脚本包流程----------------------------------------------------*/

    /*-----------------------------------------------执行脚本前置条件判段流程-------------------------------------------------*/
    //判断是否处于OTA模式下
    /* kevin check
    ota_mode = condition_check->IsOtaMode();
    if(ota_mode){
        //ota模式中，仲裁远程任务结束，messageType = 101
        LOGERROR << "in ota mode,stop remote task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 226;
        custom_parameter["message"] = "in ota mode";

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);

        is_in_local_task = false;
        obd_task_script_state = ScriptState::NoTask;
        return;
    }
    else{
        LOGINFO << "not in ota mode.";
    }

    //调用配电模块进行智能配电操作
    condition_check->SetVTState(0x01);
    condition_check->SetIEPowerState(static_cast<uint16_t>(taskTimeout),true);
    */
    if(is_received_stop_task_signal){
        LOGERROR << "receive stop signal,quit task.";
        obd_task_script_state = ScriptState::NoTask;
        is_received_stop_task_signal = false;
        is_in_local_task = false; //signal false
        return;
    }
    /*-----------------------------------------------执行脚本前置条件判段流程-------------------------------------------------*/

    /*------------------------------------------------调用脚本解析模块执行---------------------------------------------------*/
    Json::Reader reader;
    Json::Value root;
    std::string socket_file_path;
    std::string interact_file_path;
    std::string clear_command = "rm -rf ";
    std::ifstream ifs;
    int start_result;
    int send_result;

    LOGINFO << "start to invoke the script parsing module...";
    ifs.open(SOCKET_CONFIG_FILE_PATH);
    if (!reader.parse(ifs, root, false)){
        //脚本执行模块socket连接失败，脚本启动失败，messageType = 101
        LOGINFO << "parse socket parameters file failed!";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 216;
        custom_parameter["message"] = "parse socket parameters file failed.";

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);

        is_in_local_task = false;
        obd_task_script_state = ScriptState::NoTask;
        ifs.close();
        return;
    }
    else{
        socket_file_path = root["Socket_Pip_File"].asString();
        interact_file_path = root["Interact_Pip_File"].asString();
        LOGINFO <<  "open socket file,path: " << socket_file_path.c_str() << "open interact socket file,path: " << interact_file_path.c_str();
    }
    ifs.close();

    //如脚本有交互内容，初始化交互状态,并创建交线程
    if(interactScript){
        interact_state.protocol_version = protocolVersion;
        interact_state.task_id = taskId;
        interact_state.run_type = 603;
        interact_state.is_response = false;
        interact_state.is_finish = false;
    }

    //如果存在结果文件先进行清除操作
    file_opreation_instance.deleteDir(DIAG_RESULT_FILE_FLODER,false);

    if(is_received_stop_task_signal){
        LOGERROR << "receive stop signal,quit task.";
        obd_task_script_state = ScriptState::NoTask;
        is_received_stop_task_signal = false;
        is_in_local_task = false;
        return;
    }

    //ScriptProcess* script_process_instance = ScriptProcess::GetInstance();
    start_result = script_process_instance->Start(socket_file_path.c_str());
    LOGINFO <<  "connectting script parsing module,start: " << start_result;

    //SendData data;
    data.itype = kCloud;
    data.guid = const_cast<char*>(packageName.c_str());
    send_result = script_process_instance->Send(&data);
    process_stop_request_completed = false;
    LOGINFO <<  "sending script parsing module script name: " << data.guid;
    LOGINFO <<  "sending script parsing module data,send: " << send_result;

    ErrorType process_result;
    struct timespec timer;
    clock_gettime(CLOCK_REALTIME,&timer);
    clock_t start_script_run_time = timer.tv_sec;
    clock_t current_time;
    u_int script_run_time;
    u_int recieve_count = 0;
    // u_int stop_count = 0;
    bool is_report_launch_result = false;
    //*package_name = const_cast<char*>(packageName.c_str());

    obd_task_script_state = ScriptState::Running;

    while(SCRIPT_RUNING){
        //收到退出信号，结束线程
        if(global_stop_flag){
            return;
        }
        /* kevin check
        //当处于OTA模式中时，仲裁远程任务结束，messageType = 101
        if(condition_check->IsOtaMode()){
            LOGERROR << "in ota mode,stop the remote task.";
            custom_parameter.clear();
            custom_parameter["runType"] = 603;
            custom_parameter["runState"] = 226;
            custom_parameter["message"] = "in ota mode,stop remote task.";

            response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
            LocalDiagClient::SendDiagData(response_json);

            //停止脚本运行
            data.itype = kStop;
            data.guid = const_cast<char*>(packageName.c_str());
            send_result = script_process_instance->Send(&data);
            LOGINFO <<  "sending script parsing module stop message,send: " << send_result;

            while(SCRIPT_STOPPING){
                //收到退出信号，结束线程
                if(global_stop_flag){
                    return;
                }

                std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(2));
                process_result = script_process_instance->ReceiveDatas(packageName.c_str());

                if(process_result != KStopSuccess){
                    if(process_result == kStoping){
                        clock_gettime(CLOCK_REALTIME,&timer);
                        current_time = timer.tv_sec;
                        script_run_time = (u_int)(current_time - start_script_run_time);
                        if(script_run_time >= taskTimeout){
                            LOGERROR << "stop script failed,quit.";
                            break;
                        }
                        else{
                            LOGINFO << "script is stopping...";
                            continue; 
                        }
                    }
                    if(stop_count == 2){
                        LOGERROR << "stop script unknown error.";
                        stop_count = 0;
                        break;
                    }
                    LOGINFO << "stoping script,state: " << process_result;
                    stop_count++;
                }
                else{
                    LOGINFO << "stop script running successful.";
                    break;
                }
            }
            break;
        }
        */
        //监听脚本执行状态
        //判断是否超时
        clock_gettime(CLOCK_REALTIME,&timer);
        current_time = timer.tv_sec;
        script_run_time = (u_int)(current_time - start_script_run_time);

        LOGINFO <<  "script running spend times: " << script_run_time;
        if(script_run_time >= taskTimeout){
            //脚本运行超时，停止脚本运行
            data.itype = kStop;
            data.guid = const_cast<char*>(packageName.c_str());
            send_result = script_process_instance->Send(&data);
            LOGINFO <<  "sending script parsing module stop message,send: " << send_result;

            std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(3));
            process_result = script_process_instance->ReceiveDatas(packageName.c_str());

            if(process_result == KStopSuccess){
                //脚本停止成功，messageType = 101
                LOGERROR << "script running timeout,stop task successful.";
                custom_parameter.clear();
                custom_parameter["runType"] = 603;
                custom_parameter["runState"] = 224;
                custom_parameter["message"] = "script running times out,stop task successful.";

                response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
                LocalDiagClient::SendDiagData(response_json);
            }
            else{
                //脚本停止失败，messageType = 101
                LOGERROR << "script running timeout,stop task failed.";
                custom_parameter.clear();
                custom_parameter["runType"] = 603;
                custom_parameter["runState"] = 225;
                custom_parameter["message"] = "script running times out,stop task failed";

                response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
                LocalDiagClient::SendDiagData(response_json);
            }
            break;
        }

        std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
        process_result = script_process_instance->ReceiveDatas(packageName.c_str());
        LOGINFO <<  "script running state: " << process_result;

        /*
        kTooMany         = 0X01,//request queue is full
        kExcutedOver     = 0X02,//script had executed
        kExcutedFaile    = 0X03,//script had not executed
        kNoscript        = 0X04,//find no script
        kNoZIP           = 0X05,//the scriptzip does not exist
        kCONDNOTALLOW    = 0X06,//conditions do not allow
        kDECFAILE        = 0X07,//reserved fields: "decompression failed"
        KStopSuccess     = 0x08,//stop script
        KFindNoScriptTh  = 0x09,//find no script thread
        KScriptRunning   = 0x0A,//script is running
        KUnKnown         = 0x0B//unknown err
        */
        if(process_result == kExcutedOver){
            //脚本执行完成，messageType = 101
            LOGINFO << "script execute over.";
            custom_parameter.clear();
            custom_parameter["runType"] = 603;
            custom_parameter["runState"] = 218;
            custom_parameter["message"] = "script execute over.";

            response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
            LocalDiagClient::SendDiagData(response_json);
            break;
        }
        else if(process_result == kCONDNOTALLOW){
            //获取条件检测失败信息
            std::string condition_json = "";
            Json::Value result;
            Json::Value conditions;

            ifs.open(DIAG_RESULT_JSON_FILE_PATH);
            if(ifs.is_open()){
                if (reader.parse(ifs, result)){
                    conditions = result["Result"]["ConditionChecks"];
                    condition_json = conditions.toStyledString();
                }
                else{
                    LOGERROR << "can not parse DiagResult.json.";
                }
            }
            else{
                LOGERROR << "open DiagResult.json failed.";
            }
            ifs.close();

            //脚本执行条件不允许，messageType = 101
            LOGERROR << "script condition check failed.";
            custom_parameter.clear();
            custom_parameter["runType"] = 603;
            custom_parameter["runState"] = 215;
            custom_parameter["message"] = condition_json;

            response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
            LocalDiagClient::SendDiagData(response_json);
            break;
        }
        else if(process_result == KScriptRunning){
            //以running状态判断脚本是否启动成功，messageType = 101 
            if((recieve_count == 0) && is_report_launch_result == false){
                LOGINFO << "script launch successful.";
                custom_parameter.clear();
                custom_parameter["runType"] = 603;
                custom_parameter["runState"] = 213;
                custom_parameter["message"] = "";

                response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
                LocalDiagClient::SendDiagData(response_json);
                is_report_launch_result = true;
            }

            recieve_count++;
            LOGINFO << "script is running.";
            if((stateUpload) && (recieve_count == uploadPeriod)){
                //响应脚本执行进度，messageType = 101
                custom_parameter.clear();
                custom_parameter["runType"] = 603;
                custom_parameter["runState"] = 217;
                custom_parameter["message"] = "";

                response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
                LocalDiagClient::SendDiagData(response_json);
                recieve_count = 0;
            }
        }
        else if(process_result == kStoping){
            LOGINFO << "script is stopping...";
            continue;
        }
        else if(process_result == KStopSuccess){
            LOGINFO << "stop script running successful.";
            script_stop_success_flag = true;
            break;
        }
        else{
            //脚本已从103信令处停止
            if(process_stop_request_completed){
                LOGINFO << "script has stopped,do not need to get the status.";
                break;
            }

            //脚本执行未知错误，messageType = 101
            LOGERROR << "script running unknow error,state: " << (int)process_result;
            custom_parameter.clear();
            custom_parameter["runType"] = 603;
            custom_parameter["runState"] = 216;
            custom_parameter["message"] = "unknow error";

            response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
            LocalDiagClient::SendDiagData(response_json);
            break;
        }
    }
    script_process_instance->Stop();
    obd_task_script_state = ScriptState::Finish;

    //重置脚本交互状态
    if(interactScript){
        interact_state.task_id = "";
        interact_state.protocol_version = "";
        interact_state.sequence = 0;
        interact_state.is_response = false;
        interact_state.is_finish = true;
    }
    /* kevin check
    //申请智能配电下电
    condition_check->SetIEPowerState(0,false);
    condition_check->SetVTState(0x00);
    */
    LOGINFO << "invoke the script parsing module finish.";
    /*------------------------------------------------调用脚本解析模块执行---------------------------------------------------*/

    /*---------------------------------------------------打包脚本包流程-----------------------------------------------------*/
    //将脚本解析模块生成的三个结果文件打包
    if(access(DIAG_RESULT_FILE_PATH,0) == 0){
        LOGINFO << "DiagResult.tar.gz file exist,deleting...";
        file_opreation_instance.deleteFile(DIAG_RESULT_FILE_PATH);
        if(access(DIAG_RESULT_FILE_PATH,0) == -1){
            LOGINFO << "DiagResult.tar.gz file delete successful.";
        }
        else{
            LOGERROR << "DiagResult.tar.gz file delete failed.";
        }
    }

    diag_file_instance.CompressPackage("DiagResult");
    if(access(DIAG_RESULT_FILE_PATH,0) == -1){
        //结果文件打包失败，messageType = 101
        LOGERROR << "create diag result package failed!";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 220;
        custom_parameter["message"] = "create diag result package failed.";

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);

        is_in_local_task = false;
        obd_task_script_state = ScriptState::NoTask;
        return;
    }
    else{
        //结果文件打包成功，messageType = 101
        LOGINFO << "create diag result package successful.";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 219;
        custom_parameter["message"] = "create diag result package successful.";

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
    }
    /*---------------------------------------------------打包脚本包流程-----------------------------------------------------*/

    /*--------------------------------------------------结果文件加密流程-----------------------------------------------------*/
    std::ifstream result_package_file;
    std::ofstream encrypt_result_package_file;
    std::vector<uint8_t> result_package;
    std::vector<uint8_t> encrypt_result_package;
    std::vector<uint8_t> result_package_sha256;
    std::vector<uint8_t> encrypt_result_package_sha256;
    std::stringstream ioss;
    std::string result_key0;
    std::string result_key1;
    std::string s_temp;
    std::string result_package_sha256_string = "";
    std::string encrypt_result_package_sha256_string = "";
    char tmp;

    result_package_file.open(DIAG_RESULT_FILE_PATH, std::ios::binary | std::ios::in);
    if(result_package_file.is_open()){
        index = 0;
        while(result_package_file.get(tmp))
        {
            result_package.push_back((uint8_t)tmp);
            index++;
        }
        LOGINFO <<  "open result package successful,size: " << index << " bytes";
    }
    else{
        //加密失败，messageType = 101
        LOGERROR << "open result package failed!";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 222;
        custom_parameter["message"];

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
    }
    result_package_file.close();

    //计算结果包的sha256值
    result_package_sha256 = diag_file_instance.CalculateFileSha256(result_package);
    for(auto iter : result_package_sha256){
        ioss << std::hex << (int)iter;
        ioss >> s_temp;
        if(s_temp.length() < 2){
            s_temp = "0" + s_temp;
        }
        result_package_sha256_string = result_package_sha256_string + s_temp;
        s_temp.clear();
        ioss.clear();
    }
    LOGINFO << "calculate result package sha256: " << result_package_sha256_string.c_str();

    //AES加密结果压缩包
    result_key0 = diag_file_instance.RandomKey();
    result_key1 = diag_file_instance.ConvertKey(result_key0);
    #if (defined USING_AY5G) || (defined USING_A19G1) || (defined USING_AY2G)
    encrypt_result_package = diag_file_instance.CryptFileByAES(result_package,result_key1,result_key0);
    #else
    encrypt_result_package = diag_file_instance.CryptFileByAES(result_package,result_key1);
    #endif

    //计算加密结果包的sha256值
    encrypt_result_package_sha256 = diag_file_instance.CalculateFileSha256(encrypt_result_package);
    for(auto iter : encrypt_result_package_sha256){
        ioss << std::hex << (int)iter;
        ioss >> s_temp;
        if(s_temp.length() < 2){
            s_temp = "0" + s_temp;
        }
        encrypt_result_package_sha256_string = encrypt_result_package_sha256_string + s_temp;
        s_temp.clear();
        ioss.clear();
    }
    LOGINFO <<  "calculate encryption result package sha256: " << encrypt_result_package_sha256_string.c_str();

    //输出加密的结果包文件
    index = 0;
    encrypt_result_package_file.open(DIAG_RESULT_FILE_PATH);
    if(encrypt_result_package_file.is_open()){
        for(auto iter : encrypt_result_package){
            index++;
            encrypt_result_package_file << iter;
        }
        encrypt_result_package_file.close();

        //加密成功，messageType = 101
        LOGINFO << "encrypto result package successful.";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 221;
        custom_parameter["message"];

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
    }
    else{
        //加密失败，messageType = 101
        LOGERROR << "encrypto result package failed.";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 222;
        custom_parameter["message"];

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);

        is_in_local_task = false;
        obd_task_script_state = ScriptState::NoTask;
        return;
    }
    /*--------------------------------------------------结果文件加密流程-----------------------------------------------------*/

    /*--------------------------------------------------结果文件上传流程-----------------------------------------------------*/
    uint8_t upload_result;
    uint upload_time = 1;
    std::string result_package_path = DIAG_RESULT_FILE_PATH;

    upload_result = LocalDiagClient::DiagScriptRunResultUpload("DiagResult.tar.gz", result_package_path,resultUrl);
    while(upload_result != UPloadResult::UploadSuccessful){
        if(upload_time == 3){
            LOGERROR << "upload retry 2 times,failed.";
            break;
        }

        LOGERROR << "upload failed,retry...";
        upload_result = LocalDiagClient::DiagScriptRunResultUpload("DiagResult.tar.gz", result_package_path,resultUrl);
        upload_time++;
    }

    if(upload_result == UPloadResult::UploadSuccessful){
        //执行结果上传成功，messageType = 107
        LOGINFO << "the diag result package uploads successful.";
        custom_parameter.clear();
        custom_parameter["resultUrl"] = resultUrl;
        custom_parameter["packageSha256"] = result_package_sha256_string;
        custom_parameter["encryptFileSha256"] = encrypt_result_package_sha256_string;
        custom_parameter["key0"] = result_key0;

        response_json = LocalDiagClient::CreateJsonResponse(107,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
    }
    else{
        LOGERROR << "the diag result package uploads failed.";
        custom_parameter.clear();
        custom_parameter["runType"] = 603;
        custom_parameter["runState"] = 223;
        custom_parameter["message"] = "";

        response_json = LocalDiagClient::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
    }
    /*--------------------------------------------------结果文件上传流程-----------------------------------------------------*/

    is_in_local_task = false;
    obd_task_script_state = ScriptState::NoTask;
}

void LocalDiagClient::PresetDiagTaskProcessForLocal()
{

}

void LocalDiagClient::StopDiag(std::string protocolVersion, std::string taskId, std::string packageName, u_int taskTimeout)
{
    Json::Value custom_parameter;
    Json::Value root;
    Json::Reader reader;
    std::string response_json;
    std::string socket_file_path;
    u_int stop_count = 0;
    std::ifstream ifs;
    DiagFile diag_file_instance;

    ifs.open(SOCKET_CONFIG_FILE_PATH);
    if (!reader.parse(ifs, root, false)){
        //messageType = 104
        LOGERROR << "parse socket parameters file failed!";
        custom_parameter.clear();
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 999;

        response_json = LocalDiagClient::CreateJsonResponse(104,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
        return;
    }
    else{
        socket_file_path = root["Socket_Pip_File"].asString();
        LOGINFO << "open socket file,path: " << socket_file_path.c_str();
    }
    ifs.close();

    //根据脚本状态执行不同的停止策略
    struct timespec timer;
    clock_gettime(CLOCK_REALTIME,&timer);
    clock_t start_time = timer.tv_sec;
    clock_t current_time;
    u_int stop_time;

    if(obd_task_script_state == ScriptState::Running){
        //SendData data;
        data.itype = kStop;
        data.guid = (char*)packageName.c_str();
        size_t send_result = script_process_instance->Send(&data);
        LOGINFO << "sending script parsing module script name: " << data.guid;
        LOGINFO << "sending script parsing module data,send: " << send_result;

        script_stop_success_flag = false;

        //根据调用停止接口返回结果，messageType = 104
        ErrorType process_result;
        while(SCRIPT_STOPPING){
            //收到退出信号，结束线程
            if(global_stop_flag){
                return;
            }

            std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
            process_result = script_process_instance->ReceiveDatas(packageName.c_str());

            if(process_result != KStopSuccess){
                if(process_result == kStoping){
                    clock_gettime(CLOCK_REALTIME,&timer);
                    current_time = timer.tv_sec;
                    stop_time = (u_int)(current_time - start_time);

                    if(stop_time >= taskTimeout){
                        LOGERROR << "stop script failed,quit.";
                        custom_parameter.clear();
                        custom_parameter["result"] = false;
                        custom_parameter["failureReason"] = 999;
                        break;
                    }
                    else{
                        LOGINFO << "script is stopping...";
                        continue;
                    }
                }

                if(stop_count == 2){
                    LOGERROR << "stop script unknown error.";
                    custom_parameter.clear();
                    custom_parameter["result"] = false;
                    custom_parameter["failureReason"] = 999;
                    break;
                }

                if(script_stop_success_flag){
                    LOGINFO << "stop script running successful.";
                    custom_parameter.clear();
                    custom_parameter["result"] = true;
                    custom_parameter["failureReason"];
                    break;
                }

                LOGINFO <<  "stoping script,state: " << process_result;
                stop_count++;
            }
            else{
                LOGINFO << "stop script running successful.";
                custom_parameter.clear();
                custom_parameter["result"] = true;
                custom_parameter["failureReason"];
                break;
            }
        }
        process_stop_request_completed = true;
    }
    else if(obd_task_script_state == ScriptState::NotExecuted){
        LOGERROR << "script is not executed,stopping task...";
        is_received_stop_task_signal = true;

        custom_parameter.clear();
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 406;
        response_json = LocalDiagClient::CreateJsonResponse(104,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);

        while(obd_task_script_state != ScriptState::NoTask){
            clock_gettime(CLOCK_REALTIME,&timer);
            current_time = timer.tv_sec;
            stop_time = (u_int)(current_time - start_time);

            if(stop_time >= taskTimeout){
                LOGERROR << "stop script failed,quit.";
                custom_parameter.clear();
                custom_parameter["result"] = false;
                custom_parameter["failureReason"] = 999;
                goto RESULT;
            }
            else{
                LOGINFO << "task is stopping...";
            }

            std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
        }

        custom_parameter.clear();
        custom_parameter["result"] = true;
        custom_parameter["failureReason"];        
    }
    else if(obd_task_script_state == ScriptState::Finish){
        LOGERROR << "script execution has completed,can not be stopped.";
        custom_parameter.clear();
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 405;
    }
    else{
        LOGERROR << "there is no script task going on,it can not be stopped.";
        custom_parameter.clear();
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 999;
    }

RESULT:
    response_json = LocalDiagClient::CreateJsonResponse(104,protocolVersion,taskId,custom_parameter);
    LocalDiagClient::SendDiagData(response_json);
}

void LocalDiagClient::SendInteractResponseParameter(std::string protocolVersion, std::string taskId,
                                                       std::string packageName,Json::Value interactResponseParameter)
{
    Json::Value custom_parameter;
    std::string response_json;
    DiagFile diag_file_instance;

    LOGINFO <<  "get script: " << packageName.c_str() << "interaction response parameter.";
    LOGINFO <<  "get interaction response json: " << interactResponseParameter.toStyledString().c_str();

    std::string interact_type = interactResponseParameter["interactType"].asString();
    int result = interactResponseParameter["result"].asInt();

    //messageBox
    if(strcmp(interact_type.c_str(),"1") == 0){
        interact_state.response_parameter.interact_type = interact_type;
        interact_state.response_parameter.result = result;
    }
    //inputBox
    else if(strcmp(interact_type.c_str(),"2") == 0){
        interact_state.response_parameter.interact_type = interact_type;
        interact_state.response_parameter.result = result;
        interact_state.response_parameter.input_value.clear();
        for(u_int i = 0; i < interactResponseParameter["inputValues"].size(); i++){
            interact_state.response_parameter.input_value.
                push_back(interactResponseParameter["inputValues"][i].asString());
        }
    }
    //selectionList
    else if(strcmp(interact_type.c_str(),"3") == 0){
        interact_state.response_parameter.interact_type = interact_type;
        interact_state.response_parameter.result = result;
        interact_state.response_parameter.selected_index = interactResponseParameter["selectedIndex"].asInt();
    }
    //information
    else{
        interact_state.response_parameter.interact_type = interact_type;
    }

    interact_state.is_response = true;
    LOGINFO << "transmit interaction response parameter finish.";
}

void LocalDiagClient::ResponseVehicleData(std::string protocolVersion, std::string taskId)
{
    Json::Reader reader;
    Json::Value vehicle_data;
    Json::Value custom_parameter;
    Json::FastWriter writer;
    std::ifstream ifs;
    std::string vehicle_data_string;
    std::string response_json;
    char temp;

    if(access(VEHICLE_DATA_FILE_PATH,0) == 0){
        LOGINFO << "vehicle_data.json file exists.";
        ifs.open(VEHICLE_DATA_FILE_PATH);
        if(ifs.is_open()){
            ifs >> temp;
            if(ifs.eof()){
                LOGERROR << "vehicle_data.json is an empty file.";
                custom_parameter["runState"] = 303;
            }
            else{
                ifs.clear();
                ifs.seekg(0, std::ios::beg);

                if(reader.parse(ifs, vehicle_data)){
                    //vehicle_data_string = vehicle_data.toStyledString();
                    vehicle_data_string = writer.write(vehicle_data);
                    ifs.close();
                    LOGINFO <<  "get vehicle data : " << vehicle_data_string.c_str();
                    custom_parameter["runState"] = 301;
                    custom_parameter["vehicleData"] = vehicle_data_string;
                }
                else{
                    LOGERROR << "parse vehicle_data.json failed!";
                    custom_parameter["runState"] = 305;
                }
            }
            ifs.close();
        }
        else{
            LOGERROR << "can not open vehicle_data.json.";
            custom_parameter["runState"] = 305;
        }
    }
    else{
        LOGERROR << "vehicle_data.json file file does not exist.";
        custom_parameter["runState"] = 302;
    }

    custom_parameter["runType"] = 603;
    response_json = LocalDiagClient::CreateJsonResponse(109,protocolVersion,taskId,custom_parameter);
    LocalDiagClient::SendDiagData(response_json);
}

void LocalDiagClient::ModifyParameters(std::string protocolVersion, std::string taskId,Json::Value scriptRunCondition)
{
   Json::Value custom_parameter;
    std::string response_json;
    std::ofstream ofs;
    DiagFile diag_file_instance;

    //修改json中原有信息
    ofs.open(CONDITION_CHECK_FILE_PATH);
    if (ofs.is_open()){
        std::string json_output = scriptRunCondition.toStyledString();
        ofs << json_output;

        //修改成功，返回结果，messageType = 201
        LOGINFO << "modify parameters successful.";
        custom_parameter["result"] = true;
        custom_parameter["failureReason"];

        response_json = LocalDiagClient::CreateJsonResponse(201,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
    }
    else{
        //修改失败，返回结果，messageType = 201
        LOGERROR << "modify parameters failed.";
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 999;

        response_json = LocalDiagClient::CreateJsonResponse(201,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
    }  
}

void LocalDiagClient::ModifyDiagEngineTimeParameters(std::string protocolVersion, std::string taskId,
                                                        u_int p2Client, u_int p2StarClient)
{
    Json::Value root;
    Json::Value custom_parameter;
    std::string response_json;
    std::string parameters_file_path = "";
    parameters_file_path = parameters_file_path + TIME_PARAMETER_FOLDER + "engineTimeParameter.json";
    std::ofstream parameters_file;

    //将时间参数写入/containers/NeusarApp/work/common/opt/DiagnosticScript/engineTimeParameter/engineTimeParameter.json中
    if(access(TIME_PARAMETER_FOLDER,0) == -1){
        LOGINFO << "/engineTimeParameter file does not exist,creating...";
        mkdir(TIME_PARAMETER_FOLDER,0777);

        if(access(TIME_PARAMETER_FOLDER,0) == 0){
            LOGINFO << "/engineTimeParameter create successful.";
        }
        else{
            //参数修改失败，messageType = 203
            LOGERROR << "/engineTimeParameter create failed.";
            custom_parameter["result"] = false;
            custom_parameter["failureReason"] = 999;

            response_json = LocalDiagClient::CreateJsonResponse(203,protocolVersion,taskId,custom_parameter);
            LocalDiagClient::SendDiagData(response_json);
            return;
        }
    }

    parameters_file.open(parameters_file_path.c_str());
    if(parameters_file.is_open()){
        root["diagEngine"]["p2Client"] = p2Client;
        root["diagEngine"]["p2StarClient"] = p2StarClient;
        std::string time_parameter = root.toStyledString();
        parameters_file << time_parameter;
        parameters_file.close();

        //修改参数成功，messageType = 203
        LOGINFO << "Engine time parameter change successful.";
        custom_parameter.clear();
        custom_parameter["result"] = true;
        custom_parameter["failureReason"];

        response_json = LocalDiagClient::CreateJsonResponse(203,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
        return;
    }
    else{
        //修改参数失败，messageType = 203
        LOGERROR << "Engine time parameter change failed.";
        custom_parameter.clear();
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 999;

        response_json = LocalDiagClient::CreateJsonResponse(203,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
    }
}

void LocalDiagClient::DeleteLocalDiagFile(std::string protocolVersion, std::string taskId, u_int fileType, std::string fileName)
{
    Json::Value custom_parameter;
    std::string response_json;
    bool result = false;
    int failure_reason = 0;

    DIR* dir;
    struct dirent* dir_index;
    size_t index;
    std::string symbol = "";
    std::string file_name = "";
    std::string file_path = "";

    //根据fileType选择删除文件的路径
    /*
    1 固定脚本程序包
    2 数据库。
    3 安全算法文件
    4 车辆数据
    5 基础配置码文件
    6 版本文件。
    7 库函数。
    8 选配规则文件
    9 选配代码映射文件
    10 版本号规则文件
    */
    if(fileType == 1){
        file_path = LOCAL_SCRIPT_FILE_FOLDER + fileName;
    }
    else if(fileType == 2){
        file_path = file_path + ODX_FLODER;
    }
    else if(fileType == 3){
        result = false;
        failure_reason = 410;
        goto RESPONSE;
    }
    else if(fileType == 4){
        file_path = VEHICLE_DATA_FILE_PATH;
    }
    else if(fileType == 5){
        symbol = "MTOConfig";
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << "does not exist!";
            result = false;
            failure_reason = 411;
            goto RESPONSE;
        }
        else{
            dir = opendir(VEHICLE_CONFIG_FOLDER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                index = file_name.find(symbol);
                if(index != std::string::npos){
                    file_path = VEHICLE_CONFIG_FOLDER + file_name;
                    LOGINFO <<  "get MTOConfig file name: " << file_name.c_str();
                    break;
                }
                else{
                    file_name = "";
                }
            }

            if(dir){
                closedir(dir);
            }
        }

        if(strcmp(file_name.c_str(),"") == 0){
            LOGERROR << "can not get MTOConfig file name,file does not exist!";
            result = false;
            failure_reason = 411;
            goto RESPONSE;
        }
    }
    else if(fileType == 6){
        symbol = "ecuid_mtoc";
        if(access(VEHICLE_VERSION_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_VERSION_FOLDER << "does not exist!";
            result = false;
            failure_reason = 411;
            goto RESPONSE;
        }
        else{
            dir = opendir(VEHICLE_CONFIG_FOLDER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                index = file_name.find(symbol);
                if(index != std::string::npos){
                    file_path = VEHICLE_CONFIG_FOLDER + file_name;
                    LOGINFO <<  "get ecuid_mtoc file name: " << file_name.c_str();
                    break;
                }
                else{
                    file_name = "";
                }
            }

            if(dir){
                closedir(dir);
            }
        }

        if(strcmp(file_name.c_str(),"") == 0){
            LOGERROR << "can not get ecuid_mtoc file name,file does not exist!";
            result = false;
            failure_reason = 411;
            goto RESPONSE;
        }
    }
    else if(fileType == 7){
        result = false;
        failure_reason = 410;
        goto RESPONSE;
    }
    else if(fileType == 8){
        symbol = "OptionConfig";
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << "does not exist!";
            result = false;
            failure_reason = 411;
            goto RESPONSE;
        }
        else{
            dir = opendir(VEHICLE_CONFIG_FOLDER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                index = file_name.find(symbol);
                if(index != std::string::npos){
                    file_path = VEHICLE_CONFIG_FOLDER + file_name;
                    LOGINFO <<  "get OptionConfig file name: " << file_name.c_str();
                    break;
                }
                else{
                    file_name = "";
                }
            }

            if(dir){
                closedir(dir);
            }
        }

        if(strcmp(file_name.c_str(),"") == 0){
            LOGERROR << "can not get OptionConfig file name,file does not exist!";
            result = false;
            failure_reason = 411;
            goto RESPONSE;
        }
    }
    else if(fileType == 9){
        symbol = "OptionCodeMap";
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << "does not exist!";
            result = false;
            failure_reason = 411;
            goto RESPONSE;
        }
        else{
            dir = opendir(VEHICLE_CONFIG_FOLDER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                index = file_name.find(symbol);
                if(index != std::string::npos){
                    file_path = VEHICLE_CONFIG_FOLDER + file_name;
                    LOGINFO << "get OptionCodeMap file name: " << file_name.c_str();
                    break;
                }
                else{
                    file_name = "";
                }
            }

            if(dir){
                closedir(dir);
            }
        }

        if(strcmp(file_name.c_str(),"") == 0){
            LOGERROR << "can not get OptionConfig file name, file does not exist!";
            result = false;
            failure_reason = 411;
            goto RESPONSE;
        }
    }
    else if(fileType == 10){
        result = false;
        failure_reason = 410;
        goto RESPONSE;
    }
    else{
        LOGERROR << "no corresponding file type!";
        result = false;
        failure_reason = 410;
        goto RESPONSE;
    }

    //判断文件是否存在
    if(access(file_path.c_str(),0) != 0){
        LOGINFO <<  "delete " << file_path.c_str() << "failed, not exist!";
        result = false;
        failure_reason = 411;
        goto RESPONSE;
    }

    //执行删除脚本操作
    file_opreation_instance.deleteFile(file_path);
    if(access(file_path.c_str(),0) == 0){
        LOGERROR << "delete file failed!";
        result = false;
        failure_reason = 999;
    }
    else{
        LOGINFO << "delete file successful.";
        result = true;
        failure_reason = 0;
    }

    RESPONSE:
    //根据删除结果响应，messageType = 401
    custom_parameter["result"] = result;
    custom_parameter["failureReason"] = failure_reason;

    response_json = LocalDiagClient::CreateJsonResponse(401,protocolVersion,taskId,custom_parameter);
    LocalDiagClient::SendDiagData(response_json);
}

void LocalDiagClient::UpdateLocalDiagFile(std::string protocolVersion, std::string taskId, std::string packageUrl, u_int fileType)
{
    Json::Value custom_parameter;
    std::string response_json;
    DiagFile diag_file_instance;
    
    uint8_t script_download_result;
    script_download_result = LocalDiagClient::DiagFileDownload("TEMP",packageUrl);

    //下载失败返回失败信息，messageType = 403
    if(script_download_result != 0x00){
        LOGERROR << "download package failed.";
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 412;

        response_json = LocalDiagClient::CreateJsonResponse(403,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
        return;
    }
    else{
        LOGINFO << "download package successful.";
    }

    //根据文件类型解压分发文件
    if(fileType == 1){
        diag_file_instance.DecompressAndDistributeFile("Fixed",FixedScriptPackage);        
    }
    else if(fileType == 2){
        diag_file_instance.DecompressAndDistributeFile("Odx",Odx);
    }
    else if(fileType == 3){
        //diag_file_instance.DecompressAndDistributeFile("SA",SAFile);
    }
    else if(fileType == 5){
        diag_file_instance.DecompressAndDistributeFile("MTOConfig",MTOConfig);
    }
    else if(fileType == 8){
        diag_file_instance.DecompressAndDistributeFile("OptionConfig",OptionConfig);
    }
    else if(fileType == 9){
        diag_file_instance.DecompressAndDistributeFile("OptionCodeMap",OptionCodeMap);
    }
    else if(fileType == 10){
        diag_file_instance.DecompressAndDistributeFile("EcuidMtoc",EcuidMtoc);
    }
    else{
        LOGERROR << "unknow file type.";
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 999;

        response_json = LocalDiagClient::CreateJsonResponse(403,protocolVersion,taskId,custom_parameter);
        LocalDiagClient::SendDiagData(response_json);
        return;
    }

    //处理后响应，messageType = 403
    custom_parameter.clear();
    custom_parameter["result"] = true;
    custom_parameter["failureReason"];

    response_json = LocalDiagClient::CreateJsonResponse(403,protocolVersion,taskId,custom_parameter);
    LocalDiagClient::SendDiagData(response_json);

    LOGINFO << "update file finish.";
}

void LocalDiagClient::QueryFileInf(std::string protocolVersion, std::string taskId, u_int fileType, std::string packageName)
{
    Json::Value file_inf;
    Json::Value custom_parameter;
    DIR* dir;
    struct dirent* dir_index;
    std::ifstream fin;
    std::string response_json;
    std::string file_name = "";
    std::string version = "";
    std::string symbol;
    std::string::size_type index;

    //根据fileType获取相应版本信息
    //混合类型,查询所有可查询文件
    if(fileType == 0){
        bool is_none = true;
        int time = 0;
        //数据库
        if(access(ODX_FLODER,0) != 0){
            LOGERROR << "floder: " << ODX_FLODER << " does not exist!";
        }
        else{
            dir = opendir(ODX_FLODER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                if((strcmp(file_name.c_str(),".") != 0) && (strcmp(file_name.c_str(),"..") != 0)){
                    size_t j;
                    if(file_name.find("_V") != std::string::npos){
                        size_t index_V = file_name.find("_V" + 1);
                        j = file_name.find_first_of("_",index_V);
                        if(j != std::string::npos){
                            version = "";
                            for(size_t i=index_V; i < j; i++){
                                version.push_back(file_name[i]);
                            }
                        }
                        else{
                            version = "";
                        }
                    }
                    else{
                        if(file_name.find("_v" ) != std::string::npos){
                            size_t index_v = file_name.find("_v" + 1);
                            j = file_name.find_first_of("_",index_v);
                            if(j != std::string::npos){
                                version = "";
                                for(size_t i= index_v; i < j; i++){
                                    version.push_back(file_name[i]);
                                }
                            }
                            else{
                                version = "";
                            }
                        }
                        else{
                            version = "";
                        }
                    }

                    file_inf[time]["fileName"] = file_name;
                    file_inf[time]["version"] = version;
                    is_none = false;
                    time++;
                    LOGINFO <<  "get Odx file name: " << file_name.c_str() << "version: " << version.c_str();
                }
            }

            if(dir){
                closedir(dir);
            }

            if((strcmp(file_name.c_str(),".") == 0) || (strcmp(file_name.c_str(),"..") == 0)){
                LOGERROR << "can not get Odx file name,file does not exist!";
            }
        }

        //基础配置码文件
        symbol = "MTOConfig";
        file_name = "";
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0) {
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << " does not exist!";
        }
        else{
            dir = opendir(VEHICLE_CONFIG_FOLDER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                index = file_name.find(symbol);
                if(index != std::string::npos){
                   
                    size_t j;
                    if(file_name.find("_V") != std::string::npos){
                        size_t index_V= file_name.find("_V" + 1);
                        j = file_name.find_first_of("_",index_V);
                        if(j != std::string::npos){
                            version = "";
                            for(size_t i=index_V; i < j; i++){
                                version.push_back(file_name[i]);
                            }
                        }
                        else{
                            version = "";
                        }
                    }
                    else{
                        if(file_name.find("_v") != std::string::npos){
                            size_t index_v = file_name.find("_v" + 1);
                            j = file_name.find_first_of("_",index_v);
                            if(j != std::string::npos){
                                version = "";
                                for(size_t i=index_v; i < j; i++){
                                    version.push_back(file_name[i]);
                                }
                            }
                            else{
                                version = "";
                            }
                        }
                        else{
                            version = "";
                        }
                    }

                    file_inf[time]["fileName"] = file_name;
                    file_inf[time]["version"] = version;
                    is_none = false;
                    time++;
                    LOGINFO <<  "get MTOConfig file name: " << file_name.c_str() << " version: " << version.c_str();
                    break;
                }
                else{
                    file_name = "";
                }
            }

            if(dir){
                closedir(dir);
            }
        }

        if(strcmp(file_name.c_str(),"") == 0){
            LOGERROR << "can not get MTOConfig file name,file does not exist!";
        }

        //版本文件
        symbol = "ecuid_mtoc";
        file_name = "";
        if(access(VEHICLE_VERSION_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_VERSION_FOLDER << "does not exist!";
        }
        else{
            dir = opendir(VEHICLE_VERSION_FOLDER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                index = file_name.find(symbol);
                if(index != std::string::npos){
                    size_t j;
                    if(file_name.find("_V") != std::string::npos){
                        size_t index_V  = file_name.find("_V" + 1);
                        j = file_name.find_first_of("_",index_V);
                        if(j != std::string::npos){
                            version = "";
                            for(size_t i=index_V; i < j; i++){
                                version.push_back(file_name[i]);
                            }
                        }
                        else{
                            version = "";
                        }
                    }
                    else{
                        if(file_name.find("_v") != std::string::npos){
                            size_t index_v = file_name.find("_v" + 1);
                            j = file_name.find_first_of("_",index_v);
                            if(j != std::string::npos){
                                version = "";
                                for(size_t i=index_v; i < j; i++){
                                    version.push_back(file_name[i]);
                                }
                            }
                            else{
                                version = "";
                            }
                        }
                        else{
                            version = "";
                        }
                    }

                    file_inf[time]["fileName"] = file_name;
                    file_inf[time]["version"] = version;
                    time++;
                    is_none = false;
                    LOGINFO <<  "get ecuid_mtoc file name: " << file_name.c_str() << " version: "
                                   << version.c_str();
                    break;
                }
                else{
                    file_name = "";
                }
            }

            if(dir){
                closedir(dir);
            }
        }

        if(strcmp(file_name.c_str(),"") == 0){
            LOGERROR << "can not get ecuid_mtoc file name,file does not exist!";
        }

        //选配规则文件
        symbol = "OptionConfig";
        file_name = "";
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << " does not exist!";
        }
        else{
            dir = opendir(VEHICLE_CONFIG_FOLDER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                index = file_name.find(symbol);
                if(index != std::string::npos){
                    size_t j;
                    if(file_name.find("_V") != std::string::npos){
                        size_t index_V = file_name.find("_V" + 1);
                        j = file_name.find_first_of("_",index_V);
                        if(j != std::string::npos){
                            version = "";
                            for(size_t i=index_V; i < j; i++){
                                version.push_back(file_name[i]);
                            }
                        }
                        else{
                            version = "";
                        }
                    }
                    else{
                        if(file_name.find("_v") != std::string::npos){
                            size_t index_v = file_name.find("_v" + 1);
                            j = file_name.find_first_of("_",index_v);
                            if(j != std::string::npos){
                                version = "";
                                for(size_t i=index_v; i < j; i++){
                                    version.push_back(file_name[i]);
                                }
                            }
                            else{
                                version = "";
                            }
                        }
                        else{
                            version = "";
                        }
                    }

                    file_inf[time]["fileName"] = file_name;
                    file_inf[time]["version"] = version;
                    time++;
                    is_none = false;
                    LOGINFO <<  "get OptionConfig file name: " << file_name.c_str() << "version: " << version.c_str();
                    break;
                }
                else{
                    file_name = "";
                }
            }

            if(dir){
                closedir(dir);
            }
        }

        if(strcmp(file_name.c_str(),"") == 0){
            LOGERROR << "can not get OptionConfig file name,file does not exist!";
        }

        //选配代码映射文件
        symbol = "OptionCodeMap";
        file_name = "";
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0) {
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << " does not exist!";
        }
        else{
            dir = opendir(VEHICLE_CONFIG_FOLDER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                index = file_name.find(symbol);
                if(index != std::string::npos){
                    size_t j;
                    if(file_name.find("_V") != std::string::npos){
                        size_t index_V = file_name.find("_V" + 1);
                        j = file_name.find_first_of("_",index_V);
                        if(j != std::string::npos){
                            version = "";
                            for(size_t i=index_V; i < j; i++){
                                version.push_back(file_name[i]);
                            }
                        }
                        else{
                            version = "";
                        }
                    }
                    else{
                        if(file_name.find("_v") != std::string::npos){
                            size_t index_v = file_name.find("_v" + 1);
                            j = file_name.find_first_of("_",index_v);
                            if(j != std::string::npos){
                                version = "";
                                for(size_t i=index_v; i < j; i++){
                                    version.push_back(file_name[i]);
                                }
                            }
                            else{
                                version = "";
                            }
                        }
                        else{
                            version = "";
                        }
                    }

                    file_inf[time]["fileName"] = file_name;
                    file_inf[time]["version"] = version;
                    //time++;
                    is_none = false;
                    LOGINFO <<  "get OptionCodeMap file name: " << file_name.c_str() << "version: " << version.c_str();
                    break;
                }
                else{
                    file_name = "";
                }
            }

            if(dir){
                closedir(dir);
            }
        }

        if(strcmp(file_name.c_str(),"") == 0){
            LOGERROR << "can not get OptionCodeMap file name,file does not exist!";
        }

        //当所有文件都不存在时
        if(is_none){
            LOGERROR << "all files do not exist!";
            file_inf[time]["fileName"] = "";
            file_inf[time]["version"] = "";
        }
    }
    //固定脚本包
    else if(fileType == 1){}
    //数据库
    else if(fileType == 2){
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0) {
            LOGERROR << "floder: " << ODX_FLODER << " does not exist!";
        }
        else {
            int time = 0;
            dir = opendir(ODX_FLODER);
            while((dir_index = readdir(dir)) != nullptr) {
                file_name = dir_index->d_name;
                if((strcmp(file_name.c_str(),".") != 0) && (strcmp(file_name.c_str(),"..") != 0)){
                    size_t j;
                    if(file_name.find("_V") != std::string::npos){
                        size_t index_V = file_name.find("_V" + 1);
                        j = file_name.find_first_of("_",index_V);
                        if(j != std::string::npos){
                            version = "";
                            for(size_t i=index_V; i < j; i++){
                                version.push_back(file_name[i]);
                            }
                        }
                        else{
                            version = "";
                        }
                    }
                    else{
                        if(file_name.find("_v") != std::string::npos){
                            size_t index_v = file_name.find("_v" + 1);
                            j = file_name.find_first_of("_",index_v);
                            if(j != std::string::npos){
                                version = "";
                                for(size_t i=index_v; i < j; i++){
                                    version.push_back(file_name[i]);
                                }
                            }
                            else{
                                version = "";
                            }
                        }
                        else{
                            version = "";
                        }
                    }

                    file_inf[time]["fileName"] = file_name;
                    file_inf[time]["version"] = version;
                    time++;
                    LOGINFO <<  "get Odx file name: " << file_name.c_str() << " version: " << version.c_str();
                }
            }

            if(dir){
                closedir(dir);
            }

            if((strcmp(file_name.c_str(),".") == 0 )|| (strcmp(file_name.c_str(),"..") == 0)){
                LOGERROR << "can not get Odx file name,file does not exist!";
                file_inf[0]["fileName"] = "";
                file_inf[0]["version"] = "";
            }
        }
    }
    //安全算法文件
    else if(fileType == 3){}
    //车辆数据
    else if(fileType == 4){}
    //基础配置码文件
    else if(fileType == 5){
        symbol = "MTOConfig";
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << " does not exist!";
        }
        else{
            dir = opendir(VEHICLE_CONFIG_FOLDER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                index = file_name.find(symbol);
                if(index != std::string::npos){
                    size_t j;
                    if(file_name.find("_V") != std::string::npos){
                        size_t index_V = file_name.find("_V" + 1);
                        j = file_name.find_first_of("_",index_V);
                        if(j != std::string::npos){
                            for(size_t i=index_V; i < j; i++){
                                version.push_back(file_name[i]);
                            }
                        }
                        else{
                            version = "";
                        }
                    }
                    else{
                        if(file_name.find("_v") != std::string::npos){
                            size_t index_v = file_name.find("_v" + 1);
                            j = file_name.find_first_of("_",index_v);
                            if(j != std::string::npos){
                                for(size_t i=index_v; i < j; i++){
                                    version.push_back(file_name[i]);
                                }
                            }
                            else{
                                version = "";
                            }
                        }
                        else{
                            version = "";
                        }
                    }

                    file_inf[0]["fileName"] = file_name;
                    file_inf[0]["version"] = version;
                    LOGINFO <<  "get MTOConfig file name: " << file_name.c_str() << " version: " << version.c_str();
                    break;
                }
                else{
                    file_name = "";
                }
            }

            if(dir){
                closedir(dir);
            }
        }

        if(strcmp(file_name.c_str(),"") == 0){
            LOGERROR << "can not get MTOConfig file name, file does not exist!";
            file_inf[0]["fileName"] = "";
            file_inf[0]["version"] = "";
        }
    }
    //版本文件
    else if(fileType == 6){
        symbol = "ecuid_mtoc";
        if(access(VEHICLE_VERSION_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_VERSION_FOLDER << " does not exist!";
        }
        else{
            dir = opendir(VEHICLE_VERSION_FOLDER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                index = file_name.find(symbol);
                if(index != std::string::npos){
                    size_t j;
                    if(file_name.find("_V") != std::string::npos){
                        size_t index_V = file_name.find("_V" + 1);
                        j = file_name.find_first_of("_",index_V);
                        if(j != std::string::npos){
                            for(size_t i=index_V; i < j; i++){
                                version.push_back(file_name[i]);
                            }
                        }
                        else{
                            version = "";
                        }
                    }
                    else{
                        if(file_name.find("_v") != std::string::npos){
                            size_t index_v= file_name.find("_v" + 1);
                            j = file_name.find_first_of("_",index_v);
                            if(j != std::string::npos){
                                for(size_t i=index_v; i < j; i++){
                                    version.push_back(file_name[i]);
                                }
                            }
                            else{
                                version = "";
                            }
                        }
                        else{
                            version = "";
                        }
                    }

                    file_inf[0]["fileName"] = file_name;
                    file_inf[0]["version"] = version;
                    LOGINFO <<  "get ecuid_mtoc file name: " << file_name.c_str() << " version: " << version.c_str();
                    break;
                }
                else{
                    file_name = "";
                }
            }

            if(dir){
                closedir(dir);
            }
        }

        if(strcmp(file_name.c_str(),"") == 0){
            LOGERROR << "can not get ecuid_mtoc file name,file does not exist!";
            file_inf[0]["fileName"] = "";
            file_inf[0]["version"] = "";
        }
    }
    //库函数
    else if(fileType == 7){}
    //选配规则文件
    else if(fileType == 8){
        symbol = "OptionConfig";
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << " does not exist!";
        }
        else {
            dir = opendir(VEHICLE_CONFIG_FOLDER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                index = file_name.find(symbol);
                if(index != std::string::npos){
                    size_t j;
                    if(file_name.find("_V") != std::string::npos){
                        size_t index_V = file_name.find("_V" + 1);
                        j = file_name.find_first_of("_",index_V);
                        if(j != std::string::npos){
                            for(size_t i=index_V; i < j; i++){
                                version.push_back(file_name[i]);
                            }
                        }
                        else{
                            version = "";
                        }
                    }
                    else{
                        if(file_name.find("_v") != std::string::npos){
                            size_t index_v = file_name.find("_v" + 1);
                            j = file_name.find_first_of("_",index_v);
                            if(j != std::string::npos){
                                for(size_t i=index_v; i < j; i++){
                                    version.push_back(file_name[i]);
                                }
                            }
                            else{
                                version = "";
                            }
                        }
                        else{
                            version = "";
                        }
                    }

                    file_inf[0]["fileName"] = file_name;
                    file_inf[0]["version"] = version;
                    LOGINFO <<  "get OptionConfig file name: " << file_name.c_str() << "version: " << version.c_str();
                    break;
                }
                else{
                    file_name = "";
                }
            }

            if(dir){
                closedir(dir);
            }
        }

        if(strcmp(file_name.c_str(),"") == 0){
            LOGERROR << "can not get OptionConfig file name, file does not exist!";
            file_inf[0]["fileName"] = "";
            file_inf[0]["version"] = "";
        }
    }
    //选配代码映射文件
    else if(fileType == 9){
        symbol = "OptionCodeMap";
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << " does not exist!";
        }
        else{
            dir = opendir(VEHICLE_CONFIG_FOLDER);
            while((dir_index = readdir(dir)) != nullptr){
                file_name = dir_index->d_name;
                index = file_name.find(symbol);
                if(index != std::string::npos){
                    size_t j;
                    if(file_name.find("_V") != std::string::npos){
                        size_t index_V = file_name.find("_V" + 1);
                        j = file_name.find_first_of("_",index_V);
                        if(j != std::string::npos){
                            for(size_t i=index_V; i < j; i++){
                                version.push_back(file_name[i]);
                            }
                        }
                        else{
                            version = "";
                        }
                    }
                    else{
                        if(file_name.find("_v") != std::string::npos){
                            size_t index_v = file_name.find("_v" + 1);
                            j = file_name.find_first_of("_",index_v);
                            if(j != std::string::npos){
                                for(size_t i=index_v; i < j; i++){
                                    version.push_back(file_name[i]);
                                }
                            }
                            else{
                                version = "";
                            }
                        }
                        else{
                            version = "";
                        }
                    }

                    file_inf[0]["fileName"] = file_name;
                    file_inf[0]["version"] = version;
                    LOGINFO <<  "get OptionCodeMap file name: "<< file_name.c_str() << "version: " << version.c_str();
                    break;
                }
                else{
                    file_name = "";
                }
            }

            if(dir){
                closedir(dir);
            }
        }

        if(strcmp(file_name.c_str(),"") == 0){
            LOGERROR << "can not get OptionConfig file name,file does not exist!";
            file_inf[0]["fileName"] = "";
            file_inf[0]["version"] = "";
        }
    }
    //版本号规则文件
    else if(fileType == 10){}
    else{
        LOGERROR << "no corresponding file type!";
        file_inf[0]["fileName"] = "";
        file_inf[0]["version"] = "";
    }

    //message type = 501,文件信息请求应答
    custom_parameter["fileType"] = fileType;
    custom_parameter["fileInformation"] = file_inf;

    response_json = LocalDiagClient::CreateJsonResponse(501,protocolVersion,taskId,custom_parameter);
    LocalDiagClient::SendDiagData(response_json);
}

uint8_t LocalDiagClient::DiagFileDownload(const std::string& diagFileName, const std::string& fileUrl)
{
    /* kevin http*/
    LOGINFO << "start to download the script package...";

    //创建存放压缩包文件夹
    if(access(TEMP_STORAGE_FOLDER,0) == -1){
        LOGINFO << "/temp floder does not exist,creating...";
        mkdir(TEMP_STORAGE_FOLDER,0777);

        if(access(TEMP_STORAGE_FOLDER,0) == 0){
            LOGINFO << "/temp floder create successful";
        }
        else{
            LOGERROR << "/temp floder create failed.";
            return DownloadResult::DownloadFailed;
        }
    }

#ifdef HTTP_WRAPPER
    std::string file_path = TEMP_STORAGE_FOLDER + diagFileName + ".tar.gz";
    std::shared_ptr<FileAdapter> file_adapter = std::make_shared<FileAdapter>();
    #if (defined USING_A19G1) || (defined USING_AY5G) || (defined USING_AY2G)
    HttpClient *pCHttpExport = new HttpClient(HttpVerTypeEN::OPEN_SSL_HTTP);
    #else
    HttpClient *pCHttpExport = new HttpClient();
    #endif

    file_adapter->SetFilePath(file_path);
    pCHttpExport->SetVLan("vlan10");
    pCHttpExport->SetRequestType(Type::DOWNLOAD);
    pCHttpExport->SetUrlPath(fileUrl);
    pCHttpExport->SetFileOperatePtr(file_adapter);
    pCHttpExport->SetConnectTimeOut(300);
    pCHttpExport->SetTimeOut(600);
    pCHttpExport->SetTries(1);
    //pCHttpExport->SetIsUerLocalCertificate(true);
    pCHttpExport->Call();

    LOGINFO <<  "download response code: " << pCHttpExport->GetResponseCode();
    if(pCHttpExport->GetResponseCode() != 200 && pCHttpExport->GetResponseCode() != 206){
        LOGERROR << "download failed!";
        delete pCHttpExport;
        pCHttpExport = nullptr;
        return DownloadResult::DownloadFailed;
    }
    delete pCHttpExport;
    pCHttpExport = nullptr;
    LOGINFO << "download successful.";
#else
    return DownloadResult::DownloadFailed;
#endif
    
    return DownloadResult::DownloadSuccessful;
}

uint8_t LocalDiagClient::DiagScriptRunResultUpload(const std::string& fileName, const std::string& resultPackageNameAndPath,
                                                   const std::string& resultURL)
{
    /* kevin http */
    LOGINFO << "start to upload diag result package...";

#ifdef HTTP_WRAPPER
    std::shared_ptr<FileAdapter> file_adapter = std::make_shared<FileAdapter>();
    #if (defined USING_A19G1) || (defined USING_AY5G) || (defined USING_AY2G)
    HttpClient *pCHttpExport = new HttpClient(HttpVerTypeEN::OPEN_SSL_HTTP);
    #else
    HttpClient *pCHttpExport = new HttpClient();
    #endif
    std::map<std::string, std::string> head;
    std::map<std::string, std::string> body;

    file_adapter->SetFilePath(resultPackageNameAndPath);
    pCHttpExport->SetVLan("vlan10");
    pCHttpExport->SetRequestType(Type::UPLOADBYPUT);
    pCHttpExport->SetUrlPath(resultURL);
    pCHttpExport->SetFileOperatePtr(file_adapter);
    pCHttpExport->SetTimeOut(600);
    pCHttpExport->Call();

    LOGINFO <<  "upload response code: " << pCHttpExport->GetResponseCode();
    if(pCHttpExport->GetResponseCode() != 200){
        LOGERROR << "upload failed!";
        delete pCHttpExport;
        pCHttpExport = nullptr;
        return UPloadResult::UploadFailed;
    }
    else{
        LOGINFO << "upload successful.";
        delete pCHttpExport;
        pCHttpExport = nullptr;
        return UPloadResult::UploadSuccessful;
    }
#else
    return UPloadResult::UploadFailed;
#endif
   return UPloadResult::UploadSuccessful;
}

std::string LocalDiagClient::CreateJsonResponse(int messageType, std::string protocolVersion,
                                                std::string taskId,Json::Value customParameter)
{
    Json::Value response_root;
    time_t timestamp;
    std::string response_json;

    timestamp = LocalDiagClient::GetTimeStamp();
    response_root["messageType"] = messageType;
    response_root["protocolVersion"] = protocolVersion;
    response_root["timestamp"] = timestamp;
    response_root["taskId"] = taskId;
    response_root["customParameter"] = customParameter;

    response_json = response_root.toStyledString();
    LOGINFO <<  "create response Json string: " << response_json.c_str();
    return response_json;
}

void LocalDiagClient::SendDiagData(const std::string diagData)
{
    this->localDiagExcuteResult.Update(diagData);
}

time_t LocalDiagClient::GetTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp = 
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    long timestamp =  tp.time_since_epoch().count(); 
    return timestamp;
}
#endif