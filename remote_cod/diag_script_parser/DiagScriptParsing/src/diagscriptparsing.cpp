#include "diagscriptparsing.h"
#ifdef DIAGSCRIPTPARSING_TEST
ScriptProcess* ScriptProcess::instance_ = nullptr;
std::mutex ScriptProcess::mutex_;

void ScriptProcess::ReceiveTask()
{
    std::cout << __FUNCTION__ << " in! " << std::endl;
    while (local_receive_status)
    {
        uint8_t lenth;
        read(local_socket_,&lenth,1);
        uint8_t type;
        read(local_socket_,&type,1);
        uint8_t err;
        read(local_socket_,&err,1);
        const ssize_t guid_size = static_cast<ssize_t>(lenth -3);
        std::vector<uint8_t> guid(guid_size);
        read(local_socket_, &guid[0], guid_size);
        std::string s_guid(guid.begin(), guid.end());

        struct ReceiveData received_data;
        received_data.itype = (InterfaceType)type;
        received_data.err_code = (ErrorType)err;
        received_data.guid = s_guid;

        if (_cty == NONE && local_receive_status)
        {
            CachMessage(received_data);
        }
        else if (_cty == CLASS && local_receive_status)
        {
            if (!m_callback.expired())
            {
                m_callback.lock()->MessageCallback(received_data);
            }
        }
        else if (_cty == FUNC && local_receive_status)
        {
            if (m_fcall)
            {
                m_fcall(received_data);
            }
        }
    }
    std::cout << __FUNCTION__ << " out! " << std::endl;
    return;
}

void ScriptProcess::CachMessage(ReceiveData& rd)
{
    std::unique_lock<std::mutex> lock(local_mutex_);
    // std::cout << " type: " << (int)rd.itype << " error: " << (int)rd.err_code << " guid: " << rd.guid << std::endl;
    cache_local_receive_data[rd.guid] = rd.err_code;
    // std::cout << " cache size: " << cache_local_receive_data.size() << std::endl;
    // std::vector<struct ReceiveData>::iterator iter = find(cache_local_receive_data.begin(),cache_local_receive_data.end(),rd);
    // auto iter = cache_local_receive_data.find(rd.guid);
    // if (iter == cache_local_receive_data.end() && local_receive_status)
    // {
    //     std::cout << " type: " << (int)rd.itype << " error: " << (int)rd.err_code << " guid: " << rd.guid << std::endl;
    //     cache_local_receive_data.push_back(rd);
    // }
    // else
    // {
    //     if (local_receive_status)
    //     {
    //         cache_local_receive_data.erase(iter);
    //         std::cout << " type: " << (int)rd.itype << " error: " << (int)rd.err_code << " guid: " << rd.guid << std::endl;
    //         cache_local_receive_data.emplace_back(rd);
    //     }
        
    // }
}
ScriptProcess* ScriptProcess::GetInstance()
{
    if (instance_ == nullptr)
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
    strcpy(local_server_address.sun_path,SocketPip);
    int opt = 1;
    setsockopt( local_socket_, SOL_SOCKET,SO_REUSEADDR,(const void *)&opt, sizeof(opt) );
    int ierr = connect(local_socket_,(struct sockaddr*)&local_server_address,sizeof(local_server_address));
    return ierr;
}
int ScriptProcess::Start(const char* SocketPip)
{
    int ierr = Connect(SocketPip);
    if (ierr == 0)
    {
        local_is_connect_ = true;
        local_receive_status = true;
        local_receive_thread = std::thread(&ScriptProcess::ReceiveTask,this);
    }
    else
    {
        local_is_connect_ = false;
    }
    return ierr;
}
int ScriptProcess::Start(const char* SocketPip,std::weak_ptr<Callback> callptr)
{
    int ierr = Connect(SocketPip);
    if (ierr == 0 && !callptr.expired())
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
int ScriptProcess::Start(const char* SocketPip,CALLFUNC call)
{
    int ierr = Connect(SocketPip);
    if (ierr == 0 && call)
    {
        local_is_connect_ = true;
        local_receive_status = true;
        m_fcall = call;
        _cty = FUNC;
        local_receive_thread = std::thread(&ScriptProcess::ReceiveTask,this);
    }
    else
    {
        local_is_connect_ = false;
    }
    return ierr;
}
/*
 * Stop the connection with the server
*/
void ScriptProcess::Stop()
{
    if (local_is_connect_)
    {
        local_is_connect_ = false;
        local_receive_status = false;
        pthread_cancel(local_receive_thread.native_handle());
        if (local_receive_thread.joinable())
        {
            local_receive_thread.join();
        }
        _cty = NONE;
        m_fcall = NULL;
        (void)shutdown(local_socket_,SHUT_RDWR);
        (void)close(local_socket_);
    }
    // std::vector<struct ReceiveData>().swap(cache_local_receive_data);
    cache_local_receive_data.clear();
}
/*
 * Send data
 * Returns the number of bytes sent on success, returns -1 on fail
*/
int ScriptProcess::Send(struct SendData* send_data)
{
    int error = -1;
    std::cout << __FUNCTION__ << " in! " << std::endl;
    const uint8_t len = static_cast<uint8_t>(send_data->guid.size() + 2);
    std::vector<uint8_t> msg{};
    msg.push_back(len);
    msg.push_back(send_data->itype);
    (void)msg.insert(msg.cend(), send_data->guid.begin(), send_data->guid.end());
    if (local_is_connect_)
    {
        error = write(local_socket_, &msg[0], msg.size());
        if (error == 0)
        {
            std::cout << __FUNCTION__ << "socket closed! fd: " << local_socket_ << std::endl;
        }
        else if (error < 0)
        {
            std::cout << __FUNCTION__ << "write error! strerror: " << strerror(errno) << std::endl;
        }
        else
        {
            std::cout << __FUNCTION__ << "success! fd: " << local_socket_ << "error: " << error << std::endl;
        }
    }
    std::cout << __FUNCTION__ << " out! " << std::endl;
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
//     if (iter != cache_local_receive_data.end())
//     {
//         std::cout << "error" << iter->err_code << " guid " << iter->guid << std::endl;
//         rdata = *iter;
//     }
//     lock.unlock();
//     return rdata;
// }
ErrorType ScriptProcess::ReceiveDatas(const char* guid)
{
    std::cout << __FUNCTION__ << " in! " << std::endl;
    ErrorType err  = KUnKnown;
    auto iter = cache_local_receive_data.find(guid);
    if (iter != cache_local_receive_data.end())
    {
        std::cout << iter->first << ", " << (int)iter->second << std::endl;
        err = iter->second;
        // cache_local_receive_data.erase(iter);
    }
    else
    {
        err = KUnKnown;
    }
    std::cout << __FUNCTION__ << " out! " << std::endl;
    return err;
}
#endif
