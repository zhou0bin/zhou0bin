#ifndef __DIAGSCRIPTPARSING_H
#define __DIAGSCRIPTPARSING_H

#ifdef DIAGSCRIPTPARSING_TEST

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdarg.h>
#include <string.h>
#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <map>
#include <vector>
#include <thread>
#include <iostream>
//error info
enum InterfaceType
{
    kCloud = 0x01,
    kLocal = 0x02,
    kStop  = 0x03,
    kNone  = 0x04
};
enum ErrorType
{
   kTooMany     = 0X01,//request queue is full
   kExcutedOver  = 0X02,//script had executed
   kExcutedFaile     = 0X03,//script had not executed
   kNoscript = 0X04,//find no script
   kNoZIP    = 0X05,//the scriptzip does not exist
   kCONDNOTALLOW = 0X06,//conditions do not allow
   kDECFAILE = 0X07,//reserved fields: "decompression failed"
   KStopSuccess = 0x08,//stop script
   KFindNoScriptTh = 0x09,//find no script thread
   KScriptRunning = 0x0A,//script is running
   KUnKnown  = 0x0B//unknown err
};
struct ReceiveData
{
    InterfaceType itype;
    ErrorType err_code;
    std::string guid;
};
struct SendData
{
    InterfaceType itype;
    std::string guid;
};

typedef void(*CALLFUNC)(ReceiveData&);
enum ECALLTYPE
{
    NONE = 1,
    CLASS = 2,
    FUNC = 3
};
/*
 @Name:DiagScriptPorcess
 @Function:Call the corresponding diagnostic script and return the execution result
 @param:
    sData:Data to be sent
    rData:Received data
    bFlag:Flag whether data is received,It will be "true" if data is received,else is "false"
*/
class ScriptProcess
{
public:
    class Callback
    {
    public:
        virtual void MessageCallback(ReceiveData& rd) = 0;
    };
public:
    int Start(const char* SocketPip);
    int Start(const char* SocketPip,std::weak_ptr<Callback> callptr);
    int Start(const char* SocketPip,CALLFUNC call);
    void Stop();
    int Send(struct SendData* send_data);
    // struct ReceiveData ReceiveDatas(const char* guid);
    ErrorType ReceiveDatas(const char* guid);
    static ScriptProcess* GetInstance();
public:
    ScriptProcess& operator = (const ScriptProcess&) = delete;

private:
    ScriptProcess() = default;
    ~ScriptProcess() = default;
    void ReceiveTask();
    int Connect(const char* SocketPip);
    void CachMessage(ReceiveData& rd);
private:
    bool local_receive_status;
    // std::vector<struct ReceiveData> cache_local_receive_data;
    std::map<std::string,ErrorType> cache_local_receive_data;
    bool local_is_connect_{false};
    int local_socket_;
    static std::mutex mutex_;
    static ScriptProcess* instance_;
    std::mutex local_mutex_;
    std::thread local_receive_thread;
    ECALLTYPE _cty{NONE};
    std::weak_ptr<Callback> m_callback;
    CALLFUNC m_fcall;
};
#endif

#endif
