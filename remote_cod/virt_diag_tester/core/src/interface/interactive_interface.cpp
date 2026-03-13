/*****************************************************************************
* @file     interact_interface.cpp
* @brief    Interact Interface
* @date     2025/4/24
*****************************************************************************/

#include "3rdparty/interactive_interface.h"
#include "common/log.h"
#include "global/global.h"

void InputBoxState::DoSomeThing(InteractiveService* is, ACKST* ack)
{
    if((int)is->buffer[0] == INPUTBOX)
    {
        //std::cout<<"______current state INPUTBOX______"<<std::endl;
        is->DataHandle(ack);
    }
    else if((int)is->buffer[0] == MESSAGEBOX)
    {
        is->ChangeState(new MessageBoxState());
        is->SetVistor(new MessageBoxVisitor());
        is->DoSomeThing(ack);
    }
    else if((int)is->buffer[0] == SELECTION)
    {
        is->ChangeState(new SelectionState());
        is->SetVistor(new SelectionVisitor());
        is->DoSomeThing(ack);
    }
    else if((int)is->buffer[0] == INFORMATION)
    {
        is->ChangeState(new InformationState());
        is->SetVistor(new InformationVisitor());
        is->DoSomeThing(ack);
    }
}

void MessageBoxState::DoSomeThing(InteractiveService* is,ACKST* ack)
{
    if((int)is->buffer[0] == MESSAGEBOX)
    {
        //std::cout<<"______current state MESSAGEBOX______"<<std::endl;
        is->DataHandle(ack);
    }
    else if((int)is->buffer[0] == INPUTBOX)
    {
        is->ChangeState(new InputBoxState());
        is->SetVistor(new InputBoxVisitor());
        is->DoSomeThing(ack);
    }
    else if((int)is->buffer[0] == SELECTION)
    {
        is->ChangeState(new SelectionState());
        is->SetVistor(new SelectionVisitor());
        is->DoSomeThing(ack);
    }
    else if((int)is->buffer[0] == INFORMATION)
    {
        is->ChangeState(new InformationState());
        is->SetVistor(new InformationVisitor());
        is->DoSomeThing(ack);
    }
}

void SelectionState::DoSomeThing(InteractiveService* is,ACKST* ack)
{
    if((int)is->buffer[0] == SELECTION)
    {
        //std::cout<<"______current state SELECTION______"<<std::endl;
        is->DataHandle(ack);
    }
    else if((int)is->buffer[0] == MESSAGEBOX)
    {
        is->ChangeState(new MessageBoxState());
        is->SetVistor(new MessageBoxVisitor());
        is->DoSomeThing(ack);
    }
    else if((int)is->buffer[0] == INPUTBOX)
    {
        is->ChangeState(new InputBoxState());
        is->SetVistor(new InputBoxVisitor());
        is->DoSomeThing(ack);
    }
    else if((int)is->buffer[0] == INFORMATION)
    {
        is->ChangeState(new InformationState());
        is->SetVistor(new InformationVisitor());
        is->DoSomeThing(ack);
    }
}

void InformationState::DoSomeThing(InteractiveService* is,ACKST* ack)
{
    if((int)is->buffer[0] == INFORMATION)
    {
        //std::cout<<"______current state INFORMATION______"<<std::endl;
        is->DataHandle(ack);
    }
    else if((int)is->buffer[0] == MESSAGEBOX)
    {
        is->ChangeState(new MessageBoxState());
        is->SetVistor(new MessageBoxVisitor());
        is->DoSomeThing(ack);
    }
    else if((int)is->buffer[0] == SELECTION)
    {
        is->ChangeState(new SelectionState());
        is->SetVistor(new SelectionVisitor());
        is->DoSomeThing(ack);
    }
    else if((int)is->buffer[0] == INPUTBOX)
    {
        is->ChangeState(new InputBoxState());
        is->SetVistor(new InputBoxVisitor());
        is->DoSomeThing(ack);
    }
}

void InputBoxVisitor::DataHandle(InteractiveService* is,ACKST* ack)
{
    //std::cout<<"______parasing InputBox message______"<<std::endl;
    //std::cout<<is->buffer<<std::endl;
    int pos = 0;
    while((is->buffer[pos] != '\0') && (pos < BUFFSIZE))
    {
        rData rd;

        rd.msgtype = (Interact)is->buffer[pos];
        pos += 1;

        char tobuf[4];
        memcpy(tobuf,(is->buffer)+pos,4);
        rd.tollent = *(unsigned int*)tobuf;
        pos += 4;

        int titlelen = (int)is->buffer[pos];
        pos += 1;
        for(int i = 0;i<titlelen;i++,pos++)
        {
            rd.title += is->buffer[pos];
        }

        rd.NumberOfInputs = (int)is->buffer[pos];
        pos += 1;
        for(int i = 0;i<rd.NumberOfInputs;i++)
        {
            int meslen = (int)is->buffer[pos];
            pos += 1;
            std::string mesage;
            for(int i=0;i<meslen;i++,pos++)
            {
                mesage += is->buffer[pos];
            }
            rd.message.push_back(mesage);
        }

        rd.mesboxbutton = (int)is->buffer[pos];
        pos += 1;

//        int varlen = (int)is->buffer[pos];
//        pos += 1;
//        for(int i=0;i < varlen;i++,pos++)
//        {
//            rd.VarName += is->buffer[pos];
//        }

        rd.VarType =(int)is->buffer[pos];
        pos += 1;

        int reglen = (int)is->buffer[pos];
        pos += 1;
        for(int i=0;i < reglen;i++,pos++)
        {
            rd.regExpression += is->buffer[pos];
        }

        int usrlen = (int)is->buffer[pos];
        pos += 1;
        for(int i=0;i < usrlen;i++,pos++)
        {
            rd.usrdata += is->buffer[pos];
        }
        is->callfunc(&rd,ack);
    }
}

void MessageBoxVisitor::DataHandle(InteractiveService* is,ACKST* ack)
{
    //std::cout<<"______parasing MessageBox message______"<<std::endl;
    //std::cout<<is->buffer<<std::endl;
    int pos = 0;
    while((is->buffer[pos] != '\0') && (pos < BUFFSIZE))
    {
        rData rd;

        rd.msgtype = (Interact)is->buffer[pos];
        pos += 1;

        char tobuf[4];
        memcpy(tobuf,(is->buffer)+pos,4);
        rd.tollent = *(unsigned int*)tobuf;
        pos += 4;

        int titlelen = (int)is->buffer[pos];
        pos += 1;
        for(int i=0;i<titlelen;i++,pos++)
        {
            rd.title += is->buffer[pos];
        }

        int meslen = (int)is->buffer[pos];
        pos += 1;
        std::string mes;
        for(int i=0;i<meslen;i++,pos++)
        {
            mes += is->buffer[pos];
        }
        rd.message.push_back(mes);

        rd.mesboxbutton = (int)is->buffer[pos];
        pos += 1;

        rd.MeesageBoxIcon = (int)is->buffer[pos];
        pos += 1;

        int usrlen = (int)is->buffer[pos];
        pos += 1;
        for(int i=0;i < usrlen;i++,pos++)
        {
            rd.usrdata += is->buffer[pos];
        }
        is->callfunc(&rd,ack);
     }
}

void SelectionVisitor::DataHandle(InteractiveService* is,ACKST* ack)
{
    //std::cout<<"______parasing Selection message______"<<std::endl;
    //std::cout<<is->buffer<<std::endl;
    int pos = 0;
    while((is->buffer[pos] != '\0') && (pos < BUFFSIZE))
    {
        rData rd;

        rd.msgtype = (Interact)is->buffer[pos];
        pos += 1;

        char tobuf[4];
        memcpy(tobuf,(is->buffer)+pos,4);
        rd.tollent = *(unsigned int*)tobuf;
        pos += 4;

        int titlelen = (int)is->buffer[pos];
        pos += 1;
        for(int i=0;i<titlelen;i++,pos++)
        {
            rd.title += is->buffer[pos];
        }

        int meslen = (int)is->buffer[pos];
        pos += 1;
        std::string mes;
        for(int i=0;i<meslen;i++,pos++)
        {
            mes += is->buffer[pos];
        }
        rd.message.push_back(mes);

        rd.mesboxbutton = (int)is->buffer[pos];
        pos += 1;

        rd.NumberOfInputs = (int)is->buffer[pos];
        pos += 1;
        for(int i = 0;i<rd.NumberOfInputs;i++)
        {
            int itmelen = (int)is->buffer[pos];
            pos += 1;
            std::string item;
            for(int i=0;i<itmelen;i++,pos++)
            {
                item += is->buffer[pos];
            }
            rd.SelectionItems.push_back(item);
        }

        int varlen = (int)is->buffer[pos];
        pos += 1;
        for(int i=0;i<varlen;i++,pos++)
        {
            rd.VarName += is->buffer[pos];
        }

        int usrlen = (int)is->buffer[pos];
        pos += 1;
        for(int i=0;i < usrlen;i++,pos++)
        {
            rd.usrdata += is->buffer[pos];
        }
        is->callfunc(&rd,ack);
    }
}

void InformationVisitor::DataHandle(InteractiveService* is,ACKST* ack)
{
    //std::cout<<"______parasing Information message______"<<std::endl;
    //std::cout<<is->buffer<<std::endl;
    int pos = 0;
    while((is->buffer[pos] != '\0') && (pos < BUFFSIZE))
    {
        rData rd;

        rd.msgtype = (Interact)is->buffer[pos];
        pos += 1;

        char tobuf[4];
        memcpy(tobuf,(is->buffer)+pos,4);
        rd.tollent = *(unsigned int*)tobuf;
        pos += 4;

        int titlelen = (int)is->buffer[pos];
        pos += 1;
        for(int i = 0;i<titlelen;i++,pos++)
        {
            rd.title += is->buffer[pos];
        }
        //std::cout<<rd.title<<std::endl;
        int meslen = (int)is->buffer[pos];
        pos += 1;
        std::string mes;
        for(int i=0;i<meslen;i++,pos++)
        {
            mes += is->buffer[pos];
        }
        //std::cout<<mes<<std::endl;
        rd.message.push_back(mes);

        int usrlen = (int)is->buffer[pos];
        pos += 1;
        for(int i=0;i < usrlen;i++,pos++)
        {
            rd.usrdata += is->buffer[pos];
        }
        //std::cout<<rd.usrdata<<std::endl;
        is->callfunc(&rd,ack);
    }
}

void InteractiveService::DoNothingForTransparent(ACKST* ack)
{
    tData td;
    unsigned char type[4];
    memcpy(type,buffer,4);
    td.msgtype = (Interact)*(Interact*)type;
    unsigned char time[4];
    memcpy(time,buffer+4,4);
    td.timeout = *(float*)time;
    td.message = (buffer+8);
    trans(&td,ack);
}
void InteractiveService::DataHandle(ACKST* ack)
{
    current_vistor->DataHandle(this,ack);
}

void InteractiveService::SetVistor(Visitor* vs)
{
    delete current_vistor;
    current_vistor = vs;
}
void InteractiveService::DoSomeThing(ACKST* ack)
{
    current_state->DoSomeThing(this,ack);
}

bool InteractiveService::BuildService(struct sockaddr_un& serv)
{
    servicefd = socket(AF_UNIX,SOCK_STREAM,0);
    if(servicefd == -1)
    {
        //std::cout<<"______socket create fail______"<<std::endl;
        return false;
    }
    unlink(serv.sun_path);
    //cout << serv.sun_path << endl;
    if(bind(servicefd,(struct sockaddr*)&serv,sizeof(serv)) == -1 )
    {
        //std::cout<<"______socket bind addr fail______"<<std::endl;
        return false;
    }
    if(listen(servicefd,5) == -1)
    {
        //std::cout<<"______socket listen fail______"<<std::endl;
        return false;
    }
    flag = true;
    return true;
}

void InteractiveService::ChangeState(State* st)
{
    delete current_state;
    current_state = st;
}

void InteractiveService::Close()
{
    flag = false;
    pthread_cancel(servicethread.native_handle());
    if(servicethread.joinable())
    {
        servicethread.join();
    }
    memset(buffer,'\0',BUFFSIZE);
    //cout<<"______rec thread stop______"<<endl;

    servicefd = -1;
    callfunc = nullptr;
    trans = nullptr;
    memset(buffer,0,BUFFSIZE);
    flag = -1;
    delete current_state;
    current_state = nullptr;
    delete current_vistor;
    current_vistor = nullptr;
    istrans = true;
}

int InteractiveService::SendMSG(ACKST* msg)
{
    int err = -1;
    try
    {
        if((msg->acktype != INFORMATION )&& (msg->acktype != OTHER))
        {
            int tollen = 20;
            for(auto item:msg->info)
            {
                tollen += static_cast<int>(item.size());
                tollen += 4;
            }
            char* buf = (char*)malloc(static_cast<size_t>(tollen+1));
            memset(buf,0,static_cast<size_t>(tollen+1));
            int pos = 0;
//            *(buf+pos) = (char)tollen;
//            pos+=1;
            memcpy(buf+pos,&tollen,sizeof(tollen));
            pos += sizeof(tollen);

//            *(buf+pos) = (char)msg->acktype;
//            pos+=1;
            memcpy(buf+pos,&msg->acktype,sizeof(msg->acktype));
            pos += sizeof(msg->acktype);

//            *(buf+pos) = (char)msg->result;
//            pos+=1;
            memcpy(buf+pos,&msg->result,sizeof(msg->result));
            pos += sizeof(msg->result);

//            *(buf+pos) = (char)msg->SelectedIndex;
//            pos+=1;
            memcpy(buf+pos,&msg->SelectedIndex,sizeof(msg->SelectedIndex));
            pos += sizeof(msg->SelectedIndex);

//            *(buf + pos) = (char)msg->info.size();
//            pos += 1;
            int num = static_cast<int>(msg->info.size());
            memcpy(buf+pos,&num,sizeof(num));
            pos += sizeof(num);

            for(auto item:msg->info)
            {
//                *(buf+pos) = (char)item.size();
                int size = static_cast<int>(item.size());
                memcpy(buf+pos,&size,sizeof(size));
                pos+=sizeof(size);
                memcpy(buf+pos,item.c_str(),item.size());
                pos +=  static_cast<int>(item.size());
            }
            err =  static_cast<int>(write(clientfd,buf,static_cast<size_t>(tollen)));
            //std::cout<<"______send msg ______:"<<buf<<std::endl;
            free(buf);
            buf = nullptr;
        }
        else
        {
            //std::cout<<"______do not need to send msg ______"<<std::endl;
        }
        return err;
    }
    catch(...)
    {
        //std::cout<<"______send msg fail______"<<std::endl;
        return err;
    }

}

bool InteractiveService::SetCallback(DEALMSG func)
{
    if(!func)
    {
        //std::cout<<"______setCallback func is nullptr______"<<std::endl;
        return false;
    }
    callfunc = func;
    istrans = false;
    servicethread = std::thread(&InteractiveService::ReceiveMSG,this);
    return true;
}

bool InteractiveService::SetCallback(TRANSPARENT func)
{
    //func = std::bind(&DiagTaskInteract::ScriptInteractCallback,diag_task_instance,std::placeholders::_1,std::placeholders::_2);
    if(!func)
    {
        //std::cout<<"______setCallback func is nullptr______"<<std::endl;
        return false;
    }
    trans = func;
    istrans = true;
    servicethread = std::thread(&InteractiveService::ReceiveMSG,this);
    return true;
}

bool InteractiveService::SetCallback()
{
    TRANSPARENT func = std::bind(&DiagTaskInteract::ScriptInteractCallback,diag_task_instance,std::placeholders::_1,std::placeholders::_2);
    if(!func)
    {
        //std::cout<<"______setCallback func is nullptr______"<<std::endl;
        return false;
    }
    trans = func;
    istrans = true;
    servicethread = std::thread(&InteractiveService::ReceiveMSG,this);
    return true;
}

void InteractiveService::ReceiveMSG()
{
    if(servicefd == -1)
    {
        //std::cout<<"______servicefd is error______"<<std::endl;
        return;
    }
    if(istrans)
    {
        if(trans == nullptr)
        {
            //std::cout<<"______thread callback function is nullptr______"<<std::endl;
            return;
        }
    }
    else
    {
        if(callfunc == nullptr)
        {
            //std::cout<<"______thread callback function is nullptr______"<<std::endl;
            return;
        }
    }
    while(flag)
    {
        clientfd = accept(servicefd,nullptr,0);
        if(clientfd != -1)
        {
            is_read_interaction_message_finish = false;
            while((read(clientfd,buffer,BUFFSIZE)>0) && flag)
            {
                if(interact_state.is_finish){
                    Clearbuffer();
                    LOGINFO << "the task has been executed,clear interaction buffer.";
                    continue;
                }

                ACKST ack;
                if(trans)
                {
                    DoNothingForTransparent(&ack);
                }
                else
                {
                    DoSomeThing(&ack);
                }
                SendMSG(&ack);
                Clearbuffer();
            }
            is_read_interaction_message_finish = true;
        }
    }
    close(servicefd);
}

void DiagTaskInteract::ScriptInteractCallback(tData* td,ACKST* ack)
{
    Json::Value root;
    Json::Value custom_parameter;
    Json::Value interact_parameter;
    Json::Value script_value;
    Json::Reader reader;
    std::string request_string = "";
    std::string task_id;
    time_t timestamp;
    std::string interact_type;
    struct timespec timer;
    int task_timeout = 0;

    if ((td == nullptr) || (ack == nullptr)) {
        LOGERROR << "DiagTaskInteract::ScriptInteractCallback: error,input value is nullptr.";
        return;
    }
    else{
        ++interact_state.sequence;
    }

    if(interact_state.is_finish) {
        LOGERROR << "DiagTaskInteract::ScriptInteractCallback: the task has been executed or no task has been executed,do not need to interact.";
        return;
    }

    task_timeout = static_cast<int>(td->timeout);
    //cout << "[DiagTaskInteract::ScriptInteractCallback]get an interactive task:"
    //     << " message type:" << td->msgtype 
    //     << " timeout:" << td->timeout
    //     << " message:" << td->message << endl;
    //cout << "[DiagTaskInteract::ScriptInteractCallback]interactive sequence:" << state.sequence << endl;
    LOGINFO << "DiagTaskInteract::ScriptInteractCallback: message type: " << td->msgtype << " ,timeout: " << (int)task_timeout
            << " ,message: " << td->message.c_str() << " ,interactive sequence: " << interact_state.sequence;

    //组合交互请求json,发送给云端，messageType = 105
    timestamp = DiagTaskInteract::GetTimeStamp();
    root["messageType"] = 105;
    root["protocolVersion"] = interact_state.protocol_version;
    root["timestamp"] = timestamp;
    root["taskId"] = interact_state.task_id;

    custom_parameter["sequence"] = interact_state.sequence;
    custom_parameter["runType"] = interact_state.run_type;
    custom_parameter["taskTimeout"] = task_timeout;

    if(td->msgtype == INFORMATION){
        custom_parameter["needResponse"] = 0;
    }
    else{
        custom_parameter["needResponse"] = 1;
    }
    if(reader.parse(td->message,script_value)){
        interact_type = script_value["interactType"].asString();
        interact_parameter = script_value["interactParameter"];
        custom_parameter["interactParameter"] = interact_parameter;
    }
    else{
        custom_parameter["interactParameter"];
        LOGERROR << "DiagTaskInteract::ScriptInteractCallback: parse script interact value failed,can not get interact parameter.";
    }
    root["customParameter"] = custom_parameter;

    request_string = root.toStyledString();
    LOGINFO << "DiagTaskInteract::ScriptInteractCallback: create interact request Json string: " << request_string.c_str();

    if(is_in_local_task){
        LOGINFO << "DiagTaskInteract::ScriptInteractCallback: interaction type: obd.";
        // kevin 本地诊断
        // this->dm_local_skeleton->localDiagExcuteResult.Update(request_string);
    }
    else{
        LOGINFO << "DiagTaskInteract::ScriptInteractCallback: interaction type: cloud.";
        DiagTaskInteract::SendDiagData(request_string);
    }
    interact_state.is_response = false;

    //开启计时，等待云端回复
    clock_gettime(CLOCK_REALTIME,&timer);
    clock_t start_time = timer.tv_sec;
    clock_t current_time = timer.tv_sec;

    //每0.5秒查看是否已收到云端回复
    while(1){
        if(global_stop_flag){
            return;
        }

        //判断是否超时,超时传递默认参数给脚本解析模块
        if(task_timeout <= 0){
            LOGINFO << "DiagTaskInteract::ScriptInteractCallback: interact type: information,do not need to get response.";
            break;
        }

        //任务已结束，放弃等待交互响应
        if(interact_state.is_finish){
            LOGINFO << "DiagTaskInteract::ScriptInteractCallback: the task has been executed,do not need to get response.";
            break;
        }

        clock_gettime(CLOCK_REALTIME,&timer);
        current_time = timer.tv_sec;
        if((current_time - start_time) > static_cast<clock_t>(task_timeout)){
            if(strcmp(interact_type.c_str(),"1") == 0){
                ack->acktype = MESSAGEBOX;
            }
            else if(strcmp(interact_type.c_str(),"2") == 0){
                ack->acktype = INPUTBOX;
            }
            else if(strcmp(interact_type.c_str(),"3") == 0){
                ack->acktype = SELECTION;
            }
            else if(strcmp(interact_type.c_str(),"4") == 0){
                ack->acktype = INFORMATION;
            }
            else{
                ack->acktype = OTHER;
            }
            LOGERROR << "DiagTaskInteract::ScriptInteractCallback: wait cloude response value timeout,can not get interact response parameter.";
            break;
        }

        //将云端返回的参数传给脚本解析模块
        if(interact_state.is_response == true){
            // cout << "[DiagTaskInteract::ScriptInteractCallback]get cloude interact response,parameter:" 
            //      << " interactType:" << interact_state.response_parameter.interact_type 
            //      << " result:" << interact_state.response_parameter.result
            //      << " selected_index:" << interact_state.response_parameter.selected_index
            //      << " inputValues:";
            //for(auto iter : interact_state.response_parameter.input_value){
                // cout << iter << " ";
            // }
            // cout << endl;

            if(strcmp(interact_state.response_parameter.interact_type.c_str(),"1") == 0){
                ack->acktype = MESSAGEBOX;
                ack->result = interact_state.response_parameter.result;
            }
            else if(strcmp(interact_state.response_parameter.interact_type.c_str(),"2") == 0){
                ack->acktype = INPUTBOX;
                ack->result = interact_state.response_parameter.result;
                for(auto iter : interact_state.response_parameter.input_value){
                    ack->info.push_back(iter);
                }
            }
            else if(strcmp(interact_state.response_parameter.interact_type.c_str(),"3") == 0){
                ack->acktype = SELECTION;
                ack->result = interact_state.response_parameter.result;
                ack->SelectedIndex = interact_state.response_parameter.selected_index;
            }
            else if(strcmp(interact_state.response_parameter.interact_type.c_str(),"4") == 0){
                ack->acktype = INFORMATION;
            }
            else{
                ack->acktype = OTHER;
            }
            break;
        }

        std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::milliseconds(500));
    }

    //重置交互状态
    LOGINFO << "DiagTaskInteract::ScriptInteractCallback: send interact response parameter finish.";
    interact_state.is_response = false;
    interact_state.response_parameter.result = -1;
    interact_state.response_parameter.interact_type = "";
    interact_state.response_parameter.selected_index = -1;
    interact_state.response_parameter.input_value.clear();
    return;
}

void DiagTaskInteract::SendDiagData(const std::string diagData)
{
#if 0
    if(!(vc_proxy_instance->Valid()) || vc_proxy_instance->VCPProxy() == nullptr) {
        LOGERROR << "DiagTaskInteract::ScriptInteractCallback: VC proxy is null,can not send diag data!";
    }
    else{
        auto method_future = vc_proxy_instance->VCPProxy()->ReportDiagnosisData(diagData);
        std::chrono::time_point<std::chrono::system_clock> deadline = 
            std::chrono::system_clock::now() + std::chrono::milliseconds(5000);
#ifdef NEUSAR_AP_R2111
        if(method_future.wait_until(deadline) != ara::core::future_status::ready){
            LOGERROR << "DiagTaskInteract::ScriptInteractCallback: VC proxy request timeout, quit!";
        }
#else
        if(method_future.wait_until(deadline) != decltype(method_future)::Status::kReady){
            LOGERROR << "DiagTaskInteract::ScriptInteractCallback: VC proxy request timeout, quit!";
        }
#endif
        else{
            auto result = method_future.GetResult();
            if(result.HasValue()) {
                auto value = result.Value();
                LOGINFO << "DiagTaskInteract::ScriptInteractCallback: cloud response: " << value.ReportDiagnosisResult.c_str();
            }
            else {
                LOGERROR << "DiagTaskInteract::ScriptInteractCallback: cloud unresponsive!";
            }
        }
    }
#endif
}