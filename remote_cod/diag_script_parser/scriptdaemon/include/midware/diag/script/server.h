// --------------------------------------------------------------------------
// |                _     _              _____         _____                |
// |               |  \  | |            / ____|  /\   |  __ \               |
// |               | | \ | |  __       | (___   /  \  | |__) |              |
// |               | |\ \| | /__\|   |  \___ \ / /\ \ |  _  /               |
// |               | | \ \ ||    |   |   ___) / /__\ \| | \ \               |
// |               |_|  \_\| \__/ \_/|/|_____/________\_|  \_\              |
// |                                                                        |
// --------------------------------------------------------------------------
// COPYRIGHT
// --------------------------------------------------------------------------
//
// This software is copyright protected and proprietary to Neusoft Reach.
// Neusoft Reach grants to you only those rights as set out in the license 
// conditions.
// All other rights remain with Neusoft Reach.
// --------------------------------------------------------------------------

#ifndef MIDWARE_DIAG_SCRIPT_SERVER_H
#define MIDWARE_DIAG_SCRIPT_SERVER_H

#include <string>
#include <filesystem>
#include <functional>

#include <midware/diag/script/common.h>
#include <midware/diag/script/loop.h>

#define EPOLL_MAX 20

namespace midware::diag::script {

class Server {
public:
    std::function<void (const std::string&)> StartTaskRequest;
    std::function<void (const std::string&)> StopTaskRequest;

public:
    explicit Server(Poller& poller, std::string& path);
    Server(Server&) = delete;
    Server(Server&&) = delete;
    virtual ~Server();
    Server& operator=(const Server&) = delete;
    Server& operator=(Server&&) = delete;

    void Start();
    void Stop();
    void ForkChild();
    void SendMessage(ScriptOpc opc, std::vector<uint8_t> payload);

    void SendMessage(ScriptOpc opc, const std::string& payload) {
        SendMessage(opc, std::vector<uint8_t>(payload.begin(), payload.end()));
    }

private:
    void CloseSocket();
    void HandleListenReadable();
    void HandleClientReadable();
    static bool ChangeResourceFileMode(const std::string& path);
    static bool SetClinetNotBlock(const int client_fd);

private:
    Poller& poller_;
    std::filesystem::path path_;
    int listen_sk_;
    int client_sk_;
};

} // namespace midware::diag::script

#endif // MIDWARE_DIAG_SCRIPT_SERVER_H
