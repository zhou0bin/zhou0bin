/*****************************************************************************
* @file     vehicle_cloud_proxy.cpp
* @brief    Vehicle Cloud Proxy 
* @date     2025/4/23
*****************************************************************************/
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <cstdio>
#include <string>
#include <functional>
#include <locale>
#include <codecvt>
#include <dlfcn.h>

#include "diag/diag_client_interface.h"

#ifdef SECURITY_EVENT_WRAPPER
#include "midware/security_event_wrapper/security_event_wrapper.h"
#endif

#include "3rdparty/Seed2Key.h"
#include "3rdparty/Base64.h"
#include "common/macro.h"
#include "diag/file_adapter.h"
#include <filesystem>

#ifdef HTTP_WRAPPER
using FileAdapter = asf::vdi::FileAdapter;
#endif

std::string DM_DiagClientInterf::receiveDiagData(const std::string receiveDiagnosisData)
{
    Json::Reader reader;      
    Json::Reader reader2;
    Json::Value value;
    Json::Value custom_parameter;
    std::string signaling_content = "";
    std::string custom_parameter_string = "";
    bool check_json_result;
#ifdef AP_CRYPTO
    DiagFile diag_file_instance;
#endif

    //分段打印信令
    int signaling_index;
    int max_log_length = 1000;
    std::string subsection_string;

    signaling_index = static_cast<int>(receiveDiagnosisData.size()) / max_log_length;
    if(static_cast<int>(receiveDiagnosisData.size()) > max_log_length) {
        LOGINFO << "get cloud signaling:";

        for(int i = 1; i <= signaling_index; i++){
            for(int j = max_log_length * (i-1); j < max_log_length * i; j++){
                subsection_string.push_back(receiveDiagnosisData[j]);
            }
            LOGINFO << "index: " << i << " ,content: " << subsection_string.c_str();
            subsection_string.clear();
        }

        if((receiveDiagnosisData.size() % max_log_length) != 0){
            int residual = receiveDiagnosisData.size() % max_log_length;
            for(int k = max_log_length * signaling_index; k < max_log_length * signaling_index + residual; k++) {
                subsection_string.push_back(receiveDiagnosisData[k]);
            }
            LOGINFO <<  "index: " <<  signaling_index + 1 << " ,content: " << subsection_string.c_str();
        }
    }
    else{
        LOGINFO << "get cloud signaling: " << receiveDiagnosisData.c_str();
    }

    //解析云端信令
    if(reader.parse(receiveDiagnosisData.c_str(), value)){
        // 校验信令字段
        if(value["protocolVersion"].type() != Json::ValueType::stringValue){
            LOGERROR << "protocolVersion is not a string.";
            goto ACK;
        }

        if(value["taskId"].type() != Json::ValueType::stringValue){
            LOGERROR << "taskId is not a string.";
            goto ACK;
        }

        std::string task_id = value["taskId"].asString();
        std::string protocol_version = value["protocolVersion"].asString();

        check_json_result = check_json_key_instance.CheckJsKey(receiveDiagnosisData.c_str());
        if(!check_json_result){
            std::thread response_thread(
            [&,task_id,protocol_version]()
            {
                LOGERROR << "check keys of remote task signaling failed!";

                std::string response_json_string;
                Json::Value response;

                response["runType"] = 601;
                response["runState"] = 228;
                response_json_string = DM_DiagClientInterf::CreateJsonResponse(101,protocol_version,
                                                                               task_id,response);
                DM_DiagClientInterf::SendDiagData(response_json_string);
            });
            response_thread.detach();
            goto ACK;
        }
        else{
            LOGINFO << "check the key of remote task signaling pass.";
        }

        //获取信令中的主要key的信息,根据messageType执行相应的操作
        size_t message_type = value["messageType"].asUInt();
        long timestamp = value["timestamp"].asInt64();
        std::string sign_base64 = value["signBase64"].asString();
        // custom_parameter_string = value["customParameter"].asString();
        custom_parameter = value["customParameter"];

        //messageType = 2,下发公钥
        if(message_type == 2) {
            LOGINFO << "message type:2,deliver public key.";

            // if(reader2.parse(custom_parameter_string,custom_parameter)){
                std::string public_key = custom_parameter["publicKey"].asString();
                std::string public_key_sha256 = custom_parameter["publicKeySha256"].asString();
                std::string pki_sign_value = custom_parameter["pkiSignValue"].asString();
                std::string pki_signer = custom_parameter["pkiSigner"].asString();
                bool pki_sign = custom_parameter["pkiSign"].asBool();

                std::thread key_process_thread(&DM_DiagClientInterf::ReceiveAndSavePublicKey,this,protocol_version,
                                               task_id,public_key,public_key_sha256,pki_sign,pki_sign_value,pki_signer);
                key_process_thread.detach();
            // }
            // else{
            //     LOGERROR << "parse custom parameter failed!";
            // }
        }
        //messageType = 100，诊断请求
        else if(message_type == 100){
            LOGINFO << "message type:100, diagnostic request,processing...";

            // if(reader2.parse(custom_parameter_string,custom_parameter)) {
                std::string package_name = custom_parameter["packageName"].asString();
                std::string package_url = custom_parameter["packageUrl"].asString();
                std::string result_url = custom_parameter["resultUrl"].asString();
                // kevin 加密
                std::string encrypt_file_sha256 = custom_parameter["encryptFileSha256"].asString();
                std::string package_sha256 = custom_parameter["packageSha256"].asString();
                std::string public_key_sha256 = custom_parameter["publicKeySha256"].asString();
                std::string key0 = custom_parameter["key0"].asString();
                std::string encrypt_vehicle_data_base64 = custom_parameter["vehicleData"].asString();
                size_t package_size = custom_parameter["packageSize"].asUInt();
                size_t temp_or_fixed = custom_parameter["tempOrFixed"].asUInt();
                size_t task_timeout = custom_parameter["taskTimeout"].asUInt();
                size_t upload_period = custom_parameter["uploadPeriod"].asUInt();
                bool state_upload = custom_parameter["stateUpload"].asBool();
                bool interact_script = custom_parameter["interactScript"].asBool();
                int download_auth_mode = custom_parameter["packageUrlSsl"].asInt();
                int uploadload_auth_mode = custom_parameter["resultUrlSsl"].asInt();
                int excution_mode = custom_parameter["excutionMode"].asUInt();
                int task_priority = custom_parameter["taskPriority"].asUInt();
                int task_period = custom_parameter["taskPeriod"].asUInt();
                Json::Value condition_check_value = custom_parameter["conditionCheck"];
                bool usr_auth = false;
                if (condition_check_value.isMember("diagTaskUserAuth")) {
                    LOGINFO << "condition check: diagTaskUserAuth.";
                    usr_auth = true;
                    // Json::Value diag_task_user_auth = condition_check_value["diagTaskUserAuth"];
                    // if (diag_task_user_auth.isMember("enable")) {
                    //     usr_auth = diag_task_user_auth["enable"].asBool();
                    //     LOGINFO << "condition check: diagTaskUserAuth.enable: " << usr_auth;
                    // }
                } else {
                    LOGINFO << "condition check: no diagTaskUserAuth.";
                }
                bool gloal_auth = (user_auth_code_ == 2 || user_auth_code_ == 3);
                if(usr_auth == true && gloal_auth == false){
                    LOGINFO << "diagTaskUserAuth is enable, need to check user auth.";
                    //IDCU交互，请求用户授权并获取授权结果
                    std::unique_lock<std::mutex> lock_connect(idcu_connect_mutex_);
                    // while(idcu_client_socket_.Connect() == false){
                    //     LOGERROR << "connect to idcu failed!";
                    //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    // }
                    //消息处理是同步的，每个客户端连接处理完消息后立即关闭，所以每次发消息之前需要重连。
                    bool connect = idcu_client_socket_.Connect();
                    if(!connect){
                        LOGERROR << "connect to idcu server failed!";
                    }
                    lock_connect.unlock(); 
                    
                    LOGINFO << "connect to idcu server success!";
                    st_DiagTaskUserAuthRequest request;
                    request.taskid = task_id;
                    request.taskdesc = package_name;
                    std::string send_msg = SerializeDiagTaskUserAuthRequest(request);
                    bool send_result = idcu_client_socket_.SendMsg(send_msg);
                    uint8_t auth_code;
                    if(send_result == false){
                        LOGERROR << "send diagTaskUserAuth request to idcu failed!";
                        std::string response_json_string;
                        Json::Value response;

                        response["runType"] = 601;
                        response["runState"] = 216;//脚本启动失败-未知错误，或者再定义一个错误码代表这种情况
                        response_json_string = DM_DiagClientInterf::CreateJsonResponse(101,protocol_version,
                                                                                    task_id,response);
                        DM_DiagClientInterf::SendDiagData(response_json_string);
                    } else {
                        LOGINFO << "send diagTaskUserAuth request to idcu success!";
                        //等待idcu返回结果
                        bool recv_result = false;
                        int wait_time = 0;
                        while (!recv_result && wait_time < 300)
                        {
                            LOGINFO << "wait for diagTaskUserAuth response from idcu...";
                            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                            wait_time++;
                            if(idcu_response_map_.find(task_id) != idcu_response_map_.end())
                            {
                                LOGINFO << "receive diagTaskUserAuth response from idcu success!";
                                auth_code = idcu_response_map_.find(task_id)->second;
                                if(auth_code == 0){
                                    //此任务禁止执行
                                    LOGINFO << "auth_code is 0, this task is forbidden.";
                                    std::string response_json_string;
                                    Json::Value response;

                                    response["runType"] = 601;
                                    response["runState"] = 216;//脚本启动失败-未知错误，或者再定义一个错误码代表这种情况
                                    response_json_string = DM_DiagClientInterf::CreateJsonResponse(101,protocol_version,
                                                                                                task_id,response);
                                    DM_DiagClientInterf::SendDiagData(response_json_string);
                                }
                                if(auth_code == 2 || auth_code == 3){
                                    user_auth_code_ = auth_code;
                                }
                                recv_result = true;
                                break;
                            }
                        }
                        //等待idcu 5分钟
                        if(!recv_result){
                            LOGERROR << "wait for diagTaskUserAuth response from idcu timeout!";
                            std::string response_json_string;
                            Json::Value response;

                            response["runType"] = 601;
                            response["runState"] = 216;//脚本启动失败-未知错误，或者再定义一个错误码代表这种情况
                            response_json_string = DM_DiagClientInterf::CreateJsonResponse(101,protocol_version,
                                                                                        task_id,response);
                            DM_DiagClientInterf::SendDiagData(response_json_string);
                        }
                    }
                }

                if(user_auth_code_ == 2){
                    //本次上电周期所有任务均全部禁止执行
                    LOGINFO << "user_auth_code_ is 2, all tasks are forbidden in this power cycle.";
                    std::string response_json_string;
                    Json::Value response;

                    response["runType"] = 601;
                    response["runState"] = 216;//脚本启动失败-未知错误，或者再定义一个错误码代表这种情况
                    response_json_string = DM_DiagClientInterf::CreateJsonResponse(101,protocol_version,
                                                                                task_id,response);
                    DM_DiagClientInterf::SendDiagData(response_json_string);
                }

                //如信令中含有vehicleData信息，写入到相应的目录
                /* 加密
                this->WriteVehicleData(encrypt_vehicle_data_base64,key0);
                */

                // 如信令中含有scriptParameter，写入到/DiagnosticScript/vehicleData/script_parameter.json
                Json::Value script_parameter = custom_parameter["scriptParameter"];
                if(script_parameter.size() != 0){
                    LOGINFO << "writing script parameter...";

                    std::string script_parameter_string = script_parameter.toStyledString();
                    std::ofstream script_parameter_file;
                    script_parameter_file.open(SCRIPT_PARAMETER_FILE_PATH);
                    if(script_parameter_file.is_open()) {
                        script_parameter_file << script_parameter_string;
                        LOGINFO << "writing script parameter finish";
                    }
                    else {
                        LOGERROR << "writing script parameter failed";
                    }
                    script_parameter_file.close();
                }

                //拼接信令
                signaling_content = DM_DiagClientInterf::CombineSignalingContent("100",protocol_version,timestamp,
                                                                                 task_id,custom_parameter_string);

                //根据tempOrFixed执行云端脚本或本地脚本
                if(temp_or_fixed == 0){
                    std::thread diag_task_process_thread(&DM_DiagClientInterf::RemoteDiagTaskProcess,this,
                                                         signaling_content,protocol_version,task_id,
                                                         package_name,package_url,result_url,
                                                         download_auth_mode,uploadload_auth_mode,
                                                         task_timeout,interact_script,state_upload,
                                                         upload_period,encrypt_file_sha256,package_size,
                                                         package_sha256,public_key_sha256,key0,sign_base64,
                                                         excution_mode, task_priority, task_period, condition_check_value);
                    diag_task_process_thread.detach();
                }
                else{
                    #if 0
                    std::thread diag_task_process_thread(&DM_DiagClientInterf::PresetDiagTaskProcessForCould,this,
                                                         signaling_content,protocol_version,task_id,package_name,
                                                         result_url,uploadload_auth_mode,task_timeout,
                                                         interact_script,state_upload,upload_period,sign_base64);
                    diag_task_process_thread.detach();
                    #endif
                }
            // }
            // else{
            //     LOGERROR << "parse custom parameter failed!";
            // }
        }
#if 0
        //messageType = 103，诊断终止
        else if(message_type == 103){
            LOGINFO << "message type:103,stop diagnostic,processing...";

            if(reader.parse(custom_parameter_string,custom_parameter)){
                std::string package_name = custom_parameter["packageName"].asString();
                size_t task_timeout = custom_parameter["taskTimeout"].asUInt();

                //终止脚本执行操作
                signaling_content = DM_DiagClientInterf::CombineSignalingContent("103",protocol_version,timestamp,
                                                                                 task_id,custom_parameter_string);
                std::thread stop_diag_thread(&DM_DiagClientInterf::StopDiag,this,signaling_content,sign_base64,
                                             protocol_version,task_id,package_name,task_timeout);
                stop_diag_thread.detach();
            }
            else {
                LOGERROR << "parse custom parameter failed!";
            }
        }

        //messageType = 106，诊断脚本与云端交互应答
        else if(message_type == 106){
            LOGINFO << "message type:106,diag interaction response.";

            if(reader.parse(custom_parameter_string,custom_parameter)){
                //将云端交互信息传递给脚本解析模块
                std::string package_name = custom_parameter["packageName"].asString();
                Json::Value interact_parameter = custom_parameter["interactResponseParameter"];

                signaling_content = DM_DiagClientInterf::CombineSignalingContent("106",protocol_version,timestamp,
                                                                                 task_id,custom_parameter_string);
                std::thread send_interact_response_parameter_thread(&DM_DiagClientInterf::SendInteractResponseParameter,
                                                                    this,signaling_content,sign_base64,protocol_version,
                                                                    task_id,package_name,interact_parameter);
                send_interact_response_parameter_thread.detach();
            }
            else{
                LOGERROR << "parse custom parameter failed!";
            }
        }
        //messageType = 108，诊断结果压缩包上传结果
        else if(message_type == 108){
            LOGINFO << "message type:108,Script package upload result.";

            custom_parameter_string = value["customParameter"].asString();
            if(reader.parse(custom_parameter_string,custom_parameter)){
                std::string package_name = custom_parameter["packageName"].asString();
                LOGINFO <<  "package: " << package_name.c_str() << " upload succeed.";
            }
            else{
                LOGERROR << "parse custom parameter failed!";
            }
        }
        //messageType = 200,诊断执行条件参数修改请求
        else if(message_type == 200){
            LOGINFO << "message type:200,modfy condition parameter.";

            if(reader.parse(custom_parameter_string,custom_parameter)){
                Json::Value script_run_condition = custom_parameter["scriptRunCondition"];

                //执行修改操作
                signaling_content = DM_DiagClientInterf::CombineSignalingContent("200",protocol_version,timestamp,
                                                                                 task_id,custom_parameter_string);
                std::thread modify_parameters_thread(&DM_DiagClientInterf::ModifyParameters,this,signaling_content,
                                                     sign_base64,protocol_version,task_id,script_run_condition);
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
                signaling_content = DM_DiagClientInterf::CombineSignalingContent("202",protocol_version,timestamp,
                                                                                 task_id,custom_parameter_string);
                std::thread modify_engine_time_parameters_thread(&DM_DiagClientInterf::ModifyDiagEngineTimeParameters,
                                                                 this,signaling_content,sign_base64,protocol_version, 
                                                                 task_id,p2Client,p2StarClient);
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
                signaling_content = DM_DiagClientInterf::CombineSignalingContent("400",protocol_version,timestamp,
                                                                                 task_id,custom_parameter_string);
                std::thread delete_local_script_package_thread(&DM_DiagClientInterf::DeleteLocalDiagFile,this,
                                                               signaling_content,sign_base64,protocol_version,
                                                               task_id,file_type,package_name);
                delete_local_script_package_thread.detach();
            }
            else{
                LOGERROR << "parse custom parameter failed!";
            }
        }
        //messageType = 402,新增或更新本地文件
        else if(message_type == 402) {
            LOGINFO << "message type:402, update local script or other diag files";

            if(reader.parse(custom_parameter_string,custom_parameter)){
                std::string package_url = custom_parameter["packageUrl"].asString();
                size_t file_type = custom_parameter["fileType"].asUInt();
                int download_auth_mode = custom_parameter["packageUrlSsl"].asInt();

                //执行更新文件操作
                signaling_content = DM_DiagClientInterf::CombineSignalingContent("402",protocol_version,timestamp,
                                                                                 task_id,custom_parameter_string);
                std::thread update_local_script_thread(&DM_DiagClientInterf::UpdateLocalDiagFile,this,
                                                       signaling_content,sign_base64,protocol_version,
                                                       task_id,package_url,download_auth_mode,file_type);
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
                signaling_content = DM_DiagClientInterf::CombineSignalingContent("500",protocol_version,timestamp,
                                                                                 task_id,custom_parameter_string);
                std::thread query_file_Inf_thread(&DM_DiagClientInterf::QueryFileInf,this,signaling_content,
                                                  sign_base64,protocol_version,task_id,file_type,package_name);
                query_file_Inf_thread.detach();
            }
            else{
                LOGERROR << "parse custom parameter failed!";
            }
        }
#endif
        else{
            LOGERROR << "the corresponding message type was not identified.";
        }
    }
    else{
        LOGERROR << "parse cloud signaling failed!";
    }

ACK:
    //返回给云端ack
    Json::Value receive_ack;
    receive_ack["Result"] = "OK";
    std::string receiveDiagnosisResult = receive_ack.toStyledString();
    return receiveDiagnosisResult;
}

void DM_DiagClientInterf::Init()
{
    //启动车云代理server socket，注册消息接收函数
    vcp_server_socket_.Start();
    DomainSocket::Server::MessageCallback vcp_message_callback = [this](std::string_view msg){
        LOGINFO << "receive diag task from vcp: " << msg.data();
        this->receiveDiagData(std::string(msg));
    };
    vcp_server_socket_.SetMessageCallback(vcp_message_callback);

    //启动idcu server socket，注册消息接收函数
    idcu_server_socket_.Start();
    DomainSocket::Server::MessageCallback idcu_message_callback = [&](std::string_view msg){
        LOGINFO << "receive diagTaskUserAuth response from idcu.";
        st_DiagTaskUserAuthResponse response = DeserializeDiagTaskUserAuthResponse(std::string(msg));
        idcu_response_map_.emplace(response.taskid, response.auth_code);
    };
    idcu_server_socket_.SetMessageCallback(idcu_message_callback);

    //连接脚本解析模块
    LOGINFO << "ready to connect the script parsing module...";
    Json::Reader reader;
    std::ifstream ifs;
    Json::Value root;
    std::string socket_file_path = "";
    ifs.open(SOCKET_CONFIG_FILE_PATH);
    if(ifs.is_open()){
        LOGINFO <<  "open socket parameters file: " << SOCKET_CONFIG_FILE_PATH << " success.";
        if (!reader.parse(ifs, root, false)) {
        LOGERROR << "parse socket parameters file failed!";
        } else {
            socket_file_path = root["Socket_Pip_File"].asString();
            LOGINFO <<  "parse socket file path: " << socket_file_path.c_str();
        }
    }else{
        LOGERROR << "open socket parameters file: " << SOCKET_CONFIG_FILE_PATH << " failed!";
    }

    ifs.close();
    script_process_instance->Start(socket_file_path.c_str(), [this](const ReceiveData& rd){this->ProcessEventOrPeriodDiagTaskResult(rd);});
}

void DM_DiagClientInterf::ProcessEventOrPeriodDiagTaskResult(const ReceiveData& recv_data)
{
    std::string script_path = recv_data.script_path;
    std::string task_config_path = "";
    bool script_exist = std::filesystem::exists(script_path) && std::filesystem::is_directory(script_path);
    if (script_exist) {
        task_config_path = script_path + "/task_config.json";
    } else {
        LOGERROR << "DM_DiagClientInterf::ProcessEventOrPeriodDiagTaskResult script: " << script_path << " not exist.";
        return;
    }
    
    int exection_mode = 0;
    std::string task_id = "";
    std::string protocol_version = "";
    std::string result_url = "";
    int result_url_ssl = 0;
    // 读取 JSON 文件
    std::ifstream json_file(task_config_path);
    if (!json_file.is_open()) {
        LOGERROR << "DM_DiagClientInterf::ProcessEventOrPeriodDiagTaskResult open task config file: " << task_config_path << " failed!";
        return;
    }
    Json::Reader reader;
    Json::Value root;
    // 使用 Json::Reader 解析 JSON 文件
    bool check_json_result = reader.parse(json_file, root, false);
    if (!check_json_result) {
        LOGERROR << "DM_DiagClientInterf::ProcessEventOrPeriodDiagTaskResult parse task config file: " << task_config_path << " failed!";
        return;
    } else {
        exection_mode = root["excutionMode"].asInt();
        task_id = root["taskId"].asString();
        protocol_version = root["protocolVersion"].asString();
        result_url = root["resultUrl"].asString();
        result_url_ssl = root["resultUrlSsl"].asInt();
    }

    ErrorType process_result = recv_data.err_code;
    if(exection_mode == 1){
        //单次任务不在这里处理
        return;
    } else if (exection_mode == 2 || exection_mode == 3){
        //处理事件或周期任务
        LOGINFO << "DM_DiagClientInterf::ProcessEventOrPeriodDiagTaskResult receive script: " << script_path << ", excutionMode: " << exection_mode << ", process result: " << process_result;
        bool is_report_launch_result = false;
        bool process_end = false;
        bool script_exec_finish = ProcessDiagTaskResult(process_result, script_path, is_report_launch_result, protocol_version, task_id, process_end);
        if(script_exec_finish && !process_end){
            LOGINFO << "DM_DiagClientInterf::ProcessEventOrPeriodDiagTaskResult process script: " << script_path << " result finish.";
            std::string script_result_path = script_path + "/result/";
            size_t lastSlashPos = script_path.rfind('/');
            std::string package_name = script_path.substr(lastSlashPos + 1);
            PackAndEncryptAndUploadDiagResult(script_result_path, package_name, result_url, result_url_ssl, protocol_version, task_id);
        }
    } else {
        LOGERROR << "DM_DiagClientInterf::ProcessEventOrPeriodDiagTaskResult receive script: " << script_path << " response, but excutionMode is not 1 or 2 or 3.";
    }
}

void DM_DiagClientInterf::Release()
{
    script_process_instance->Stop();

    vcp_server_socket_.Stop();

    idcu_server_socket_.Stop();
}


void DM_DiagClientInterf::RemoteDiagTaskProcess(std::string signalingContent, std::string protocolVersion,
                                                std::string taskId, std::string packageName, std::string packageUrl,
                                                std::string resultUrl, int packageUrlSsl, int resultUrlSsl,
                                                u_int taskTimeout, bool interactScript, bool stateUpload,
                                                u_int uploadPeriod, std::string encryptFileSha256,
                                                size_t package_size, std::string packageSha256,
                                                std::string publicKeySha256, std::string key0, std::string signBase64, 
                                                int excutionMode, int taskPriority, int taskPeriod, Json::Value conditionCheckValue)
{
    Json::Value custom_parameter;
    std::string response_json;
    DiagFile diag_file_instance;

    //公钥验证
    bool check_public_key_result;
    check_public_key_result = DM_DiagClientInterf::CheckPublicKeySha256(publicKeySha256);
    if(check_public_key_result == false){
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 227;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }
    else{
        LOGINFO <<  "local public key is correct, continue remote task.";
    }
    /*-----------------------------------------------------信令验签流程-----------------------------------------------------*/
#if 0
    //信令messageType100验签
    bool check_sign_result;
    check_sign_result = DM_DiagClientInterf::CheckSigalingSign(signalingContent,signBase64,diag_file_instance);
    //验签成功
    if(check_sign_result){
        LOGINFO <<  "verify signaling successful.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 201;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    //验签失败
    else{
        LOGERROR << "verify signaling failed.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 202;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }
#endif
    /*-----------------------------------------------------信令验签流程-----------------------------------------------------*/

    /*-----------------------------------------------------任务仲裁流程-----------------------------------------------------*/
#if 0
    //判断是否有任务执行中，当任务执行中时，上报massageType101，拒绝该任务
    if(is_in_task){
        LOGERROR << "script parsing module is busy,can not perform this task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 229;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }
    else{
        LOGINFO <<  "script parsing module is idle, can perform this task.";
    }

    //设置任务状态
    is_in_task = true;
    remote_task_script_state = ScriptState::NotExecuted;

    //仲裁OTA模式和本地诊断
    /* kevin auth */
    authentication_state = authentication_state_instance->authentication_state;
    if(authentication_state != midware::diag::AuthenticationState::kInitial &&
       authentication_state != midware::diag::ReportFactoryMode::kFactoryMode){
        //本地诊断任务执行中，仲裁远程任务结束，messageType = 101
        LOGERROR << "local diag task is being executed,stop remote task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 226;
        custom_parameter["message"] = "local diag task is being executed.";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
        return;
    }
    else{
        LOGINFO <<  "not in local task,continue remote task.";
    }

    //判断是否处于OTA模式下
    /* kevin check
    bool ota_mode;
    ota_mode = condition_check->IsOtaMode();
    if(ota_mode){
        //ota模式中，仲裁远程任务结束，messageType = 101
        LOGERROR << "in ota mode,stop remote task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 226;
        custom_parameter["message"] = "in ota mode";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
        return;
    }
    else{
        LOGINFO <<  "not in ota mode.";
    }
    */
    if(is_received_stop_task_signal){
        LOGERROR << "receive stop signal,quit task.";
        remote_task_script_state = ScriptState::NoTask;
        is_in_task = false;
        is_received_stop_task_signal = false;
        return;
    }
#endif
    /*-----------------------------------------------------任务仲裁流程-----------------------------------------------------*/

    /*----------------------------------------------------脚本包下载流程-----------------------------------------------------*/
    u_int download_time = 1;
    uint8_t script_download_result = 0x01;
    std::string package_path = "";
    bool script_exist = false;
    package_path = package_path + TEMP_STORAGE_FOLDER + packageName + ".tar.gz";

    //单次任务
    if(excutionMode == 1){
        if(access(package_path.c_str(),0) == 0){
            LOGINFO <<  "temp script package exist,deleting...";
            file_opreation_instance.deleteFile(package_path);
        }
        /* 不能删除TEMP_SCRIPT_FOLDER，因为有可能之前下发的单次任务脚本由于优先级低还没执行，执行完脚本的时候删除单次任务脚本*/
        // file_opreation_instance.deleteDir(TEMP_SCRIPT_FOLDER,false);
    }
    //事件或周期任务
    else if(excutionMode == 2 || excutionMode == 3){
        if(access(package_path.c_str(),0) == 0){
            LOGINFO <<  "script package exist, need not download again.";
            script_exist = true;
        }
    }

    if(excutionMode == 1 || script_exist == false){
        script_download_result = DM_DiagClientInterf::DiagFileDownload(packageName, packageUrl, packageUrlSsl);
        while(script_download_result != 0x00){
            if(download_time == 3){
                LOGERROR << "download retry 2 times,failed.";
                break;
            }

            LOGERROR << "download failed,retry...";
            file_opreation_instance.deleteFile(package_path);
            script_download_result = DM_DiagClientInterf::DiagFileDownload(packageName, packageUrl, packageUrlSsl);
            download_time++;
        }
    }

    //下载失败响应，messageType = 101
    custom_parameter.clear();
    if(script_download_result != 0x00){
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 204;
        custom_parameter["message"] = "script package download failed.";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
#ifdef SECURITY_EVENT_WRAPPER
        //上报安全事件
        std::string download_error_event = "A DIAG HTTPS remote diag script download error";
        (void)middleware::seceventwrapper::SecurityEventWrapper::GetInstance().Send(download_error_event, "AsfVirtualDiagnosticTester");
#endif
        return;
    }
    //下载成功响应，messageType = 101
    else{
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 203;
        custom_parameter["message"] = "";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }

    if(is_received_stop_task_signal){
        LOGERROR << "receive stop signal,quit task.";
        remote_task_script_state = ScriptState::NoTask;
        is_in_task = false;
        is_received_stop_task_signal = false;
        return;
    }
    /*----------------------------------------------------脚本包下载流程-----------------------------------------------------*/

    /*----------------------------------------------------解密脚本包流程-----------------------------------------------------*/
    std::string key1 = "";
    std::string public_key = "";
    bool compare_result;
    std::vector<uint8_t> encrypt_file_sha256;
    std::vector<uint8_t> encrypt_file;
    std::vector<uint8_t> package_sha256;
    std::vector<uint8_t> package_sig;
    std::vector<uint8_t> package;
    std::ifstream public_key_file;
    std::ifstream download_file;
    std::ofstream decrypt_file;
    size_t index = 0;
    char temp;

    //计算加密包的encryptFileSha256，并进行比对
    download_file.open(package_path,std::ios::binary | std::ios::in);
    if(download_file.is_open()){
        while(download_file.get(temp)){
            encrypt_file.push_back((uint8_t)temp);
            index++;
        }
        LOGINFO <<  "open encrypt package succeed,size: " << index << " bytes.";
    }
    else{
        LOGERROR << "open encrypt package failed.";
    }
    download_file.close();

    encrypt_file_sha256 = diag_file_instance.CalculateFileSha256(encrypt_file);
    compare_result = diag_file_instance.CompareSha256(encrypt_file_sha256, encryptFileSha256);

    //比对sha256成功
    if(compare_result){
        LOGINFO <<  "encrypt file sha256 value is correct.";
    }
    //比对sha256失败，messageType = 101
    else{
        LOGERROR << "encrypt file sha256 value is incorrect!";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 206;
        custom_parameter["message"] = "compare encrypt file sha256 failed.";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
#ifdef SECURITY_EVENT_WRAPPER
        // 上报安全事件
        std::string completeness_check_error_event = "A DIAG SCRIPT remote diag script Completeness Check error";
        (void)middleware::seceventwrapper::SecurityEventWrapper::GetInstance().Send(completeness_check_error_event, "AsfVirtualDiagnosticTester"); 
#endif
        return;
    }

    //AES解密获得script-package.merge
    key1 = diag_file_instance.ConvertKey(key0);
    std::string encrypt_file_string(encrypt_file.begin(), encrypt_file.end()); 
    std::string package_merge_string{};
    #if (defined USING_AY5G) || (defined USING_A19G1) || (defined USING_AY2G)
    package_merge = diag_file_instance.DecryptFileByAES(encrypt_file_string,key1,key0);
    #else
    auto package_merge_result = diag_file_instance.DecryptFileByAES(encrypt_file_string, key1);
    if (!package_merge_result.has_value())
    {
        LOGERROR << "DM_DiagClientInterf::RemoteDiagTaskProcess: decrypt file by aes error.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 206;
        custom_parameter["message"] = "decrypt file by aes failed";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
#ifdef SECURITY_EVENT_WRAPPER
        // 上报安全事件
        std::string decrypt_error_event = "A DIAG SCRIPT remote diag script decrypt error";
        (void)middleware::seceventwrapper::SecurityEventWrapper::GetInstance().Send(decrypt_error_event, "AsfVirtualDiagnosticTester"); 
#endif
        return;        
    }
    package_merge_string = package_merge_result.value();
    #endif
    std::vector<uint8_t> package_merge(package_merge_string.begin(), package_merge_string.end());

    //文件提取
    diag_file_instance.ExtractFile(package_merge, package_sig, package);
    LOGINFO <<  "get dencrypt package,size: " << package.size() << " bytes.";

    //计算文件提取后的package sha256，并进行比对
    package_sha256 = diag_file_instance.CalculateFileSha256(package);
    compare_result = diag_file_instance.CompareSha256(package_sha256, packageSha256);

    //比对sha256成功
    if(compare_result){
        LOGINFO <<  "package sha256 value is correct.";
    }
    //比对sha256失败，messageType = 101
    else{
        LOGERROR << "package sha256 value is incorrect!";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 206;
        custom_parameter["message"] = "compare package sha256 failed";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
#ifdef SECURITY_EVENT_WRAPPER
        // 上报安全事件
        std::string completeness_check_error_event = "A DIAG SCRIPT remote diag script Completeness Check error";
        (void)middleware::seceventwrapper::SecurityEventWrapper::GetInstance().Send(completeness_check_error_event, "AsfVirtualDiagnosticTester"); 
#endif
        return;
    }

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
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 205;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    else{
        //解密失败，messageType = 101
        LOGERROR << "error,open package failed.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 206;
        custom_parameter["message"] = "";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
#ifdef SECURITY_EVENT_WRAPPER
        // 上报安全事件
        std::string decrypt_error_event = "A DIAG SCRIPT remote diag script decrypt error";
        (void)middleware::seceventwrapper::SecurityEventWrapper::GetInstance().Send(decrypt_error_event, "AsfVirtualDiagnosticTester"); 
#endif
        return;
    }
    decrypt_file.close();

    //RSA解密script-package.sig，并与packageSha256进行比对
    std::ifstream ifs_key;
    ifs_key.open(PUBLIC_KEY_PATH,std::ios::binary | std::ios::in);
    if(ifs_key.is_open()){
        index = 0;
        while(ifs_key.get(temp)){
            public_key.push_back((uint8_t)temp);
            index++;
        }
        //cout << "[DM_DiagClientInterf::CheckSigalingSign]public key size:" << index << endl;
    }
    diag_file_instance.PublickeyRead(public_key);
    if(public_key.size() != 0){
        LOGINFO <<  "public key size: " << public_key.size() << "bytes";
    }
    else {
        LOGERROR << "error, can not open public key file.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 210;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101, protocolVersion,taskId ,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
#ifdef SECURITY_EVENT_WRAPPER
        // 上报安全事件
        std::string sign_error_event = "A DIAG COMM remote diag command signature error";
        (void)middleware::seceventwrapper::SecurityEventWrapper::GetInstance().Send(sign_error_event, "AsfVirtualDiagnosticTester"); 
#endif
        return;
    }
    #if 0
    std::string package_sig_string(package_sig.begin(), package_sig.end());
    auto decrypt_package_result = diag_file_instance.DecryptFileByRSA(package_sig_string, public_key);
    #endif
    bool verify_sign_result = diag_file_instance.VerifySignByContext(package, package_sig, public_key);
    if (verify_sign_result)
    {
        LOGINFO <<  "package verify sign by context is correct.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 209;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    else
    {
        LOGERROR << "DM_DiagClientInterf::RemoteDiagTaskProcess: error, decrypt package failed.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 210;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101, protocolVersion,taskId ,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;

#ifdef SECURITY_EVENT_WRAPPER
        // 上报安全事件
        std::string sign_error_event = "A DIAG COMM remote diag command signature error";
        (void)middleware::seceventwrapper::SecurityEventWrapper::GetInstance().Send(sign_error_event, "AsfVirtualDiagnosticTester"); 
#endif
        return;
    }
    #if 0
    std::string decrypt_package_string = decrypt_package_result.value();
    std::vector<uint8_t> decrypt_package_sig(decrypt_package_string.begin(), decrypt_package_string.end());
    compare_result = diag_file_instance.CompareSha256(decrypt_package_sig, packageSha256);

    //验签成功，messageType = 101
    if(compare_result){
        LOGINFO <<  "package sign sha256 value is correct.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 209;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    //验签失败，messageType = 101
    else{
        LOGERROR << "package sign sha256 value is incorrect!";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 210;
        custom_parameter["message"] = "compare package sign failed";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
        // 上报安全事件
        std::string sign_error_event = "A DIAG COMM remote diag command signature error";
        (void)middleware::seceventwrapper::SecurityEventWrapper::GetInstance().Send(sign_error_event, "AsfVirtualDiagnosticTester");        
        return;
    }
    #endif
    LOGINFO <<  "file decryption process finish.";

    if(is_received_stop_task_signal){
        LOGERROR << "receive stop signal,quit task.";
        remote_task_script_state = ScriptState::NoTask;
        is_received_stop_task_signal = false;
        is_in_task = false;
        return;
    }
    /*----------------------------------------------------解密脚本包流程-----------------------------------------------------*/

    /*----------------------------------------------------解压脚本包流程-----------------------------------------------------*/
    std::string script_path = "";
    //单次任务
    if(excutionMode == 1){
        script_path = TEMP_SCRIPT_FOLDER + packageName;
        LOGINFO <<  "single task, script path: " << script_path;
        diag_file_instance.DecompressAndDistributeFile(packageName,TempScriptPackage);
    }
    //事件任务
    else if(excutionMode == 2){
        script_path = EVENT_SCRIPT_FOLDER + packageName;
        LOGINFO <<  "event task, script path: " << script_path;
        diag_file_instance.DecompressAndDistributeFile(packageName,EventScriptPackage);
    }
    //周期任务
    else if(excutionMode == 3){
        script_path = PERIOD_SCRIPT_FOLDER + packageName;
        LOGINFO <<  "period task, script path: " << script_path;
        diag_file_instance.DecompressAndDistributeFile(packageName,PeriodScriptPackage);
    }
    
    bool decompress_result = false;

    if (script_path.size() >= 4 && script_path.substr(script_path.size() - 4) == ".tgz") {
        script_path = script_path.substr(0, script_path.size() - 4);
        LOGINFO <<  "script_path: " << script_path;
    }

    if(access(script_path.c_str(),0) == 0){
        LOGINFO <<  "package is decompressed successful.";
        decompress_result = true;

        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 211;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    else{
        LOGERROR << "package is decompressed failed!";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 212;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
        return;
    }

    if(decompress_result){
        //解压脚本成功后将检查条件保存成文件到脚本同样的目录
        std::string condition_file_name = script_path + "/condition_check.json";
        Json::FastWriter writer;
        std::string json_str = writer.write(conditionCheckValue);
        std::ofstream condition_file;
        condition_file.open(condition_file_name);
        if (condition_file.is_open()) {
            condition_file << json_str;
            condition_file.close();
            LOGINFO <<  "save condition check value " << conditionCheckValue.toStyledString() << " to file: " << condition_file_name << " successful. ";
        } else {
            LOGERROR << "Failed to open condition_file for writing: " << condition_file_name;
        }

        std::string config_file_name = script_path + "/task_config.json";
        Json::Value root;
        root["excutionMode"] = excutionMode;
        root["taskPriority"] = taskPriority;
        if(excutionMode == 3){
            root["taskPeriod"] = taskPeriod;
        }
        if(excutionMode == 2 || excutionMode == 3){
            root["taskId"] = taskId;
            root["protocolVersion"] = protocolVersion;
            root["resultUrl"] = resultUrl;
            root["resultUrlSsl"] = resultUrlSsl;
        }
        Json::FastWriter writer1;
        std::string json_str1 = writer1.write(root);
        std::ofstream config_file(config_file_name);
        if (config_file.is_open()) {
            config_file << json_str1;
            config_file.close();
            LOGINFO <<  "save task config " << root.toStyledString() << " to file: " << config_file_name << " successful. ";
        } else {
            LOGERROR << "Failed to open config_file for writing: " << config_file_name;
        }
    }

    if(is_received_stop_task_signal){
        LOGERROR << "receive stop signal,quit task.";
        remote_task_script_state = ScriptState::NoTask;
        is_received_stop_task_signal = false;
        is_in_task = false;
        return;
    }
    /*----------------------------------------------------解压脚本包流程-----------------------------------------------------*/

    /*-----------------------------------------------执行脚本前置条件判段流程--------------------------------------------------*/
#if 0
    //判断诊断代理的安全认证状态，判断是否有本地诊断任务执行中
    /* kevin auth */
    authentication_state = authentication_state_instance->authentication_state;
    if(authentication_state != midware::diag::AuthenticationState::kInitial &&
       authentication_state != midware::diag::ReportFactoryMode::kFactoryMode){
        //本地诊断任务执行中，仲裁远程任务结束，messageType = 101
        LOGERROR << "local diag task is being executed,stop remote task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 226;
        custom_parameter["message"] = "local diag task is being executed,stop remote task.";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
        return;
    }
    else{
        LOGINFO <<  "not in local task,continue remote task.";
    }
    /* kevin check
    //判断是否处于OTA模式下
    ota_mode = condition_check->IsOtaMode();
    if(ota_mode){
        //ota模式中，仲裁远程任务结束，messageType = 101
        LOGERROR << "in ota mode,stop remote task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 226;
        custom_parameter["message"] = "in ota mode";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
        return;
    }
    else{
        LOGINFO <<  "not in ota mode.";
    }
    */
    //调用配电模块进行智能配电操作
    /* kevin 配电
    condition_check->SetVTState(0x01);
    condition_check->SetIEPowerState(static_cast<uint16_t>(taskTimeout),true);
    */
    if(is_received_stop_task_signal){
        LOGERROR << "receive stop signal,quit task.";
        remote_task_script_state = ScriptState::NoTask;
        is_received_stop_task_signal = false;
        is_in_task = false;
        return;
    }
#endif
    /*-----------------------------------------------执行脚本前置条件判段流程--------------------------------------------------*/

    /*-----------------------------------------------调用脚本解析模块执行任务--------------------------------------------------*/
    Json::Reader reader;
    Json::Value root;
    //string config_path = SOCKET_CONFIG_FILE_PATH;
    std::string interact_file_path;
    std::string clear_command = "rm -rf ";
    int send_result = -1;
    #if 0
    u_int connect_time = 1;
    #endif

    //如脚本有交互内容，初始化交互状态,并创建交线程
    if(interactScript){
        interact_state.protocol_version = protocolVersion;
        interact_state.task_id = taskId;
        interact_state.run_type = 601;
        interact_state.is_response = false;
        interact_state.is_finish = false;
    }

    //如果存在结果文件先进行清除操作
    std::string script_result_path = script_path + "/result/";
    if(access(script_result_path.c_str(), 0) == 0){
        file_opreation_instance.deleteDir(script_result_path, false);
    }

    if(is_received_stop_task_signal){
        LOGERROR << "receive stop signal,quit task.";
        remote_task_script_state = ScriptState::NoTask;
        is_received_stop_task_signal = false;
        is_in_task = false;
        return;
    }

    /*Init函数中的Start中while循环调用Connect，理论上走到这里一定是连接成功的，不需要重连操作。*/
    #if 0
    //ScriptProcess* script_process_instance = ScriptProcess::GetInstance();
    start_result = script_process_instance->Start(socket_file_path.c_str());
    LOGINFO <<  "connectting script parsing module,start: " << start_result; 
    while(start_result != 0){
        if(connect_time == u_int(3)){
            LOGERROR << "connect script parsing module retry 2 times,failed.";
            break;
        }

        start_result = script_process_instance->Start(socket_file_path.c_str());
        connect_time++;
        LOGINFO <<  "connectting script parsing module failed,retry: " << start_result;
    }
    #endif

    SendData send_data;
    send_data.script_state = ScriptRunState::kScriptStart;
    send_data.script_path = const_cast<char*>(script_path.c_str());
    send_result = script_process_instance->Send(&send_data);
    process_stop_request_completed = false;
    LOGINFO <<  "sending script parsing module script path: " << script_path;
    LOGINFO <<  "sending script parsing module data,send: " << send_result;

    if(excutionMode == 1) {
        ErrorType process_result;
        struct timespec timer;
        clock_gettime(CLOCK_REALTIME,&timer);
        clock_t start_script_run_time = timer.tv_sec;
        clock_t current_time;
        u_int script_run_time;
        // u_int recieve_count = 0;
    #if 0
    u_int stop_count = 0;
    u_int wait_interaction_finish_time = 0;
    #endif
        bool is_report_launch_result = false;
        //*package_name = const_cast<char*>(packageName.c_str());

        remote_task_script_state = ScriptState::Running;

        bool process_end = false;
        while(SCRIPT_RUNING){
            //收到退出信号，结束线程
            if(global_stop_flag){
                return;
            }

    #if 0
            //如有本地诊断任务执行中，仲裁远程任务结束，messageType = 101
            /* kevin auth */
            authentication_state = authentication_state_instance->authentication_state;
            if((authentication_state != midware::diag::AuthenticationState::kInitial) &&
            (authentication_state != midware::diag::ReportFactoryMode::kFactoryMode)){
                LOGERROR << "local diag task is being executed,stop the remote task.";
                custom_parameter.clear();
                custom_parameter["runType"] = 601;
                custom_parameter["runState"] = 226;
                custom_parameter["message"] = "local diag task is being executed,stop remote task.";

                response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
                DM_DiagClientInterf::SendDiagData(response_json);

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
                                LOGERROR << "stop script failed, quit.";
                                break;
                            }
                            else{
                                LOGINFO <<  "script is stopping...";
                                continue; 
                            }
                        }
                        if(stop_count == 2){
                            LOGERROR << "stop script unknown error.";
                            stop_count = 0;
                            break;
                        }
                        LOGINFO <<  "stoping script,state: " << process_result;
                        stop_count++;
                    }
                    else{
                        LOGINFO <<  "stop script running successful.";
                        break;
                    }
                }
                break;
            }
            /* kevin check 
            //当处于OTA模式中时，仲裁远程任务结束，messageType = 101
            if(condition_check->IsOtaMode()){
                LOGERROR << "in ota mode,stop the remote task.";
                custom_parameter.clear();
                custom_parameter["runType"] = 601;
                custom_parameter["runState"] = 226;
                custom_parameter["message"] = "in ota mode,stop remote task.";

                response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
                DM_DiagClientInterf::SendDiagData(response_json);

                //停止脚本运行
                data.itype = kStop;
                data.guid = const_cast<char*>(packageName.c_str());
                send_result = script_process_instance->Send(&data);
                LOGINFO << "sending script parsing module stop message,send: " << send_result;

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
                                LOGERROR << "stop script failed, quit.";
                                break;
                            }
                            else{
                                LOGINFO <<  "script is stopping...";
                                continue; 
                            }
                        }
                        if(stop_count == 2){
                            LOGERROR << "stop script unknown error.";
                            stop_count = 0;
                            break;
                        }
                        LOGINFO <<  "stoping script,state: " << process_result;
                        stop_count++;
                    }
                    else{
                        LOGINFO <<  "stop script running successful.";
                        break;
                    }
                }
                break;
            }
            */
    #endif
            //监听脚本执行状态
            //判断是否超时
            clock_gettime(CLOCK_REALTIME,&timer);
            current_time = timer.tv_sec;
            script_run_time = (u_int)(current_time - start_script_run_time);

            LOGINFO <<  "script running spend times: " << script_run_time;
            //单次任务才判断超时
            if(script_run_time >= taskTimeout){
                //脚本运行超时，停止脚本运行
                process_end = true;
                send_data.script_state = ScriptRunState::KScriptStop;
                send_result = script_process_instance->Send(&send_data);
                LOGINFO <<  "sending script parsing module stop message,send: " << send_result;

                std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(3));
                process_result = script_process_instance->ReceiveDatas(script_path.c_str());

                if(process_result == KStopSuccess){
                    //脚本停止成功，messageType = 101
                    LOGERROR << "script running timeout,stop task successful.";
                    custom_parameter.clear();
                    custom_parameter["runType"] = 601;
                    custom_parameter["runState"] = 224;
                    custom_parameter["message"] = "script running times out,stop task successful.";

                    response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
                    DM_DiagClientInterf::SendDiagData(response_json);
                }
                else{
                    //脚本停止失败，messageType = 101
                    LOGERROR << "script running timeout,stop task failed.";
                    custom_parameter.clear();
                    custom_parameter["runType"] = 601;
                    custom_parameter["runState"] = 225;
                    custom_parameter["message"] = "script running times out,stop task failed";

                    response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
                    DM_DiagClientInterf::SendDiagData(response_json);
                }
                break;
            }

            std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
            process_result = script_process_instance->ReceiveDatas(script_path.c_str());
            LOGINFO <<  "script running state: " << process_result;

            bool script_exec_finish = ProcessDiagTaskResult(process_result, script_path, is_report_launch_result, protocolVersion, taskId, process_end);
            if(script_exec_finish){
                break;
            }
        }
        // script_process_instance->Stop();
        remote_task_script_state = ScriptState::Finish;

        //重置脚本交互状态
        if(interactScript){
            interact_state.task_id = "";
            interact_state.protocol_version = "";
            interact_state.sequence = 0;
            interact_state.is_response = false;
            interact_state.is_finish = true;
        }

        //申请智能配电下电
        /* kevin 下电
        condition_check->SetIEPowerState(0,false);
        condition_check->SetVTState(0x00);
        */

        LOGINFO <<  "invoke the script parsing module finish.";
    /*-----------------------------------------------调用脚本解析模块执行任务--------------------------------------------------*/

    /*-----------------------------------------打包脚本包、加密结果文件、上传结果文件--------------------------------------------*/
        if(!process_end){
            PackAndEncryptAndUploadDiagResult(script_result_path, packageName, resultUrl, resultUrlSsl, protocolVersion, taskId);
        }
    /*-----------------------------------------打包脚本包、加密结果文件、上传结果文件--------------------------------------------*/

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
        
        std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(5));
        //单次任务脚本执行完成删除脚本文件, 等待5s云拿完了结果再删
        std::ostringstream command;
        command << "rm -rf " << script_path << " " << TEMP_SCRIPT_STORAGE_FOLDER + packageName.substr(0, packageName.rfind(".tgz")) << " " << TEMP_STORAGE_FOLDER + packageName + ".tar.gz";
        LOGINFO << "delete temp task script dir command: " << command.str();
        if (system(command.str().c_str()) == -1) {
            LOGERROR << "delete temp task script dir failed.";
        } else {
            LOGINFO << "delete temp task script dir successful.";
        }
    }
}

bool DM_DiagClientInterf::ProcessDiagTaskResult(ErrorType process_result, std::string script_path, bool& is_report_launch_result, std::string protocolVersion, std::string taskId, bool& process_end)
{
    Json::Reader reader;
    Json::Value custom_parameter;
    std::string response_json;
    bool script_exec_finish = false;
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
    kStoping         = 0x0C
    KScriptWaiting   = 0x10
    */
    if(process_result == kExcutedOver){
        #if 0
        //判断是否有交互信息读取并未处理上传完成
        while(!is_read_interaction_message_finish){
            if(wait_interaction_finish_time >= taskTimeout - script_run_time) {
                LOGERROR << "wait for interaction message processing too long,upload the result.";
                break;
            }
            LOGINFO << "interaction message in the buffer has not been processed yet,processing....";
            std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
            wait_interaction_finish_time++;
        }
        #endif

        //脚本执行完成，messageType = 101
        LOGINFO <<  "script execute over.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 218;
        custom_parameter["message"] = "script execute over.";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        script_exec_finish = true;
    }
    else if(process_result == kCONDNOTALLOW){
        process_end = true;
        LOGERROR << "script condition check failed.";
        //获取条件检测失败信息
        std::string condition_json = "";
        Json::Value result;
        Json::Value conditions;

        std::ifstream ifs;
        ifs.open(script_path + "/DiagResult.json");
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
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 215;
        custom_parameter["message"] = condition_json;

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        script_exec_finish = true;
    }
    else if(process_result == KScriptWaiting){
        LOGINFO <<  "script is waiting to run...";
    }
    else if(process_result == KScriptRunning){
        //以running状态判断脚本是否启动成功，messageType = 101 
        // if((recieve_count == 0) && is_report_launch_result == false){
        if(is_report_launch_result == false){
            LOGINFO <<  "script launch successful.";
            custom_parameter.clear();
            custom_parameter["runType"] = 601;
            custom_parameter["runState"] = 213;
            custom_parameter["message"] = "";

            response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
            DM_DiagClientInterf::SendDiagData(response_json);
            is_report_launch_result = true;
        }

        LOGINFO <<  "script is running.";
        // recieve_count++;
        // if((stateUpload) && (recieve_count == uploadPeriod)){
        //     //响应脚本执行进度，messageType = 101
        //     custom_parameter.clear();
        //     custom_parameter["runType"] = 601;
        //     custom_parameter["runState"] = 217;
        //     custom_parameter["message"] = "";

        //     response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        //     DM_DiagClientInterf::SendDiagData(response_json);
        //     recieve_count = 0;
        // }
    }
    else if(process_result == kStoping){
        LOGINFO <<  "script is stopping...";
    }
    else if(process_result == KStopSuccess){
        LOGINFO <<  "stop script running successful.";
        script_stop_success_flag = true;
        script_exec_finish = true;
    }
    else{
        //脚本已从103信令处停止
        if(process_stop_request_completed){
            LOGINFO <<  "script has stopped,do not need to get the status.";
            script_exec_finish = true;
        }

        process_end = true;
        //脚本执行未知错误，messageType = 101
        LOGERROR << "script running unknow error,state: " << (int)process_result;
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 216;
        custom_parameter["message"] = "unknow error";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        script_exec_finish = true;
    }
    return script_exec_finish;
}


void DM_DiagClientInterf::PackAndEncryptAndUploadDiagResult(std::string script_result_path, std::string packageName, std::string resultUrl, int resultUrlSsl,
                                                            std::string protocolVersion, std::string taskId)
{
    Json::Value custom_parameter;
    std::string response_json;
    size_t index = 0;
    DiagFile diag_file_instance;
    /*---------------------------------------------------打包脚本包流程------------------------------------------------------*/
    //将脚本解析模块生成的三个结果文件打包
    std::string script_result_file_path = script_result_path + "DiagResult.tar.gz";
    if(access(script_result_file_path.c_str(), 0) == 0){
        LOGINFO <<  "DiagResult.tar.gz file exist,deleting...";
        file_opreation_instance.deleteFile(script_result_file_path);
        if(access(script_result_file_path.c_str(), 0) == -1){
            LOGINFO <<  "DiagResult.tar.gz file delete successful.";
        }
        else{
            LOGERROR << "DiagResult.tar.gz file delete failed.";
        }
    }

    diag_file_instance.CompressPackage(script_result_path, "DiagResult");
    if(access(script_result_file_path.c_str(), 0) == -1){
        //结果文件打包失败，messageType = 101
        LOGERROR << "create diag result package failed!";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 220;
        custom_parameter["message"] = "create diag result package failed.";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
        return;
    }
    else{
        //结果文件打包成功，messageType = 101
        LOGINFO <<  "create diag result package successful.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 219;
        custom_parameter["message"] = "create diag result package succeed";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }

    //将结果文件复制到大数据目录/data/DiagnosticScript/BackupResult/下
    diag_file_instance.ProcessFile2BigDataFolder(script_result_path, packageName);
    /*---------------------------------------------------打包脚本包流程------------------------------------------------------*/

    /*--------------------------------------------------结果文件加密流程------------------------------------------------------*/
    
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
    
    result_package_file.open(script_result_file_path, std::ios::binary | std::ios::in);
    if(result_package_file.is_open()){
        index = 0;
        while(result_package_file.get(tmp))
        {
            result_package.push_back((uint8_t)tmp);
            index++;
        }
        result_package_file.close();
        LOGINFO <<  "open result package successful,size: " << index << " bytes.";
    }
    else{
        //加密失败，messageType = 101
        LOGERROR << "open result package failed!";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 222;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        result_package_file.close();
        return;
    }
    // result_package_file.close();

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
    LOGINFO <<  "calculate result package sha256: " << result_package_sha256_string.c_str();

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
    encrypt_result_package_file.open(script_result_file_path);
    if(encrypt_result_package_file.is_open()){
        for(auto iter : encrypt_result_package){
            index++;
            encrypt_result_package_file << iter;
        }
        encrypt_result_package_file.close();

        //加密成功，messageType = 101
        LOGINFO <<  "encrypto result package successful.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 221;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101, protocolVersion, taskId, custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    else{
        //加密失败，messageType = 101
        LOGERROR << "encrypto result package failed.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 222;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        remote_task_script_state = ScriptState::NoTask;
        return;
    }
    /*--------------------------------------------------结果文件加密流程------------------------------------------------------*/
    
    /*--------------------------------------------------结果文件上传流程------------------------------------------------------*/
    uint8_t upload_result;
    uint upload_time = 1;

    upload_result = DM_DiagClientInterf::DiagScriptRunResultUpload("DiagResult.tar.gz", script_result_file_path, resultUrl, resultUrlSsl);
    while(upload_result != 0x00){
        if(upload_time == 3){
            LOGERROR << "upload retry 2 times,failed.";
            break;
        }

        LOGERROR << "upload failed,retry...";
        upload_result = DM_DiagClientInterf::DiagScriptRunResultUpload("DiagResult.tar.gz", script_result_file_path, resultUrl, resultUrlSsl);
        upload_time++;
    }

    if(upload_result == 0x00){
        //执行结果上传成功，messageType = 107
        LOGINFO <<  "the diag result package uploads successful.";
        custom_parameter.clear();
        custom_parameter["resultUrl"] = resultUrl;
        custom_parameter["packageSha256"] = result_package_sha256_string;
        custom_parameter["encryptFileSha256"] = encrypt_result_package_sha256_string;
        custom_parameter["key0"] = result_key0;

        response_json = DM_DiagClientInterf::CreateJsonResponse(107,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    else
    {
        LOGERROR << "the diag result package uploads failed.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 223;
        custom_parameter["message"] = "";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    /*--------------------------------------------------结果文件上传流程------------------------------------------------------*/
}

#if 0
void DM_DiagClientInterf::PresetDiagTaskProcessForCould(std::string signalingContent, std::string protocolVersion,
                                                        std::string taskId, std::string packageName, std::string resultUrl,
                                                        int resultUrlSsl, u_int taskTimeout,
                                                        bool interactScript, bool stateUpload, u_int uploadPeriod,
                                                        std::string signBase64)
{
    //Json::Value response_root;
    Json::Value custom_parameter;
    std::string response_json;
    DiagFile diag_file_instance;
    /* 加密
    //初始化加密模块调用
    diag_file_instance.GetCryptoProvider();
    */
    /*-----------------------------------------------------信令验签流程-----------------------------------------------------*/
    //信令messageType100验签
    /* 加密
    bool check_sign_result;
    check_sign_result = DM_DiagClientInterf::CheckSigalingSign(signalingContent,signBase64,diag_file_instance);
    //验签成功
    if(check_sign_result){
        LOGINFO <<  "verify signaling successful.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 201;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    //验签失败
    else{
        LOGERROR << "verify signaling failed.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 202;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }
    */
    /*-----------------------------------------------------信令验签流程-----------------------------------------------------*/

    /*-----------------------------------------------------任务仲裁流程-----------------------------------------------------*/
    //判断是否有任务执行中，当任务执行中时，上报massageType101，拒绝该任务
    if(is_in_task){
        LOGERROR << "script parsing module is busy,can not perform this task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 229;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }
    else{
        LOGINFO <<  "script parsing module is idle, can perform this task.";
    }

    //设置任务状态
    is_in_task = true;

    //仲裁OTA模式和本地诊断
    /* kevin auth */
    authentication_state = authentication_state_instance->authentication_state;
    if(authentication_state != midware::diag::AuthenticationState::kInitial &&
       authentication_state != midware::diag::ReportFactoryMode::kFactoryMode){
        //本地诊断任务执行中，仲裁远程任务结束，messageType = 101
        LOGERROR << "local diag task is being executed,stop remote task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 226;
        custom_parameter["message"] = "local diag task is being executed.";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        is_in_task = false;
        return;
    }
    else{
        LOGINFO <<  "not in local task,continue remote task.";
    }
    /*kevin check
    bool ota_mode;
    ota_mode = condition_check->IsOtaMode();
    if(ota_mode){
        //ota模式中，仲裁远程任务结束，messageType = 101
        LOGERROR << "in ota mode,stop remote task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 226;
        custom_parameter["message"] = "in ota mode";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        return;
    }
    else{
        LOGINFO <<  "not in ota mode.";
    }
    */
    /*-----------------------------------------------------任务仲裁流程-----------------------------------------------------*/

    /*------------------------------------------------执行脚本前置条件判段流程------------------------------------------------*/
    //判断诊断代理的安全认证状态，判断是否有本地诊断任务执行中
    /* kevin auth */
    authentication_state = authentication_state_instance->authentication_state;
    if(authentication_state != midware::diag::AuthenticationState::kInitial &&
       authentication_state != midware::diag::ReportFactoryMode::kFactoryMode){
        //本地诊断任务执行中，仲裁远程任务结束，messageType = 101
        LOGERROR << "local diag task is being executed,stop remote task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 226;
        custom_parameter["message"] = "local diag task is being executed,stop remote task.";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        return;
    }
    else{
        LOGINFO <<  "not in local task,continue remote task.";
    }

    //判断是否处于OTA模式下
    /*kevin check
    ota_mode = condition_check->IsOtaMode();
    if(ota_mode){
        //ota模式中，仲裁远程任务结束，messageType = 101
        LOGERROR << "in ota mode,stop remote task.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 226;
        custom_parameter["message"] = "in ota mode";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        return;
    }
    else{
        LOGINFO <<  "not in ota mode.";
    }
    */
    //调用配电模块进行智能配电操作
    /*
    condition_check->SetVTState(0x01);
    condition_check->SetIEPowerState(static_cast<uint16_t>(taskTimeout),true);
    */
    /*------------------------------------------------执行脚本前置条件判段流程------------------------------------------------*/

    /*------------------------------------------------调用脚本解析模块执行任務------------------------------------------------*/
    Json::Reader reader;
    Json::Value root;
    //string config_path = SOCKET_CONFIG_FILE_PATH;
    std::string socket_file_path;
    std::string interact_file_path;
    std::string clear_command = "rm -rf ";
    std::ifstream ifs;
    int start_result;
    int send_result;
    //char **package_name = new char*;

    LOGINFO <<  "start to invoke the script parsing module...";
    ifs.open(SOCKET_CONFIG_FILE_PATH);
    if(ifs.is_open()){
        if(!reader.parse(ifs, root, false)){
            //脚本执行模块socket连接失败，脚本启动失败，messageType = 101
            LOGINFO <<  "parse socket parameters file failed!";
            custom_parameter.clear();
            custom_parameter["runType"] = 601;
            custom_parameter["runState"] = 216;
            custom_parameter["message"] = "parse socket parameters file failed";

            response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
            DM_DiagClientInterf::SendDiagData(response_json);
            return;
        }
        else{
            socket_file_path = root["Socket_Pip_File"].asString();
            interact_file_path = root["Interact_Pip_File"].asString();
            LOGINFO <<  "open socket file,path: " << socket_file_path.c_str() << " open interact socket file,path: "
                    << interact_file_path.c_str();
        }
    }
    else{
        //脚本执行模块socket连接失败，脚本启动失败，messageType = 101
        LOGINFO <<  "parse socket parameters file failed!";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 216;
        custom_parameter["message"] = "parse socket parameters file failed";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }
    ifs.close();

    //如脚本有交互内容，初始化交互状态,并创建交线程
    if(interactScript){
        interact_state.protocol_version = protocolVersion;
        interact_state.task_id = taskId;
        interact_state.run_type = 601;
        interact_state.is_response = false;
        interact_state.is_finish = false;
    }

    //如果存在结果文件先进行清除操作
    file_opreation_instance.deleteDir(DIAG_RESULT_FILE_FLODER,false);

    script_process_instance = ScriptProcess::GetInstance();
    start_result = script_process_instance->Start(socket_file_path.c_str());
    LOGINFO <<  "connectting script parsing module,start: " << start_result;

    // SendData data;
    data.itype = kLocal;
    data.guid = const_cast<char*>(packageName.c_str());
    send_result = script_process_instance->Send(&data);
    LOGINFO << "sending script parsing module script name: " << data.guid;
    LOGINFO << "sending script parsing module data,send: " << send_result;

    ErrorType process_result;
    struct timespec timer;
    clock_gettime(CLOCK_REALTIME,&timer);
    clock_t start_script_run_time = timer.tv_sec;
    clock_t current_time;
    u_int script_run_time;
    u_int recieve_count = 0;
    u_int stop_count = 0;
    bool is_report_launch_result = false;
    //*package_name = const_cast<char*>(packageName.c_str());

    while(SCRIPT_RUNING){
        //如有本地诊断任务执行中，仲裁远程任务结束，messageType = 101
        /* kevin auth */
        authentication_state = authentication_state_instance->authentication_state;
        if((authentication_state != midware::diag::AuthenticationState::kInitial) &&
           (authentication_state != midware::diag::ReportFactoryMode::kFactoryMode)){
            LOGERROR << "local diag task is being executed,stop the remote task.";
            custom_parameter.clear();
            custom_parameter["runType"] = 601;
            custom_parameter["runState"] = 226;
            custom_parameter["message"] = "local diag task is being executed,stop remote task.";

            response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
            DM_DiagClientInterf::SendDiagData(response_json);

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
                            LOGINFO <<  "script is stopping...";
                            continue; 
                        }
                    }
                    if(stop_count == 2){
                        LOGERROR << "stop script unknown error.";
                        stop_count = 0;
                        break;
                    }
                    LOGINFO <<  "stoping script,state: " << process_result;
                    stop_count++;
                }
                else{
                    LOGINFO << "stop script running successful.";
                    break;
                }
                std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
            }
            break;
        }

        //当处于OTA模式中时，仲裁远程任务结束，messageType = 101
        /* kevin check
        if(condition_check->IsOtaMode()){
            LOGERROR << "in ota mode,stop the remote task.";
            custom_parameter.clear();
            custom_parameter["runType"] = 601;
            custom_parameter["runState"] = 226;
            custom_parameter["message"] = "in ota mode,stop remote task";

            response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
            DM_DiagClientInterf::SendDiagData(response_json);

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
                            LOGINFO <<  "script is stopping...";
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
                std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
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
                LOGINFO <<  "script running times out,stop task successful.";
                custom_parameter.clear();
                custom_parameter["runType"] = 601;
                custom_parameter["runState"] = 224;
                custom_parameter["message"] = "script running timeout,stop task succeed.";

                response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
                DM_DiagClientInterf::SendDiagData(response_json);
            }
            else{
                //脚本停止失败，messageType = 101
                LOGERROR << "script running timeout,stop task failed!";
                custom_parameter.clear();
                custom_parameter["runType"] = 601;
                custom_parameter["runState"] = 225;
                custom_parameter["message"] = "script running times out,stop task failed.";

                response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
                DM_DiagClientInterf::SendDiagData(response_json);
            }
            break;
            //return;
        }

        std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
        process_result = script_process_instance->ReceiveDatas(packageName.c_str());
        LOGINFO << "script running state: " << process_result;

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
            LOGINFO <<  "script execute over.";
            custom_parameter.clear();
            custom_parameter["runType"] = 601;
            custom_parameter["runState"] = 218;
            custom_parameter["message"] = "script execute over.";

            response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
            DM_DiagClientInterf::SendDiagData(response_json);
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
            custom_parameter["runType"] = 601;
            custom_parameter["runState"] = 215;
            custom_parameter["message"] = condition_json;

            response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
            DM_DiagClientInterf::SendDiagData(response_json);
            break;
        }
        else if(process_result == KScriptRunning){
            //以running状态判断脚本是否启动成功，messageType = 101 
            if((recieve_count == 0) && (is_report_launch_result == false)){
                LOGINFO <<  "script launch successful.";
                custom_parameter.clear();
                custom_parameter["runType"] = 601;
                custom_parameter["runState"] = 213;
                custom_parameter["message"] = "";

                response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
                DM_DiagClientInterf::SendDiagData(response_json);
                is_report_launch_result = true;
            }

            recieve_count++;
            LOGINFO <<  "script is running.";
            if((stateUpload == true )&&( recieve_count == uploadPeriod)){
                //响应脚本执行进度，messageType = 101
                custom_parameter.clear();
                custom_parameter["runType"] = 601;
                custom_parameter["runState"] = 217;
                custom_parameter["message"] = "";

                response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
                DM_DiagClientInterf::SendDiagData(response_json);
                recieve_count = 0;
            }
        }
        else{
            //脚本执行未知错误，messageType = 101
            LOGERROR << "script running unknow error.";
            custom_parameter.clear();
            custom_parameter["runType"] = 601;
            custom_parameter["runState"] = 216;
            custom_parameter["message"] = "unknow error";

            response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
            DM_DiagClientInterf::SendDiagData(response_json);
            break;
        }
    }
    script_process_instance->Stop();

    //重置脚本交互状态
    if(interactScript){
        interact_state.task_id = "";
        interact_state.protocol_version = "";
        interact_state.sequence = 0;
        interact_state.is_response = false;
        interact_state.is_finish = true;
    }

    //申请智能配电下电
    /* 配电
    condition_check->SetIEPowerState(0,false);
    condition_check->SetVTState(0x00);
    */
    LOGINFO <<  "invoke the script parsing module finish.";
    /*------------------------------------------------调用脚本解析模块执行任務------------------------------------------------*/

    /*----------------------------------------------------打包脚本包流程----------------------------------------------------*/
    //将脚本解析模块生成的三个结果文件打包
    if(access(DIAG_RESULT_FILE_PATH,0) == 0){
        LOGINFO <<  "DiagResult.tar.gz file exist,deleting.";
        file_opreation_instance.deleteFile(DIAG_RESULT_FILE_PATH);

        if(access(DIAG_RESULT_FILE_PATH,0) == -1) {
            LOGINFO <<  "DiagResult.tar.gz file delete successful.";
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
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 220;
        custom_parameter["message"] = "create diag result package failed.";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        return;
    }
    else{
        //结果文件打包成功，messageType = 101
        LOGINFO <<  "diag result package create successful.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 219;
        custom_parameter["message"] = "create diag result package succeed.";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    
    //将结果文件复制到大数据目录/data/DiagnosticScript/BackupResult/下
    diag_file_instance.ProcessFile2BigDataFolder();
    /*----------------------------------------------------打包脚本包流程----------------------------------------------------*/

    /*-------------------------------------------------结果文件加密流程开始--------------------------------------------------*/
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
    // size_t index = 0;
    // char tmp;
    /* kevin 加密
    result_package_file.open(DIAG_RESULT_FILE_PATH,std::ios::binary | std::ios::in);
    if(result_package_file.is_open()){
        index = 0;
        while(result_package_file.get(tmp))
        {
            result_package.push_back((uint8_t)tmp);
            index++;
        }
        LOGINFO <<  "open result package succeed,size: " << index << "bytes";
    }
    else{
        //加密失败，messageType = 101
        LOGERROR << "open result package failed.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 222;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
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

    //AES加密结果压缩吧
    result_key0 = diag_file_instance.RandomKey();
    result_key1 = diag_file_instance.ConvertKey(result_key0);
    #if (defined USING_AY5G) || (defined USING_A19G1) || (defined USING_AY2G)
    encrypt_result_package = diag_file_instance.CryptFileByAES(result_package,result_key1,result_key0);
    #else
    encrypt_result_package = diag_file_instance.CryptFileByAES(result_package,result_key1);
    #endif

    //计算结果包的sha256值
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

        //加密成功，messageType = 101
        LOGINFO <<  "encryptn result package succeed.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 221;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    else{
        //加密失败，messageType = 101
        LOGERROR << "encryptn result package failed.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 222;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        is_in_task = false;
        return;
    }
    */
    /*-------------------------------------------------结果文件加密流程开始--------------------------------------------------*/

    /*---------------------------------------------------结果文件上传流程---------------------------------------------------*/
    uint8_t upload_result;
    std::string result_package_path = DIAG_RESULT_FILE_PATH;
    upload_result = DM_DiagClientInterf::DiagScriptRunResultUpload("DiagResult.tar.gz", result_package_path,resultUrl,resultUrlSsl);

    if(upload_result == 0x00){
        //执行结果上传成功，messageType = 107
        LOGERROR << "the diag result package uploads successful.";
        custom_parameter.clear();
        custom_parameter["resultUrl"] = resultUrl;
        custom_parameter["packageSha256"] = result_package_sha256_string;
        custom_parameter["encryptFileSha256"] = encrypt_result_package_sha256_string;
        custom_parameter["key0"] = result_key0;

        response_json = DM_DiagClientInterf::CreateJsonResponse(107,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    else{
        LOGERROR << "the diag result package uploads failed.";
        custom_parameter.clear();
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 223;
        custom_parameter["message"] = "";

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    /*---------------------------------------------------结果文件上传流程---------------------------------------------------*/

    is_in_task = false;
}

void DM_DiagClientInterf::StopDiag(std::string signalingContent, std::string signBase64, std::string protocolVersion,
                                   std::string taskId, std::string packageName, u_int taskTimeout)
{
    Json::Value custom_parameter;
    Json::Value root;
    Json::Reader reader;
    std::string response_json;
    std::string socket_file_path;
    u_int stop_count = 0;
    std::ifstream ifs;
    DiagFile diag_file_instance;

    //信令messageType103验签
    /*加密
    diag_file_instance.GetCryptoProvider();
    bool check_sign_result;
    check_sign_result = DM_DiagClientInterf::CheckSigalingSign(signalingContent,signBase64,diag_file_instance);
    //验签成功
    if(check_sign_result){
        LOGINFO <<  "verify signaling successful.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 201;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    //验签失败
    else{
        LOGERROR << "verify signaling failed.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 202;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }
    */
    ifs.open(SOCKET_CONFIG_FILE_PATH);
    if (!reader.parse(ifs, root, false)){
        //messageType = 104
        LOGERROR << "parse socket parameters file failed!";
        custom_parameter.clear();
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 999;

        response_json = DM_DiagClientInterf::CreateJsonResponse(104,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }
    else{
        socket_file_path = root["Socket_Pip_File"].asString();
        LOGINFO <<  "open socket file,path: " << socket_file_path.c_str();
    }
    ifs.close();

    //根据脚本状态执行不同的停止策略
    struct timespec timer;
    clock_gettime(CLOCK_REALTIME,&timer);
    clock_t start_time = timer.tv_sec;
    clock_t current_time;
    u_int stop_time;
    
    if(remote_task_script_state == ScriptState::Running){
        //SendData data;
        data.itype = kStop;
        data.guid = (char*)packageName.c_str();
        size_t send_result = script_process_instance->Send(&data);
        LOGINFO <<  "sending script parsing module script name: " << data.guid;
        LOGINFO <<  "sending script parsing module data,send: " << send_result;

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
                        LOGINFO <<  "script is stopping...";
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
                    LOGINFO <<  "stop script running successful.";
                    custom_parameter.clear();
                    custom_parameter["result"] = true;
                    custom_parameter["failureReason"];
                    break;
                }

                LOGINFO << "stoping script,state: " << process_result;
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
    else if(remote_task_script_state == ScriptState::NotExecuted){
        LOGERROR << "script is not executed,stopping task...";
        is_received_stop_task_signal = true;

        custom_parameter.clear();
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 406;
        response_json = DM_DiagClientInterf::CreateJsonResponse(104,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);

        while(remote_task_script_state != ScriptState::NoTask){
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
                LOGINFO <<  "task is stopping...";
            }

            std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
        }

        custom_parameter.clear();
        custom_parameter["result"] = true;
        custom_parameter["failureReason"];        
    }
    else if(remote_task_script_state == ScriptState::Finish){
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
    //script_process_instance->Stop();
    response_json = DM_DiagClientInterf::CreateJsonResponse(104,protocolVersion,taskId,custom_parameter);
    DM_DiagClientInterf::SendDiagData(response_json);
}
#endif

void DM_DiagClientInterf::ModifyParameters(std::string signalingContent, std::string signBase64,
                                           std::string protocolVersion, std::string taskId,Json::Value scriptRunCondition)
{
    Json::Value custom_parameter;
    std::string response_json;
    std::ofstream ofs;
    DiagFile diag_file_instance;

    //信令messageType200验签
    diag_file_instance.GetCryptoProvider();
    bool check_sign_result;
    check_sign_result = DM_DiagClientInterf::CheckSigalingSign(signalingContent,signBase64,diag_file_instance);
    //验签成功
    if(check_sign_result){
        LOGINFO <<  "verify signaling successful.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 201;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    //验签失败
    else{
        LOGERROR << "verify signaling failed.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 202;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }

    //修改json中原有信息
    ofs.open(CONDITION_CHECK_FILE_PATH);
    if (ofs.is_open()){
        std::string json_output = scriptRunCondition.toStyledString();
        ofs << json_output;

        //修改成功，返回结果，messageType = 201
        LOGINFO <<  "modify parameters successful.";
        custom_parameter["result"] = true;
        custom_parameter["failureReason"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(201,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    else{
        //修改失败，返回结果，messageType = 201
        LOGERROR << "modify parameters failed.";
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 999;

        response_json = DM_DiagClientInterf::CreateJsonResponse(201,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
}

void DM_DiagClientInterf::ModifyDiagEngineTimeParameters(std::string signalingContent, std::string signBase64,
                                                         std::string protocolVersion, std::string taskId,
                                                         u_int p2Client, u_int p2StarClient)
{
    Json::Value root;
    std::string response_json;
    Json::Value custom_parameter;
    std::string parameters_file_path = "";
    parameters_file_path = parameters_file_path + TIME_PARAMETER_FOLDER + "engineTimeParameter.json";
    std::ofstream parameters_file;
    DiagFile diag_file_instance;

    //信令messageType202验签
    diag_file_instance.GetCryptoProvider();
    bool check_sign_result;
    check_sign_result = DM_DiagClientInterf::CheckSigalingSign(signalingContent,signBase64,diag_file_instance);
    //验签成功
    if(check_sign_result){
        LOGINFO <<  "verify signaling successful.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 201;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    //验签失败
    else{
        LOGERROR << "verify signaling failed.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 202;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }

    //将时间参数写入/containers/NeusarApp/work/common/opt/DiagnosticScript/EngineTimeParameter/EngineTimeParameter.json中
    if(access(TIME_PARAMETER_FOLDER,0) == -1){
        LOGINFO <<  "/engineTimeParameter file does not exist,creating...";
        mkdir(TIME_PARAMETER_FOLDER,0777);

        if(access(TIME_PARAMETER_FOLDER,0) == 0){
            LOGINFO <<  "/engineTimeParameter create successful.";
        }
        else{
            //参数修改失败，messageType = 203
            LOGERROR << "/engineTimeParameter create failed.";
            custom_parameter["result"] = false;
            custom_parameter["failureReason"] = 999;

            response_json = DM_DiagClientInterf::CreateJsonResponse(203,protocolVersion,taskId,custom_parameter);
            DM_DiagClientInterf::SendDiagData(response_json);
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
        LOGINFO <<  "Engine time parameter change successful.";
        custom_parameter.clear();
        custom_parameter["result"] = true;
        custom_parameter["failureReason"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(203,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }
    else{
        //修改参数失败，messageType = 203
        LOGERROR << "Engine time parameter change failed.";
        custom_parameter.clear();
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 999;

        response_json = DM_DiagClientInterf::CreateJsonResponse(203,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
}

void DM_DiagClientInterf::DeleteLocalDiagFile(std::string signalingContent, std::string signBase64, std::string protocolVersion,
                                              std::string taskId, u_int fileType, std::string fileName)
{
    Json::Value custom_parameter;
    std::string response_json;
    DiagFile diag_file_instance;
    bool result = false;
    int failure_reason = 0;

    DIR* dir;
    struct dirent* dir_index;
    size_t index;
    std::string symbol = "";
    std::string file_name = "";
    std::string file_path = "";
    //string remove_command = "";

    //信令messageType400验签
    diag_file_instance.GetCryptoProvider();
    bool check_sign_result;
    check_sign_result = DM_DiagClientInterf::CheckSigalingSign(signalingContent,signBase64,diag_file_instance);
    //验签成功
    if(check_sign_result){
        LOGINFO <<  "verify signaling successful.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 201;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    //验签失败
    else{
        LOGERROR << "verify signaling failed.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 202;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }

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
            LOGERROR << "floder: " <<  VEHICLE_CONFIG_FOLDER << " does not exist!";
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
                    LOGINFO << "get ecuid_mtoc file name: " << file_name.c_str();
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
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << " does not exist!";
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
                    LOGINFO << "get OptionConfig file name: " << file_name.c_str();
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
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << " does not exist!";
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
                    LOGINFO <<  "get OptionCodeMap file name: " << file_name.c_str();
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
        LOGINFO <<  "delete " << file_path.c_str() << " failed,not exist!";
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
        LOGINFO <<  "delete file successful.";
        result = true;
        failure_reason = 0;
    }

    RESPONSE:
    //根据删除结果响应，messageType = 401
    custom_parameter["result"] = result;
    custom_parameter["failureReason"] = failure_reason;
    response_json = DM_DiagClientInterf::CreateJsonResponse(401,protocolVersion,taskId,custom_parameter);
    DM_DiagClientInterf::SendDiagData(response_json);
}

void DM_DiagClientInterf::UpdateLocalDiagFile(std::string signalingContent, std::string signBase64, std::string protocolVersion,
                                              std::string taskId, std::string packageUrl, int packageUrlSsl, u_int fileType)
{
    Json::Value custom_parameter;
    std::string response_json;
    DiagFile diag_file_instance;

    //信令messageType402验签
    diag_file_instance.GetCryptoProvider();
    bool check_sign_result;
    check_sign_result = DM_DiagClientInterf::CheckSigalingSign(signalingContent,signBase64,diag_file_instance);
    //验签成功
    if(check_sign_result){
        LOGINFO <<  "verify signaling successful.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 201;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    //验签失败
    else{
        LOGERROR << "verify signaling failed.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 202;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }
    
    uint8_t script_download_result;
    script_download_result = DM_DiagClientInterf::DiagFileDownload("TEMP",packageUrl,packageUrlSsl);

    //下载失败返回失败信息，messageType = 403
    if(script_download_result != 0x00){
        LOGERROR << "download package failed.";
        custom_parameter["result"] = false;
        custom_parameter["failureReason"] = 412;

        response_json = DM_DiagClientInterf::CreateJsonResponse(403,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }
    else{
        LOGINFO <<  "download package successful.";
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

        response_json = DM_DiagClientInterf::CreateJsonResponse(403,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }

    //处理后响应，messageType = 403
    custom_parameter.clear();
    custom_parameter["result"] = true;
    custom_parameter["failureReason"];

    response_json = DM_DiagClientInterf::CreateJsonResponse(403,protocolVersion,taskId,custom_parameter);
    DM_DiagClientInterf::SendDiagData(response_json);

    LOGINFO <<  "update file finish.";
}

void DM_DiagClientInterf::QueryFileInf(std::string signalingContent, std::string signBase64, std::string protocolVersion,
                                       std::string taskId, u_int fileType, std::string packageName)
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
    DiagFile diag_file_instance;

    //信令messageType500验签
    diag_file_instance.GetCryptoProvider();
    bool check_sign_result;
    check_sign_result = DM_DiagClientInterf::CheckSigalingSign(signalingContent,signBase64,diag_file_instance);
    //验签成功
    if(check_sign_result){
        LOGINFO <<  "verify signaling successful.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 201;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    //验签失败
    else{
        LOGERROR << "verify signaling failed.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 202;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }

    //根据fileType获取相应版本信息
    //混合类型,查询所有可查询文件
    if(fileType == 0){
        bool is_none = true;
        int time = 0;
        //数据库
        if(access(ODX_FLODER,0) != 0){
            LOGERROR << "floder: " << ODX_FLODER << "does not exist!";
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
                        if(file_name.find("_v") != std::string::npos){
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
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << "does not exist!";
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
                    LOGINFO <<  "get MTOConfig file name: " << file_name.c_str() << "version: " << version.c_str();
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
                    LOGINFO << "get ecuid_mtoc file name: " << file_name.c_str() << "version: " << version.c_str();
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
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << "does not exist!";
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
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << "does not exist!";
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
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0){
            LOGERROR << "floder: " << ODX_FLODER << " does not exist!";
        }
        else{
            int time = 0;
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
                    LOGINFO <<  "get Odx file name: " << file_name.c_str() << "version: " << version.c_str();
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
                    LOGINFO <<  "get MTOConfig file name: " << file_name.c_str() << "version: " << version.c_str();
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
                    LOGINFO <<  "get ecuid_mtoc file name: " << file_name.c_str() << "version: " << version.c_str();
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
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << "does not exist!";
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
            file_inf[0]["fileName"] = "";
            file_inf[0]["version"] = "";
        }
    }
    //选配代码映射文件
    else if(fileType == 9){
        symbol = "OptionCodeMap";
        if(access(VEHICLE_CONFIG_FOLDER,0) != 0){
            LOGERROR << "floder: " << VEHICLE_CONFIG_FOLDER << "does not exist!";
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

    response_json = DM_DiagClientInterf::CreateJsonResponse(501,protocolVersion,taskId,custom_parameter);
    DM_DiagClientInterf::SendDiagData(response_json);
}

void DM_DiagClientInterf::WriteVehicleData(const std::string& encryptVehicleDataBase64, const std::string& key0)
{
    //如信令中含有vehicleData信息，写入到相应的文件/DiagnosticScript/vehicleData/vehicle_data.json
    if(encryptVehicleDataBase64.size() != 0){
        std::string encrypt_vehicle_data;
        std::string key1;
        std::string vehicle_data_string;
        std::vector<uint8_t> encrypt_vehicle_data_byte;
        std::vector<uint8_t> vehicle_data_byte;
        std::wofstream vehicle_data_file;
        std::wofstream vehicle_data_backup_file;
        std::wstring vehicle_data_wstring;
        DiagFile diag_file_instance;

        encrypt_vehicle_data = Base64Decode(encryptVehicleDataBase64);
        key1 = diag_file_instance.ConvertKey(key0);
        for(auto iter : encrypt_vehicle_data){
            encrypt_vehicle_data_byte.push_back((uint8_t)iter);
            //printf("%02X",(uint8_t)iter);
        }
        std::string encrypt_vehicle_data_string(encrypt_vehicle_data_byte.begin(), encrypt_vehicle_data_byte.end());
        #if (defined USING_AY5G) || (defined USING_A19G1) || (defined USING_AY2G)
        auto vd_result = diag_file_instance.DecryptFileByAES(encrypt_vehicle_data_string, key1, key0);
        if (!vd_result.has_value())
        {
            LOGERROR << "DM_DiagClientInterf::WriteVehicleData: decrypt file by aes(encrypt_vehicle_data) failed.";
            return;
        }
        vehicle_data_string = vd_result.value();
        #else
        auto vd_result = diag_file_instance.DecryptFileByAES(encrypt_vehicle_data_string, key1);
        if (!vd_result.has_value())
        {
            LOGERROR << "DM_DiagClientInterf::WriteVehicleData: decrypt file by aes(encrypt_vehicle_data) failed.";
            return;
        }
        vehicle_data_string = vd_result.value();
        #endif

        // for(auto iter : vehicle_data_byte){
        //     vehicle_data_string.push_back(iter);
        // }

        vehicle_data_wstring = 
            std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(vehicle_data_string);
        //std::wcout << "get vehicle data:" << vehicle_data_wstring << endl;

        vehicle_data_file.open(VEHICLE_DATA_FILE_PATH);
        if(vehicle_data_file.is_open()){
            vehicle_data_file << vehicle_data_wstring;
            LOGINFO <<  "write vehicle data finish.";
        }
        else{
            LOGERROR << "write vehicle data failed!";
        }
        vehicle_data_file.close();

        vehicle_data_backup_file.open(VEHICLE_DATA_FILE_BACKUP_PATH);
        if(vehicle_data_backup_file.is_open()){
            vehicle_data_backup_file << vehicle_data_wstring;
            LOGINFO <<  "write vehicle backup data finish.";
        }
        else{
            LOGERROR << "write vehicle backup data failed!";
        }
        vehicle_data_backup_file.close();
    }
    //如果信令中不含有vehicleData信息，使用备份的vehicle_data.json
    else{
        // SystemCmd file_opreation_instance;
        std::ifstream vehicle_data_file;
        Json::Reader reader;
        Json::Value root;

        //当/shared/VehicleData/vehicle_data.json存在并且可以解析时，复制到脚本解析指定目录下
        if(access(VEHICLE_DATA_FILE_BACKUP_PATH,0) == -1){
            LOGERROR << "/shared/VehicleData/vehicle_data.json file does not exist,can not copy.";
            return;
        }
        else{
            vehicle_data_file.open(VEHICLE_DATA_FILE_BACKUP_PATH);
            if (!reader.parse(vehicle_data_file, root, false)){
                LOGERROR << "parse /shared/VehicleData/vehicle_data.json file failed!";
            }
            else{
                file_opreation_instance.deleteFile(VEHICLE_DATA_FILE_PATH);
                file_opreation_instance.copy_file(VEHICLE_DATA_FILE_BACKUP_PATH,VEHICLE_DATA_FILE_FLODER);
                LOGINFO <<  "copy vehicle_data.json finish.";
            }
            vehicle_data_file.close();
        }
    }
}

uint8_t DM_DiagClientInterf::DiagFileDownload(const std::string& diagFileName, const std::string& fileUrl, int authMode)
{
    /* kevin http */
    LOGINFO <<  "start to download the script package...";

    //创建存放压缩包文件夹
    if(access(TEMP_STORAGE_FOLDER,0) == -1){
        LOGINFO <<  "/temp floder does not exist,creating...";
        mkdir(TEMP_STORAGE_FOLDER,0777);

        if(access(TEMP_STORAGE_FOLDER,0) == 0){
            LOGINFO <<  "/temp floder create successful";
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
    pCHttpExport->SetRequestType(Type::DOWNLOAD);
    pCHttpExport->SetUrlPath(fileUrl);
    pCHttpExport->SetFileOperatePtr(file_adapter);
    pCHttpExport->SetConnectTimeOut(300);
    pCHttpExport->SetTimeOut(600);
    pCHttpExport->SetTries(1);
    //pCHttpExport->SetIsUerLocalCertificate(true);
    if(authMode == 1){
        pCHttpExport->SetAuthsMode(AuthsModeTypeEN::TOW_WAY);
        LOGINFO <<  "set bidirectional authentication mode.";
    }
    else if(authMode == 2){
        pCHttpExport->SetAuthsMode(AuthsModeTypeEN::ONE_WAY);
        LOGINFO <<  "set unidirectional authentication mode.";
    }
    else{
        LOGINFO <<  "set no authentication mode.";
    }
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
    LOGINFO <<  "download successful.";
#else
    return DownloadResult::DownloadFailed;
#endif

    return DownloadResult::DownloadSuccessful;
}

uint8_t DM_DiagClientInterf::DiagScriptRunResultUpload(const std::string& fileName, const std::string& resultPackageNameAndPath,
                                                       const std::string& resultURL, int authMode)
{
    LOGINFO <<  "start to upload diag result package...";
#ifdef HTTP_WRAPPER
    //文件名
    //创建http传输接口
#ifdef USE_OPENSSL
    auto pHttpClient = std::make_unique<HttpClient>(HttpVerTypeEN::OPEN_SSL_HTTP);
#else
    auto pHttpClient = std::make_unique<HttpClient>();
#endif
    //设置请求类型
    pHttpClient->SetRequestType(Type::UPLOADBYPOST);
    //设置post请求地址URL
    pHttpClient->SetUrlPath(resultURL);
    //设置本地路径
    //pHttpClient->SetLocalFilePath(file_path_);
    pHttpClient->SetLocalFilePath(resultPackageNameAndPath);
    //设置本地文件名称
    pHttpClient->SetLocalFileName(fileName);
    //设置连接超时时间（秒S）
    pHttpClient->SetConnectTimeOut(300);
    //设置返回超时时间（秒S）
    pHttpClient->SetTimeOut(6000);
    //设置双向认证
    //pHttpClient->SetAuthsMode(AuthsModeTypeEN::TOW_WAY);
    if(authMode == 1){
        pHttpClient->SetAuthsMode(AuthsModeTypeEN::TOW_WAY);
        LOGINFO <<  "set bidirectional authentication mode.";
    }
    else if(authMode == 2){
        pHttpClient->SetAuthsMode(AuthsModeTypeEN::ONE_WAY);
        LOGINFO <<  "set unidirectional authentication mode.";
    }
    else{
        LOGINFO <<  "set no authentication mode.";
    }
    // pHttpClient->SetAuthsMode(AuthsModeTypeEN::NO_WAY);
    // pHttpClient->SetAuthsMode(AuthsModeTypeEN::ONE_WAY);
    //设置头信息
    std::map<std::string, std::string> head;
    head["Content-Type"] = "multipart/form-data";
    pHttpClient->SetHeaders(std::move(head));
    // // //设置body体数据
    std::map<std::string, std::string> body;
    body["file"] = "upload"; 
    body["filename"] = fileName;

    pHttpClient->SetBodys(move(body));
    //执行下载
    (void)pHttpClient->Call();    
    //获取执行是否成功
    (void)pHttpClient->IsSucceed();
    //获取curl返回值
    (void)pHttpClient->GetResponseCode();
    //获取云端返回信息
    (void)pHttpClient->ResponseDataAsString();
    //获取curl返回信息
    (void)pHttpClient->GetErrorBuffer();
    //获取云端返回信息

    if(pHttpClient->IsSucceed() && (pHttpClient->GetResponseCode()==200))
    {
        LOGINFO << "upload success";
        return UPloadResult::UploadSuccessful;
    }
    else
    {
        LOGERROR << "upload failed";
        return UPloadResult::UploadFailed;
    }    
#endif
    return UPloadResult::UploadSuccessful;
}

time_t DM_DiagClientInterf::GetTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp = 
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    long timestamp =  tp.time_since_epoch().count();
    return timestamp;
}

std::string DM_DiagClientInterf::CreateJsonResponse(int messageType, std::string protocolVersion,
                                                    std::string taskId,Json::Value customParameter)
{
    Json::Value response_root;
    time_t timestamp;
    std::string response_json;

    timestamp = DM_DiagClientInterf::GetTimeStamp();
    response_root["messageType"] = messageType;
    response_root["protocolVersion"] = protocolVersion;
    response_root["timestamp"] = timestamp;
    response_root["taskId"] = taskId;
    response_root["customParameter"] = customParameter;

    response_json = response_root.toStyledString();
    LOGINFO <<  "create response Json string: " << response_json.c_str();
    return response_json;
}

std::string DM_DiagClientInterf::CombineSignalingContent(std::string messageType, std::string protocolVersion,
                                                         long timeStamp, std::string taskId, std::string customParameter)
{
    std::string result = "";
    std::string timestamp_string = std::to_string(timeStamp);
    result = "messageType=" + messageType +
             "&protocolVersion=" + protocolVersion +
             "&timestamp=" + timestamp_string +
             "&taskId=" + taskId +
             "&customParameter=" + customParameter;
    return result;
}

bool DM_DiagClientInterf::CheckSigalingSign(std::string signalingContent, std::string signBase64, DiagFile &diagFile)
{
    LOGINFO <<  "checking signaling sign...";

    //size_t index = 0;
    //char temp;
    std::ifstream public_key_file;
    std::string public_key;
    // std::vector<uint8_t> encrypt_sign_byte;
    std::string sign_string;
    std::vector<uint8_t> signaling_content_byte;
    std::vector<uint8_t> signal_sha256;

    //计算信令的sha256
    for(auto iter : signalingContent){
        signaling_content_byte.push_back((uint8_t)iter);
    }

    signal_sha256 = diagFile.CalculateFileSha256(signaling_content_byte);

    //对信令中的sign进行RSA解密
    std::vector<unsigned char> dst_vec;
    auto base64_result = diagFile.Base64Decode(signBase64, dst_vec);
    if (base64_result == false) return false;
    std::string encrypt_sign((char*)dst_vec.data(), dst_vec.size());
    auto pr_result = diagFile.PublickeyRead(public_key);
    if(pr_result){
        LOGINFO <<  "get public key,size: " << public_key.size() << " bytes";
    }
    else{
        LOGERROR << "error,can not open public key file.";
        return false;
    }
    #if 0
    auto sign_result = diagFile.DecryptFileByRSA(dstData, public_key);
    if (!sign_result.has_value())
    {
        LOGERROR << "DM_DiagClientInterf::CheckSigalingSign: decrypt file by rsa failed.";
        return false;
    }
    sign_string = sign_result.value();
    #endif
    sign_string = encrypt_sign;
    std::string signal_sha256_string(signal_sha256.begin(), signal_sha256.end());
    //比对签名内容
    if (sign_string != signal_sha256_string)
    {
        LOGERROR << "failed to compare the signature value.";
        return false;
    }

    LOGINFO <<  "the signaling signature is successfully verified.";
    return true;
}

void DM_DiagClientInterf::SendInteractResponseParameter(std::string signalingContent, std::string signBase64,
                                                        std::string protocolVersion, std::string taskId, std::string packageName,
                                                        Json::Value interactResponseParameter)
{
    Json::Value custom_parameter;
    std::string response_json;
    DiagFile diag_file_instance;

    //信令messageType106验签
    diag_file_instance.GetCryptoProvider();
    bool check_sign_result;
    check_sign_result = DM_DiagClientInterf::CheckSigalingSign(signalingContent,signBase64,diag_file_instance);
    //验签成功
    if(check_sign_result){
        LOGINFO <<  "verify signaling successful.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 201;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
    }
    //验签失败
    else {
        LOGERROR << "verify signaling failed.";
        custom_parameter["runType"] = 601;
        custom_parameter["runState"] = 202;
        custom_parameter["message"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(101,protocolVersion,taskId, custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        return;
    }

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
            interact_state.response_parameter.input_value
                .push_back(interactResponseParameter["inputValues"][i].asString());
        }
    }
    //selectionList
    else if(strcmp(interact_type.c_str(),"3") == 0) {
        interact_state.response_parameter.interact_type = interact_type;
        interact_state.response_parameter.result = result;
        interact_state.response_parameter.selected_index = interactResponseParameter["selectedIndex"].asInt();
    }
    //information
    else {
        interact_state.response_parameter.interact_type = interact_type;
    }

    interact_state.is_response = true;
    LOGINFO <<  "transmit interaction response parameter finish.";
}

void DM_DiagClientInterf::SendDiagData(const std::string diagData)
{
    bool reconnect = true;
    vc_proxy_instance->ReportDiagnosisData(diagData, reconnect);
}

void DM_DiagClientInterf::Pn2EcuName(std::string &pnStr, std::string &ecuStr)
{
    Json::Reader reader;
    Json::Value root;
    std::ifstream ifs;
    std::string pn_symbol;
    std::string temp;

    #if (defined USING_A02GH)
    std::string a02gh_pn_json = A02GH_PN_CODE;
    if (!reader.parse(a02gh_pn_json, root)){
        LOGERROR << "parse a02gn sa json string failed,can not correspond to ecu name!";
        return;
    }
    #elif (defined USING_A66)
    std::string a66_pn_json = A66_PN_CODE;
    if (!reader.parse(a66_pn_json, root)){
        LOGERROR << "parse a66 sa json string failed,can not correspond to ecu name!";
        return;
    }
    #else
    ifs.open(SA_MAP_FILE_PATH);
    root.clear();
    if (!reader.parse(ifs, root)){
        LOGERROR << "parse sa json failed,can not correspond to ecu name!";
        return;
    }
    #endif

    //索引12位PN
    pn_symbol = pnStr.substr(0,12);
    Json::Value::Members member = root.getMemberNames();
    for(Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter){
        if(root[*iter].size() == 0){
            temp = root[*iter].asString().substr(0,12);
            if(pn_symbol == temp){
                LOGINFO <<  "get ECU name: " << (*iter).c_str();
                ifs.close();
                ecuStr = std::string(*iter);
                return;
            }
        }
        else{
            for(u_int i = 0; i != root[*iter].size(); i++){
                temp = root[*iter][i].asString().substr(0,12);
                if(pn_symbol == temp){
                    LOGINFO <<  "get ECU name: " << (*iter).c_str();
                    ifs.close();
                    ecuStr = std::string(*iter);
                    return;
                }
            }
        }
    }
    temp.clear();
    pn_symbol.clear();

    //索引PN前7位
    pn_symbol = pnStr.substr(0,7);
    for(Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter){
        if(root[*iter].size() == 0){
            temp = root[*iter].asString().substr(0,7);
            if(pn_symbol == temp){
                LOGINFO <<  "get ECU name: " << (*iter).c_str();
                ifs.close();
                ecuStr = std::string(*iter);
                return;
            }
        }
        else{
            for(int i = 0; i != static_cast<int>(root[*iter].size()); i++){
                temp = root[*iter][i].asString().substr(0,7);
                if(pn_symbol == temp){
                    LOGINFO <<  "get ECU name: " << (*iter).c_str();
                    ifs.close();
                    ecuStr = std::string(*iter);
                    return;
                }
            }
        }
    }

    if(strcmp(ecuStr.c_str(),"") == 0){
        LOGERROR << "the PN code does not correspond to ecu name!";
    }
    ifs.close();
}
// 对比公钥哈希值
bool DM_DiagClientInterf::CheckPublicKeySha256(std::string publicKeySha256)
{
    std::ifstream public_key_sha256_file;
    std::string local_public_key_sha256 = "";
    char temp;

    public_key_sha256_file.open(PUBLIC_KEY_SHA256_PATH,std::ios::binary | std::ios::in);
    if(public_key_sha256_file.is_open()) {
        while(public_key_sha256_file.get(temp)){
            local_public_key_sha256.push_back(temp);
        }
        public_key_sha256_file.close();

        if(local_public_key_sha256.size() != 64){
            LOGERROR << "local public key sha256 value size is wrong,size: " << local_public_key_sha256.size();
            return false;
        }

        for(uint i = 0; i < local_public_key_sha256.size(); i++){
            if(local_public_key_sha256[i] != publicKeySha256[i]){
                LOGERROR << "failed to compare sha256 value of the local public key.";
                return false;
            }
        }
        return true;
    }
    else{
        LOGERROR << "can not open public key sha256 file.";
        return false;
    }
}

void DM_DiagClientInterf::ReceiveAndSavePublicKey(std::string protocolVersion, std::string taskId,
                                                  std::string publicKey, std::string publicKeySha256,
                                                  bool pkiSign, std::string pkiSignValue, std::string pkiSigner)
{
    Json::Value custom_parameter;
    //ofstream  public_key_file;
    std::ofstream public_key_sha256_file;
    DiagFile diag_file_instance;
#if 0
    bool check_pki_signer_result = false;
    bool check_pki_sign_value_result = false;
    bool check_pki_sign_result = false;
#endif
    bool save_public_key_result = false;
    std::string response_json = "";

    if(access(KEY_FOLDER,0) == -1){
        LOGINFO <<  "/key file does not exist,creating...";
        mkdir(KEY_FOLDER,0777);

        if(access(KEY_FOLDER,0) == 0){
            LOGINFO <<  "/key create succeed.";
        }
        else{
            LOGERROR << "/key floder create failed, key folder is " << KEY_FOLDER;
            custom_parameter["result"] = false;
            custom_parameter["failedReason"] = "/key floder create failed";

            response_json = DM_DiagClientInterf::CreateJsonResponse(3,protocolVersion,taskId,custom_parameter);
            DM_DiagClientInterf::SendDiagData(response_json);
            return;
        }
    }

    //验证公钥签名(autox项目不使用公钥签名，crypto_wrapper没适配VerifyCertChainSign接口)
#if 0
    if(pkiSign){
        LOGINFO <<  "public key need to be verified.";

#if 0
        check_pki_signer_result = diag_file_instance.VerifyCertificate(pkiSigner);
        check_pki_sign_value_result = diag_file_instance.VerifySign(publicKey, pkiSignValue);
        check_pki_sign_result = ((check_pki_signer_result == true) && (check_pki_sign_value_result == true));
#else
        check_pki_sign_result = diag_file_instance.VerifyCertChainSign(pkiSigner, publicKey, pkiSignValue);
#endif
        if(check_pki_sign_result) {
            LOGINFO <<  "check public key sign passed.";
        }
        else{
            custom_parameter["result"] = false;
            custom_parameter["failedReason"] = "failed to verify signature information";

            response_json = DM_DiagClientInterf::CreateJsonResponse(3,protocolVersion,taskId,custom_parameter);
            DM_DiagClientInterf::SendDiagData(response_json);
            LOGERROR << "check public key sign failed.";
            return;
        }
    }
#endif

    //存储公钥哈希值
    public_key_sha256_file.open(PUBLIC_KEY_SHA256_PATH,std::ios::out);
    if(public_key_sha256_file.is_open()) {
        public_key_sha256_file << publicKeySha256;
    }
    else{
        custom_parameter["result"] = false;
        custom_parameter["failedReason"] = "can not save public key sha256 value";

        response_json = DM_DiagClientInterf::CreateJsonResponse(3,protocolVersion,taskId,custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        LOGERROR << "can not save public key sha256 value.";
        return;
    }
    public_key_sha256_file.close();

    //公钥写入文件存入本地
    //直接存储
    /*
    public_key_file.open(PUBLIC_KEY_PATH);
    if(public_key_file.is_open()){
        public_key_file << publicKey;
        cout << "[DM_DiagClientInterf::ReceivePublicKey]write public key data finish." << endl;
    }
    else{
        cout << "[DM_DiagClientInterf::ReceivePublicKey]write public key data failed." << endl;
    }
    public_key_file.close();
    */
    //写入存储public key
    save_public_key_result = diag_file_instance.PublickeyWrite(publicKey);
    if(save_public_key_result == false){
        custom_parameter["result"] = false;
        custom_parameter["failedReason"] = "write public key failed";

        response_json = DM_DiagClientInterf::CreateJsonResponse(3,protocolVersion, taskId, custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        LOGERROR << "write public key failed.";
    }
    else {
        custom_parameter["result"] = true;
        custom_parameter["failedReason"];

        response_json = DM_DiagClientInterf::CreateJsonResponse(3, protocolVersion, taskId, custom_parameter);
        DM_DiagClientInterf::SendDiagData(response_json);
        LOGINFO <<  "write public key finish.";
    }
}

std::string DM_DiagClientInterf::SerializeDiagTaskUserAuthRequest(const st_DiagTaskUserAuthRequest& request) {
    LOGINFO <<  "SerializeDiagTaskUserAuthRequest taskid: " << request.taskid << " taskdesc: " << request.taskdesc;
    uint32_t taskidLength = static_cast<uint32_t>(request.taskid.size());
    uint32_t taskdescLength = static_cast<uint32_t>(request.taskdesc.size());
    std::string result;
    // 添加taskid长度
    LOGINFO <<  "SerializeDiagTaskUserAuthRequest taskid length: " << taskidLength;
    result.push_back(static_cast<uint8_t>(taskidLength >> 24));
    result.push_back(static_cast<uint8_t>(taskidLength >> 16));
    result.push_back(static_cast<uint8_t>(taskidLength >> 8));
    result.push_back(static_cast<uint8_t>(taskidLength));
    // 添加taskid内容
    result.insert(result.end(), request.taskid.begin(), request.taskid.end());
    // 添加taskdesc长度
    LOGINFO <<  "SerializeDiagTaskUserAuthRequest taskdesc length: " << taskdescLength;
    result.push_back(static_cast<uint8_t>(taskdescLength >> 24));
    result.push_back(static_cast<uint8_t>(taskdescLength >> 16));
    result.push_back(static_cast<uint8_t>(taskdescLength >> 8));
    result.push_back(static_cast<uint8_t>(taskdescLength));
    // 添加taskdesc内容
    result.insert(result.end(), request.taskdesc.begin(), request.taskdesc.end());
    return result;
}

DM_DiagClientInterf::st_DiagTaskUserAuthResponse DM_DiagClientInterf::DeserializeDiagTaskUserAuthResponse(const std::string& response) {
    st_DiagTaskUserAuthResponse result;
    // 提取 taskid 长度
    uint32_t taskidLength = (static_cast<uint32_t>(response[0]) << 24) |
                            (static_cast<uint32_t>(response[1]) << 16) |
                            (static_cast<uint32_t>(response[2]) << 8) |
                            static_cast<uint32_t>(response[3]);
    LOGINFO <<  "DeserializeDiagTaskUserAuthResponse taskid length: " << taskidLength;
    if (taskidLength + 5 != response.size()) { //+5是因为前4字节固定表示taskid长度，最后1字节固定标识授权码
        LOGERROR <<  "DeserializeDiagTaskUserAuthResponse taskid length error.";
        return result;
    }
    // 提取 taskid 内容
    result.taskid = response.substr(4, taskidLength);
    // 提取 auth_code
    result.auth_code = static_cast<uint8_t>(response[4 + taskidLength]);
    LOGINFO <<  "DeserializeDiagTaskUserAuthResponse taskid: " << result.taskid << " auth_code: " << result.auth_code;
    return result;
}
