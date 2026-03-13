/*****************************************************************************
* @file     diag_script_parsing.h
* @brief    diagnostic script parsing
* @author   kevin
* @date     2025/4/24
*****************************************************************************/

#ifndef __DIAGSCRIPTPARSING_H_
#define __DIAGSCRIPTPARSING_H_

#include <arpa/inet.h>
#include <condition_variable>
#include <sys/socket.h>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <string.h>
#include <sys/un.h>
#include <stdarg.h>
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
#include <functional>

#include "common/log.h"

//error info
enum InterfaceType
{
    kCloud = 0x01,
    kLocal = 0x02,
    kStop  = 0x03,
    kNone  = 0x04
};

enum ScriptRunState
{
    kScriptStart = 0x0E,
    KScriptStop = 0x0F
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
   KUnKnown  = 0x0B,//unknown err
   kStoping = 0x0C,
//    kIncomplete = 0x0D,

   KScriptWaiting = 0x10//script is waiting to run
};

struct ReceiveData
{
    char length = 0;
    // InterfaceType itype = kNone;
    ErrorType err_code = KUnKnown;//diag type
    // char *guid = nullptr;//message returned
    char *script_path = nullptr;
    
    ReceiveData() = default;

    inline bool operator == (const ReceiveData& rd)
    {
        if(strcmp(script_path, rd.script_path) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    inline bool operator == (const char* compare_script_path)
    {
        if(strcmp(script_path, compare_script_path) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    } 
    ReceiveData(ReceiveData const & rd) = delete;
    // {
    //     length = rd.length;
    //     itype = rd.itype;
    //     err_code = rd.err_code;
    //     guid = (char*)malloc(strlen(rd.guid)+1);
    //     memset(guid,'\0',strlen(rd.guid)+1);
    //     strcpy(guid,rd.guid);
    // }
    ReceiveData& operator = (ReceiveData const & rd) = delete;
    // {
    //     length = rd.length;
    //     itype = rd.itype;
    //     err_code = rd.err_code;
    //     free(guid);
    //     guid = (char*)malloc(strlen(rd.guid)+1);
    //     memset(guid,'\0',strlen(rd.guid)+1);
    //     strcpy(guid,rd.guid);
    //     return *this;
    // }
    ReceiveData(ReceiveData&& rd) = delete;
    // {
    //     length = rd.length;
    //     itype = rd.itype;
    //     err_code = rd.err_code;
    //     guid = rd.guid;
    //     rd.guid = nullptr;
    // }
};

struct SendData
{
    char length = 0;
    // InterfaceType itype;
    // char *guid = nullptr;//diag type
    ScriptRunState script_state;
    char *script_path = nullptr;
};

using CALLFUNC = std::function<void(ReceiveData const & rd)>;

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
#if 0
    int Start(const char* SocketPip,std::weak_ptr<Callback> callptr);
#endif
    int Start(const char* SocketPip, CALLFUNC call);

    void Stop();
    int Send(struct SendData* send_data);
    // struct ReceiveData ReceiveDatas(const char* guid);
    ErrorType ReceiveDatas(const char* script_path);//const std::string &guid
    static ScriptProcess* GetInstance();
public:
    ScriptProcess& operator = (const ScriptProcess&) = delete;

private:
    ScriptProcess() = default;
    ~ScriptProcess() = default;
    void ReceiveTask();
    int Connect(const char* SocketPip);
    void CachMessage(ReceiveData const & rd);

private:
    bool local_receive_status;
    // std::vector<struct ReceiveData> cache_local_receive_data;
    std::map<std::string,ErrorType> cache_local_receive_data;
    bool local_is_connect_{false};
    int local_socket_{-1};
    static std::mutex mutex_;
    static ScriptProcess* instance_;
    std::mutex data_mutex_;
    std::thread local_receive_thread;
    ECALLTYPE _cty{NONE};
    std::weak_ptr<Callback> m_callback;
    CALLFUNC m_fcall;
    std::mutex send_mutex_;
};

#endif