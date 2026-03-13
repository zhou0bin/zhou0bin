/*****************************************************************************
* @file     OTA_interaction.cpp
* @brief    OTA Interface
* @date     2025/4/24
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

#include "interface/ota_interaction.h"
#include "3rdparty/diag_script_parsing.h"
#include "common/macro.h"

#ifdef OTA_INTERACTION
namespace asf {
namespace vdi {

void OtaInteraction::Init()
{
    //初始化服务
    LOGINFO << "OtaInteraction::Init: Offer service:OTA_VTAP_Interaction(110b).";
    this->OfferService();
}

void OtaInteraction::Release()
{
    this->StopOfferService();
}

auto OtaInteraction::OTAStartVTAPRunScript(
    const ::ara::com::vsomeip::ReqOTAStartVTAPRunScript& ReqOTAStartVTAPRunScript)
                                -> decltype(OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))
{
    OtaInteraction::OTAStartVTAPRunScriptOutput output;
    std::vector<std::string> ecu_package_name;
    std::vector<std::string> script_path;
    size_t pos;
    std::string s_temp;
    char c_temp;

    diag_file_instance.GetCryptoProvider();

    //初始化脚本运行状态，以及启动状态
    run_state = ScriptRunState::UnknowError;
    output.AckOTAStartVTAPRunScript = OtaRequestAck::Success;

    //判断上一个任务是否已完成
    if(is_in_ota_task){
        LOGERROR << "OtaInteraction::OTAStartVTAPRunScript: last request is not over!";
        output.AckOTAStartVTAPRunScript = OtaRequestAck::LastTaskNotOver;
        decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
        promise.set_value(std::move(output));
        return promise.get_future();
    }

    //解析json获取压缩包名
    Json::Value root;
    Json::Reader reader;

    if(reader.parse(ReqOTAStartVTAPRunScript,root)){
        Json::Value::Members member = root.getMemberNames();
        for(Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter){
            s_temp = root[*iter].asString();
            ecu_package_name.push_back(s_temp);
            LOGINFO << "OtaInteraction::OTAStartVTAPRunScript: get ecu package name: " << s_temp.c_str();
            s_temp.clear();
        }

    }
    else{
        LOGERROR << "OtaInteraction::OTAStartVTAPRunScript: parse ReqOTAStartVTAPRunScript failed!";
        output.AckOTAStartVTAPRunScript = OtaRequestAck::UnKnown;
        decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
        promise.set_value(std::move(output));
        return promise.get_future();
    }

    //如果存在临时脚本文件先进行清除操作
    file_opreation_instance.deleteDir(TEMP_SCRIPT_FOLDER,false);

    //检测CCU内是否存在请求的脚本包,并解压文件
    std::string script_package_path;

    for(auto iter : ecu_package_name){
        script_package_path = OTA_CONFIG_FILES_FLODER + iter;
        if(!this->IsScriptFileExist(script_package_path)){
            output.AckOTAStartVTAPRunScript = OtaRequestAck::ScriptNotExist;
            decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
            promise.set_value(std::move(output));
            return promise.get_future();
        }

        // diag_file_instance.ExtractZip(script_package_path,OTA_CONFIG_FILES_FLODER);
        diag_file_instance.ExtractZip(script_package_path,TEMP_SCRIPT_FOLDER);

        script_package_path.clear();
    }

    //获取所有主脚本path
    struct dirent* dir_index = nullptr;
    DIR* dir = nullptr;
    std::vector<std::string> script_floder;
    std::string file_name;

    for(auto iter : ecu_package_name){
        s_temp.clear();
        pos = iter.find(".zip");
        if(pos != std::string::npos){
            //s_temp =  OTA_CONFIG_FILES_FLODER + iter.substr(0,pos) + "/";
            s_temp =  TEMP_SCRIPT_FOLDER + iter.substr(0,pos) + "/";
            script_floder.push_back(s_temp);
        }
        else{
            LOGERROR << "OtaInteraction::OTAStartVTAPRunScript: wrong ecu package name.";
            output.AckOTAStartVTAPRunScript = OtaRequestAck::UnKnown;
            decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
            promise.set_value(std::move(output));
            return promise.get_future();
        }
    }

    for(auto iter : script_floder){
        dir = opendir(iter.c_str());
        if(dir != nullptr){
            while((dir_index = readdir(dir)) != nullptr){
                file_name.clear();
                file_name = dir_index->d_name;
                if((strcmp(file_name.c_str(),".") != 0) && (strcmp(file_name.c_str(),"..") != 0)){
                    if(file_name.find(".py") != std::string::npos){
                        script_path.push_back(iter + file_name);
                        LOGINFO << "OtaInteraction::OTAStartVTAPRunScript: get script path: " << (iter + file_name).c_str();
                    }

                }
            }
        }
        else{
            LOGERROR << "OtaInteraction::OTAStartVTAPRunScript: open ecu script package failed!";
            output.AckOTAStartVTAPRunScript = OtaRequestAck::DecompressZipFailed;
            decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
            promise.set_value(std::move(output));
            return promise.get_future();
        }

        if(dir){
            closedir(dir);
        }
    }

    //验证脚本的md5
    // std::vector<std::string> script_path;
    std::vector<uint8_t> script_data;
    std::vector<uint8_t> standard_script_md5;
    std::vector<uint8_t> temp_md5;
    bool compare_result;

    if(ecu_package_name.size() > size_t(1)){
        // for(auto iter : ecu_package_name){
        //     s_temp.clear();
        //     pos = iter.find(".zip");
        //     if(pos != std::string::npos){
        //         s_temp =  OTA_CONFIG_FILES_FLODER + iter.substr(0,pos) + "/guid.py";
        //         script_path.push_back(s_temp);
        //     }
        //     else{
        //         LOGERROR << "wrong ecu package name.");
        //         output.AckOTAStartVTAPRunScript = OtaRequestAck::UnKnown;
        //         decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
        //         promise.set_value(std::move(output));
        //         return promise.get_future();
        //     }
        // }

        std::ifstream fin[script_path.size()];
        fin[0].open(script_path[0],std::ios::binary | std::ios::in);
        if(fin[0].is_open()){
            while(fin[0].get(c_temp)){
                script_data.push_back((uint8_t)c_temp);
            }
            fin[0].close();

            standard_script_md5 = diag_file_instance.CalculateFileMd5(script_data);
            for(size_t i = 1; i < script_path.size(); i++){
                script_data.clear();
                temp_md5.clear();
                fin[i].open(script_path[i],std::ios::binary | std::ios::in);
                if(fin[i].is_open()){
                    while(fin[i].get(c_temp)){
                        script_data.push_back((uint8_t)c_temp);
                    }
                    fin[i].close();

                    temp_md5 = diag_file_instance.CalculateFileMd5(script_data);
                    compare_result = std::equal(standard_script_md5.begin(),standard_script_md5.end(),
                                                temp_md5.begin(),temp_md5.end());
                    if(!compare_result){
                        LOGERROR << "OtaInteraction::OTAStartVTAPRunScript: the md5 values of the script files are inconsistent!";
                        output.AckOTAStartVTAPRunScript = OtaRequestAck::DecompressZipFailed;
                        decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
                        promise.set_value(std::move(output));
                        return promise.get_future();
                    }
                }
                else{
                    LOGERROR << "OtaInteraction::OTAStartVTAPRunScript: open script file failed!";
                    output.AckOTAStartVTAPRunScript = OtaRequestAck::DecompressZipFailed;
                    decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
                    promise.set_value(std::move(output));
                    return promise.get_future();
                }
            }
        }
        else{
            LOGERROR << "OtaInteraction::OTAStartVTAPRunScript: open script file failed!";
            output.AckOTAStartVTAPRunScript = OtaRequestAck::DecompressZipFailed;
            decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
            promise.set_value(std::move(output));
            return promise.get_future();
        }
    }

    //修改主脚本名
    std::string script_old_name;
    std::string script_new_name;

    s_temp.clear();
    pos = ecu_package_name[0].find(".zip");
    s_temp = ecu_package_name[0].substr(0,pos);
    script_old_name = script_path[0];
    script_new_name = script_floder[0] + s_temp + ".py";

    if(!this->IsScriptFileExist(script_old_name)){
        LOGERROR << "can not rename script file!";

        output.AckOTAStartVTAPRunScript = OtaRequestAck::UnKnown;
        decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
        promise.set_value(std::move(output));
        return promise.get_future();
    }
    else{
        int rename_result = rename(script_old_name.c_str(),script_new_name.c_str());
        if(rename_result == 0){
            LOGINFO << "OtaInteraction::OTAStartVTAPRunScript: rename " << script_old_name.c_str() << " to " << script_new_name.c_str();         
        }
        else{
            LOGERROR << "rename script file failed.";
            output.AckOTAStartVTAPRunScript = OtaRequestAck::UnKnown;
            decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
            promise.set_value(std::move(output));
            return promise.get_future();
        }

    }

    script_name = s_temp;
    LOGINFO << "the name of the script that triggers the call : " << script_name.c_str();


    //将解压后的脚本包，复制到脚本解析/temporary目录下
    /*
    for(auto iter : ecu_package_name){
        s_temp.clear();
        pos = iter.find(".zip");
        if(pos != std::string::npos){
            s_temp =  OTA_CONFIG_FILES_FLODER + iter.substr(0,pos);
            file_opreation_instance.copy_file(s_temp,TEMP_SCRIPT_FOLDER);
        }
        else{
            LOGERROR << "wrong ecu package name.");
            output.AckOTAStartVTAPRunScript = OtaRequestAck::UnKnown;
            decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
            promise.set_value(std::move(output));
            return promise.get_future();
        }
    }
    */

    //开启执行脚本任务
    /*
    std::thread maintain_ota_task_status(&OtaInteraction::MaintainOtaTaskStatus,this);
    maintain_ota_task_status.detach();
    */

    std::thread ota_task_process_thread(&OtaInteraction::OtaTaskProcess,this);
    ota_task_process_thread.detach();

    output.AckOTAStartVTAPRunScript = OtaRequestAck::Success;
    decltype(Skeleton::OTAStartVTAPRunScript(ReqOTAStartVTAPRunScript))::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

auto OtaInteraction::OTAGetVTAPScriptRunState(
    const ::ara::com::vsomeip::OTAGetVTAPScriptRunState& OTARequestScriptRunState)
                                -> decltype(OTAGetVTAPScriptRunState(OTARequestScriptRunState))
{
    OtaInteraction::OTAGetVTAPScriptRunStateOutput output;

    if(!is_in_ota_task) {
        LOGERROR << "there is no active task!";
        //output.VTAPResponseScriptResult = ScriptRunState::UnknowError;
    }
    else {
        LOGINFO << "OtaInteraction::OTAStartVTAPRunScript: run state: " << static_cast<uint>(run_state);
    }

    output.VTAPResponseScriptResult = run_state;
    decltype(Skeleton::OTAGetVTAPScriptRunState(OTARequestScriptRunState))::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();  
}

void OtaInteraction::OtaTaskProcess()
{
    Json::Reader reader;
    Json::Value root;
    std::string socket_file_path;
    std::string interact_file_path;
    std::ifstream ifs;
    int start_result;
    int send_result;

    ErrorType process_result;
    struct timespec timer;
    clock_gettime(CLOCK_REALTIME,&timer);
    clock_t start_script_run_time = timer.tv_sec;
    clock_t current_time;
    u_int script_run_time = 0;
    // u_int wait_interaction_finish_time = 0;

    //设置ota任务状态
    is_in_ota_task = true;
    /* kevin 条件检查
    //调用配电模块进行智能配电操作
    condition_check->SetVTState(0x01);
    condition_check->SetIEPowerState(static_cast<uint16_t>(OTA_TASK_MAX_TIME),true);
    */
    /*-----------------------------------------------调用脚本解析模块执行任务--------------------------------------------------*/
    LOGINFO << "OtaInteraction::OtaTaskProcess: start to invoke the script parsing module...";

    ifs.open(SOCKET_CONFIG_FILE_PATH);
    if(!reader.parse(ifs,root,false)){
        LOGERROR << "parse socket parameters file failed!";
        is_in_ota_task = false;
        run_state = ScriptRunState::ExecuteFailed;
        return;
    }
    else{
        socket_file_path = root["Socket_Pip_File"].asString();
        interact_file_path = root["Interact_Pip_File"].asString();
        LOGINFO << "OtaInteraction::OtaTaskProcess: open socket file,path: " << socket_file_path.c_str() 
                << " ,open interact socket file,path: " << interact_file_path.c_str();
    }
    ifs.close();

    //如果存在结果文件先进行清除操作
    file_opreation_instance.deleteDir(DIAG_RESULT_FILE_FLODER, false);

    //启动脚本解析模块
    start_result = script_process_instance->Start(socket_file_path.c_str());
    LOGINFO << "OtaInteraction::OtaTaskProcess: connectting script parsing module,start: " << start_result;

    data.itype = kCloud;
    data.guid = const_cast<char*>(script_name.c_str());
    send_result = script_process_instance->Send(&data);
    LOGINFO << "OtaInteraction::OtaTaskProcess: sending script parsing module script name: " << data.guid;
    LOGINFO << "OtaInteraction::OtaTaskProcess: sending script parsing module data,send: " << send_result;

    while(SCRIPT_RUNING){
        //收到退出信号，结束线程
        if(global_stop_flag){
            return;
        }

        //监听脚本执行状态
        //判断是否超时
        clock_gettime(CLOCK_REALTIME,&timer);
        current_time = timer.tv_sec;
        script_run_time = (u_int)(current_time - start_script_run_time);

        LOGINFO << "OtaInteraction::OTAStartVTAPRunScript: script running spend times: " << script_run_time;
        if (script_run_time >= OTA_TASK_MAX_TIME) {
            //脚本运行超时，停止脚本运行
            data.itype = kStop;
            data.guid = const_cast<char*>(script_name.c_str());
            send_result = script_process_instance->Send(&data);
            run_state = ScriptRunState::ExecuteFailed;

            LOGERROR << "OtaInteraction::OtaTaskProcess: script execution timeout.";
            LOGINFO << "OtaInteraction::OtaTaskProcess: sending script parsing module stop message,send: " << send_result;
            break;
        }

        std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
        process_result = script_process_instance->ReceiveDatas(script_name.c_str());
        LOGINFO << "OtaInteraction::OtaTaskProcess: script running state: " << process_result;

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
        */
        if(process_result == kExcutedOver){
            //判断是否有交互信息读取并未处理上传完成
            // while(!is_read_interaction_message_finish){
            //     if(wait_interaction_finish_time >= OTA_TASK_MAX_TIME - script_run_time){
            //         LOGERROR <<
            //                        "wait for interaction message processing too long,upload the result.");
            //         break;
            //     }
            //     Print_Location(LOG_ID, INFO, "%s",
            //                    "interaction message in the buffer has not been processed yet,processing....");
            //     std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
            //     wait_interaction_finish_time++;
            // }

            //解析DiagResult.json文件，查看脚本运行结果
            if(!this->IsScriptFileExist(DIAG_RESULT_JSON_FILE_PATH)){
                run_state = ScriptRunState::ExecuteFailed;
                LOGERROR << "OtaInteraction::OtaTaskProcess: DiagResult.json does not exist.";
                break;
            }
            else{
                std::string total_result = "";
                Json::Value result;

                ifs.open(DIAG_RESULT_JSON_FILE_PATH);
                if(ifs.is_open()){
                    if (reader.parse(ifs,result)){
                        total_result = result["Result"]["TotalResult"].asString();
                    }
                    else{
                        LOGERROR << "OtaInteraction::OtaTaskProcess: can not parse DiagResult.json.";
                        run_state = ScriptRunState::ExecuteFailed;
                        break;
                    }
                }
                else{
                    LOGERROR << "OtaInteraction::OtaTaskProcess: open DiagResult.json failed.";
                    run_state = ScriptRunState::ExecuteFailed;
                    break;
                }
                ifs.close();

                if(strcmp(total_result.c_str(),"OK") != 0){
                    LOGERROR << "OtaInteraction::OtaTaskProcess: Script execution TotalResult failed,result: " << total_result.c_str();
                    run_state = ScriptRunState::ExecuteFailed;
                    break;
                }
            }

            run_state = ScriptRunState::ExecuteFinish;
            LOGINFO << "OtaInteraction::OtaTaskProcess: script execute over.";
            break;
        }
        else if(process_result == kCONDNOTALLOW){
            run_state = ScriptRunState::ConditionNotAllow;
            LOGERROR << "OtaInteraction::OtaTaskProcess: script condition check failed.";
            break;
        }
        else if(process_result == KScriptRunning){
            run_state = ScriptRunState::ScriptRunning;
            LOGINFO << "OtaInteraction::OtaTaskProcess: script is running.";
        }
        else{
            run_state = ScriptRunState::UnknowError;
            LOGERROR << "OtaInteraction::OtaTaskProcess: script running unknow error.";
            break;
        }
    }
    script_process_instance->Stop();
    /*-----------------------------------------------调用脚本解析模块执行任务--------------------------------------------------*/

    //将结果文件复制到/ota/VTTP/下
    file_opreation_instance.copy_dir(DIAG_RESULT_FILE_FLODER,OTA_CONFIG_FILES_FLODER);

    //打包结果文件
    diag_file_instance.CompressPackage("DiagResult");
    if(access(DIAG_RESULT_FILE_PATH,0) == -1){
        LOGERROR << "OtaInteraction::OtaTaskProcess: create diag result package failed!";
    }

    //将结果文件复制到大数据目录/data/DiagnosticScript/BackupResult/下
    diag_file_instance.ProcessFile2BigDataFolder();
    /* keivn 条件检查
    //申请智能配电下电
    condition_check->SetIEPowerState(0,false);
    condition_check->SetVTState(0x00);
    */
    //设置ota任务状态
    is_in_ota_task = false;
}

/*
void OtaInteraction::MaintainOtaTaskStatus()
{
    int count = 0;
    is_in_ota_task = true;

    Print_Location(LOG_ID, INFO, "%s", "maintain network 10 min from ota request.");
    while(MAINTAIN_NETWORK){
        if(global_stop_flag){
            break;
        }

        if(count >= int(OTA_TASK_MAX_TIME)){
            Print_Location(LOG_ID, INFO, "%s", "maintain network finish.");
            break;
        }

        std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(5));
        count += 5;
    }

    is_in_ota_task = false;
}
*/

bool OtaInteraction::IsScriptFileExist(std::string scriptFilePath)
{
    if(access(scriptFilePath.c_str(),0) == 0){
        LOGINFO << "OtaInteraction::IsScriptFileExist: script file: " << scriptFilePath.c_str() << " is exist.";
        return true;
    }
    else{
        LOGINFO << "OtaInteraction::IsScriptFileExist: script file: " << scriptFilePath.c_str() << " is not exist.";
        return false;
    }
}

} // namespace vdi
} // namespace asf
#endif
