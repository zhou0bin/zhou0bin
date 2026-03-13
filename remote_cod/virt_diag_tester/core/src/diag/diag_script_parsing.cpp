/*****************************************************************************
* @file     diag_script_parsing.cpp
* @brief    Diagnostics Script Parsing
* @date     2025/4/24
*****************************************************************************/

#include "3rdparty/diag_script_parsing.h"

ScriptProcess* ScriptProcess::instance_ = nullptr;
std::mutex ScriptProcess::mutex_;

void ScriptProcess::ReceiveTask()
{
    //std::cout<<"****recth running****"<<std::endl;
    while(local_receive_status)
    {
        int ret=0;
        char lenth;
        // ret=read(local_socket_, &lenth, 1);
        // char type;
        // read(local_socket_, &type, 1);
        char err;
        // read(local_socket_, &err, 1);
        struct ReceiveData received_data;
        char buf[2] = {0};
        ret = read(local_socket_, &buf[0], sizeof(buf));
        if (ret == -1 && errno == EAGAIN) {
            LOGINFO << "ScriptProcess::ReceiveTask: no data header to read, continue";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        else if (ret < 0)
        {
            LOGERROR << "ScriptProcess::ReceiveTask: errno= " << errno << " ,something wrong! break";
            break;
        }
        else if (ret == 0)
        {
            LOGINFO << "ScriptProcess::ReceiveTask: normal exit socket= " << ret;
            break;
        }
        else if(ret < 2)
        {
            LOGERROR << "ScriptProcess::ReceiveTask: get littel data not enough= " << ret << " ,data is: " << buf[0];
            break;
        }
        
        lenth = buf[0];
        // type=buf[1];
        // err=buf[2];
        err = buf[1];

        if(lenth < 3)
        {
            LOGERROR << "ScriptProcess::ReceiveTask: length less than 3; lenth= " << lenth;
            break;
        }
        received_data.script_path = new char[lenth - static_cast<uint8_t>(1)];//给C语言字符串最后的'\0' 留1个字节，所以是-1而不是-2
        memset(received_data.script_path, '\0', lenth - static_cast<uint8_t>(1));

        ret = read(local_socket_, received_data.script_path,  lenth - static_cast<uint8_t>(2));
        if (ret < 0)
        {
            LOGERROR << "ScriptProcess::ReceiveTask: errno= " << errno << " ,something wrong! break.";
            break;
        }
        else if (ret == 0)
        {
            LOGINFO << "ScriptProcess::ReceiveTask: normal exit socket= " << ret;
            break;
        }
        else if ( ret< (lenth -  static_cast<uint8_t>(2)) )
        {
            LOGERROR << "ScriptProcess::ReceiveTask: get littel data not enough, ret= " << ret << " ,need read= " << lenth -  static_cast<uint8_t>(2);
            break;
        }

        received_data.length = lenth;

        // if(type > 0x4 || type < 0x1) {
        //     LOGERROR << "ScriptProcess::ReceiveTask: type = " << static_cast<int>(type) << ", wrong, break!";
        //     break;
        // }
        // received_data.itype = (InterfaceType)type;

        if(err > 0x10 || err < 0x1) {
            LOGERROR << "ScriptProcess::ReceiveTask: err = " << static_cast<int>(err) << " , wrong, break!";
            break;
        }
        received_data.err_code = (ErrorType)err;
        #if 0
        if ((_cty == NONE )&& local_receive_status)
        {
            CachMessage(received_data);
            delete [] received_data.script_path;
            received_data.script_path = nullptr;
        }
        else if((_cty == CLASS )&& local_receive_status)
        {
            if(!m_callback.expired())
            {
                m_callback.lock()->MessageCallback(received_data);
                delete [] received_data.guid;
                received_data.guid = nullptr;
            }
        }
        #endif
        if((_cty == FUNC) && local_receive_status)
        {
            if(m_fcall)
            {
                LOGINFO << "ScriptProcess::ReceiveTask: receive diag result: script_path= " << received_data.script_path << ", process_result= " << static_cast<int>(received_data.err_code);
                CachMessage(received_data);
                m_fcall(received_data);
                delete [] received_data.script_path;
                received_data.script_path = nullptr;
            }
        }
        std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::milliseconds(50));
    }
    return;
}

void ScriptProcess::CachMessage(ReceiveData const & rd)
{
    std::unique_lock<std::mutex> lock(data_mutex_);
    //std::cout<<"****recth "<<"receive message:type="<<(int)rd.itype<<" errcode="<<(int)rd.err_code<<" guid="<<rd.guid<<"****"<<std::endl;
    cache_local_receive_data[rd.script_path] = rd.err_code;
    //std::cout<<"****recth cash size:"<<cache_local_receive_data.size()<<"****"<<std::endl;
    // std::vector<struct ReceiveData>::iterator iter = find(cache_local_receive_data.begin(),cache_local_receive_data.end(),rd);
    // auto iter = cache_local_receive_data.find(rd.guid);
    // if(iter == cache_local_receive_data.end() && local_receive_status)
    // {
    //     std::cout<<"****recth "<<"receive message:type="<<(int)rd.itype<<" errcode="<<(int)rd.err_code<<" guid="<<rd.guid<<"****"<<std::endl;
    //     cache_local_receive_data.push_back(rd);
    // }
    // else
    // {
    //     if(local_receive_status)
    //     {
    //         cache_local_receive_data.erase(iter);
    //         std::cout<<"****recth "<<"updata message:type="<<(int)rd.itype<<" errcode="<<(int)rd.err_code<<" guid="<<rd.guid<<"****"<<std::endl;
    //         cache_local_receive_data.emplace_back(rd);
    //     }
    // }
}

ScriptProcess* ScriptProcess::GetInstance()
{
    if(instance_ == nullptr)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        instance_ = new ScriptProcess();
    }
    return instance_;
}
/*
 * Establish a connection with the server
 * Return 0 on success, -1 on fail
 */
int ScriptProcess::Connect(const char* SocketPip)
{
    local_socket_ = socket(AF_LOCAL,SOCK_STREAM,0);
    struct sockaddr_un local_server_address;
    bzero(&local_server_address, sizeof(local_server_address));
    local_server_address.sun_family = AF_LOCAL;
    //strcpy(local_server_address.sun_path,SocketPip);
    strncpy(local_server_address.sun_path,SocketPip,sizeof(local_server_address.sun_path)-1);
    int opt = 1;
    setsockopt( local_socket_, SOL_SOCKET,SO_REUSEADDR,(const void *)&opt, sizeof(opt) );
    int ierr = connect(local_socket_,(struct sockaddr*)&local_server_address,sizeof(local_server_address));
    return ierr;
}

#if 0
int ScriptProcess::Start(const char* SocketPip)
{
    int ierr = -1;
    while(!local_is_connect_){
        ierr = Connect(SocketPip);
        LOGINFO << "ScriptProcess::Start: connect socket= " << ierr;
        if(ierr == 0)
        {
            local_is_connect_ = true;
            local_receive_status = true;
            local_receive_thread = std::thread(&ScriptProcess::ReceiveTask,this);
            break;
        }
        else
        {
            local_is_connect_ = false;
        }
       std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
    }
    
    return ierr;
}

int ScriptProcess::Start(const char* SocketPip,std::weak_ptr<Callback> callptr)
{
    int ierr = Connect(SocketPip);
    if((ierr == 0) && (!callptr.expired()))
    {
        local_is_connect_ = true;
        local_receive_status = true;
        m_callback = callptr;
        _cty = CLASS;
        local_receive_thread = std::thread(&ScriptProcess::ReceiveTask,this);
    }
    else
    {
        local_is_connect_ = false;
    }
    return ierr;
}
#endif

int ScriptProcess::Start(const char* SocketPip, CALLFUNC call)
{
    int ierr = -1;
    while(!local_is_connect_){
        ierr = Connect(SocketPip);
        
        if((ierr == 0) && call)
        {
            LOGINFO << "ScriptProcess::Start: connect success!";
            local_is_connect_ = true;
            local_receive_status = true;
            m_fcall = call;
            _cty = FUNC;
            local_receive_thread = std::thread(&ScriptProcess::ReceiveTask,this);
            break;
        }
        else
        {
            LOGERROR << "ScriptProcess::Start: connect failed, errno = " << errno;
            local_is_connect_ = false;
        }
        std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
    }
    return ierr;
}


/*
 * Stop the connection with the server
 */
void ScriptProcess::Stop()
{
    if(local_is_connect_)
    {
        local_is_connect_ = false;
        local_receive_status = false;
        pthread_cancel(local_receive_thread.native_handle());
        if(local_receive_thread.joinable())
        {
            local_receive_thread.join();
        }
        _cty = NONE;
        m_fcall = NULL;
        shutdown(local_socket_,SHUT_RDWR);
        close(local_socket_);
    }
    // std::vector<struct ReceiveData>().swap(cache_local_receive_data);
    cache_local_receive_data.clear();
    //std::cout<<"----"<<"Stop"<<"----"<<std::endl;
}

/*
 * Send data
 * Returns the number of bytes sent on success, returns -1 on fail
*/
// int ScriptProcess::Send(std::string script_path, uint8_t script_state)
int ScriptProcess::Send(struct SendData* send_data)
{
    //std::cout << "wn test guid :" << send_data->guid 
    //          << " type: " << send_data->itype 
    //          << "length: "  << send_data->length << std::endl;

    int error = -1;;
    send_data->length = static_cast<char>(strlen(send_data->script_path) + 1 + 1);
    char *data = new char[send_data->length + static_cast<uint8_t>(1)];//给C语言字符串最后的'\0' 留1个字节，所以加1
    memset(data, '\0', send_data->length + static_cast<uint8_t>(1));
    if(data == nullptr)
    {
        return -1;
    }
    *data = send_data->length;
    memcpy(data + 1, &send_data->script_state, 1);
    memcpy(data + 2, send_data->script_path, strlen(send_data->script_path));
    try
    {
        if(!local_is_connect_)
        {
            LOGERROR << "ScriptProcess::Send: not connect to server, send failed!";
            delete[] data; 
            data = nullptr;
            return -1;
        }
        std::unique_lock<std::mutex> socket_lock(send_mutex_);
        error = static_cast<int>(write(local_socket_, (const char *)data, strlen(data)));
        if(error < 0){
            LOGERROR << "ScriptProcess::Send: send failed, errno = " << errno;
        }
        socket_lock.unlock();
    }
    catch(...)
    {
        delete[] data; 
        data = nullptr;
        return error;
    }
    delete[] data;
    data = nullptr;
    return error;
}

/*
 * Receive data
 * Return the received data
*/
// struct ReceiveData ScriptProcess::ReceiveDatas(const char* guid)
// {
//     struct ReceiveData rdata;
//     rdata.err_code = KUnKnown;
//     rdata.guid = nullptr;
//     rdata.itype = kNone;
//     rdata.length = 0;
//     std::unique_lock<std::mutex> lock(local_mutex_);
//     auto iter = find(cache_local_receive_data.begin(),cache_local_receive_data.end(),guid);
//     if(iter != cache_local_receive_data.end())
//     {
//         std::cout<<"+++++"<<iter->guid<<" "<<iter->err_code<<"+++++"<<std::endl;
//         rdata = *iter;
//     }
//     lock.unlock();
//     return rdata;
// }
ErrorType ScriptProcess::ReceiveDatas(const char* script_path) //const std::string &guid 
{
    //std::cout << "wn test ReceiveDatas guid:" << guid << std::endl; 
    std::unique_lock<std::mutex> lock(data_mutex_);
    ErrorType err  = KUnKnown;
    auto iter = cache_local_receive_data.find(script_path);
    if(iter != cache_local_receive_data.end())
    {
        //std::cout<<"+++++"<<iter->first<<":"<<(int)iter->second<<"+++++"<<std::endl;
        err = iter->second;
        //cache_local_receive_data.erase(iter);
    }
    else
    {
        //std::cout<<"+++++"<<"ReceiveDatas find no result"<<"+++++"<<std::endl;
        err = KUnKnown;
    }

    return err;
}