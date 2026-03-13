/*****************************************************************************
* @file     interactive_interface.h
* @brief    
* @author   kevin
* @date     2025/4/24
*****************************************************************************/

#ifndef _Interactive_interface_
#define _Interactive_interface_

#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <sys/un.h>
#include <vector>
#include <functional>

#include "diag/local_diag_client.h"
#include "check/condition_check.h"
#include "common/json.h"

#define BUFFSIZE 15000
#define PARAS true
class rData;
class tData;
//class DiagTaskInteract;
struct ACKST;
typedef void(*DEALMSG)(rData* st,ACKST* ack);
typedef std::function<void(tData*,ACKST*)> TRANSPARENT;
//typedef void(*TRANSPARENT)(tData* td,ACKST* ack);

enum Interact
{
    MESSAGEBOX = 0X01,
    INPUTBOX = 0x02,
    SELECTION = 0X03,
    INFORMATION = 0X04,
    OTHER = 0X05
};

struct ACKST
{
    Interact acktype = OTHER;
    int result = -1;
    int SelectedIndex = -1;
    std::vector<std::string> info;
};

class tData
{
public:
    Interact msgtype{OTHER};
    float timeout{-1};
    std::string message;
};
class rData
{
public:
    unsigned int tollent{0};
    Interact msgtype{OTHER};
    std::string title;
    std::vector<std::string> message;
    int mesboxbutton{-1};
    std::string VarName;
    int VarType{-1};
    std::string regExpression;
    int MeesageBoxIcon{-1};
    int NumberOfInputs{-1};
    std::vector<std::string> SelectionItems;
    std::string usrdata;
};

class InteractiveService;
class Visitor;
class State
{
public:
    virtual void DoSomeThing(InteractiveService* is,ACKST* ack) = 0;
    virtual ~State() noexcept {};
};
class InputBoxState : public State
{
public:
     void DoSomeThing(InteractiveService* is,ACKST* ack) override;
};
class MessageBoxState : public State
{
public:
    void DoSomeThing(InteractiveService* is,ACKST* ack) override;
};
class SelectionState : public State
{
public:
    void DoSomeThing(InteractiveService* is,ACKST* ack) override;
};
class InformationState : public State
{
public:
    void DoSomeThing(InteractiveService* is,ACKST* ack) override;
};

class Visitor
{
public:
    virtual void DataHandle(InteractiveService* is,ACKST* ack) = 0;
    virtual ~Visitor() noexcept {};

};
class InputBoxVisitor : public Visitor
{
public:
     void DataHandle(InteractiveService* is,ACKST* ack) override;
};
class MessageBoxVisitor : public Visitor
{
public:
    void DataHandle(InteractiveService* is,ACKST* ack) override;

};
class SelectionVisitor : public Visitor
{
public:
    void DataHandle(InteractiveService* is,ACKST* ack) override;

};
class InformationVisitor : public Visitor
{
public:
    void DataHandle(InteractiveService* is,ACKST* ack) override;

};

class DiagTaskInteract
{
public:
    DiagTaskInteract(){}
    ~DiagTaskInteract(){}

    void ScriptInteractCallback(tData* td,ACKST* ack);
    void SendDiagData(const std::string diagData);

    time_t GetTimeStamp()
    {
        std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp =
            std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());//获取当前时间点
        long timestamp =  tp.time_since_epoch().count(); //计算距离1970-1-1,00:00的时间长度
        return timestamp;
    }
#ifdef LOCAL_DIAG_CLIENT
    // kevin 本地诊断
    std::shared_ptr<LocalDiagClient> dm_local_skeleton = LocalDiagClient::GetInstance();
#endif

};

class InteractiveService
{
public:
    friend class Visitor;
    friend class State;
    InteractiveService()
    {
        memset(buffer,'\0',BUFFSIZE);
    }
    ~InteractiveService()
    {
        servicefd = -1;
        callfunc = nullptr;
        trans = nullptr;
        memset(buffer,0,BUFFSIZE);
        flag = -1;
        if(servicethread.joinable())
        {
            servicethread.join();
        }
        delete current_state;
        current_state = nullptr;
        delete current_vistor;
        current_vistor = nullptr;
        istrans = true;
    }
public:
    bool BuildService(struct sockaddr_un& serv);
    void Close();
    bool SetCallback(DEALMSG func);
    bool SetCallback(TRANSPARENT func);
    bool SetCallback();
    void ChangeState(State* st);
    void SetVistor(Visitor* vs);
    void DataHandle(ACKST* ack);
    void DoSomeThing(ACKST* ack);
    void DoNothingForTransparent(ACKST* ack);
    void Init()
    {
        current_state = new InputBoxState();
        current_vistor = new InputBoxVisitor();
    }
private:
    void ReceiveMSG();
    int SendMSG(ACKST* msg);
    void Clearbuffer()
    {
        memset(buffer,0,BUFFSIZE);
    }

private:
    int servicefd{-1};
    std::thread servicethread;
    bool flag{false};
    int clientfd{-1};
    State* current_state;
    Visitor* current_vistor;
    bool istrans{true};
public:
    char buffer[BUFFSIZE];
    DEALMSG callfunc{nullptr};
    TRANSPARENT trans{nullptr};
    DiagTaskInteract diag_task_instance;
};

#endif