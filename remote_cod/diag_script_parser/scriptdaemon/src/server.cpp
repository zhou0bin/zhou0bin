//
//
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <midware/diag/script/log.h>
#include <midware/diag/script/scopeguard.h>
#include <midware/diag/script/server.h>

namespace midware::diag::script {

Server::Server(Poller& poller, std::string& path)
    : poller_(poller)
    , path_(path)
    , listen_sk_(-1)
    , client_sk_(-1) {
    LOG_INFO << path_.string();
}

Server::~Server() {
    LOG_INFO;
    Stop();
}

void Server::Start() {
    LOG_INFO << __func__;
    assert(listen_sk_ < 0);

    LOG_INFO << "path: " << path_.string();

    (void)std::filesystem::remove(path_);
    auto failed_guard = MakeScopeGuard([this]() {
        (void)std::filesystem::remove(path_);
    });

    struct sockaddr_un local_addr;
    (void)memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sun_family = AF_UNIX;
    (void)strcpy(local_addr.sun_path, path_.c_str());

    listen_sk_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_sk_ < 0) {
        LOG_ERROR << "Create Inter Server Fail: " << strerror(errno);
        throw std::system_error(errno, std::system_category());
    }
    auto listen_sk_guard = MakeScopeGuard([this] {
        close(listen_sk_);
        listen_sk_ = -1;
    });

    LOG_INFO << "server sk: " << listen_sk_;

    int opt = 1;
    int ret = setsockopt(listen_sk_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ret < 0) {
        LOG_ERROR << "Set Inter Socket Param Fail: " << strerror(errno);
        throw std::system_error(errno, std::system_category());
    }

    ret = bind(listen_sk_, reinterpret_cast<struct sockaddr *>(&local_addr), sizeof(local_addr));
    if (ret < 0) {
        LOG_ERROR << "Bind Inter Param Fail: " << strerror(errno);
        throw std::system_error(errno, std::system_category());
    }

    if (!ChangeResourceFileMode(path_)) {
        LOG_WARN << "ChangeResourceFileMode failed: " << path_.string();
    }

    ret = listen(listen_sk_, EPOLL_MAX);
    if (ret < 0) {
        LOG_ERROR << "Listen fail: " << strerror(errno);
        throw std::system_error(errno, std::system_category());
    }

    poller_.AddFd(listen_sk_, Poller::kEventReadable, [this](int events) {
        if (events & Poller::kEventReadable) {
            LOG_INFO << "Listen socket " << listen_sk_ << " is readable! ";
            HandleListenReadable();
        }
    });

    listen_sk_guard.Dismiss();
    failed_guard.Dismiss();
}

void Server::Stop() {
    LOG_INFO << __func__;
    CloseSocket();
    try {
        (void)std::filesystem::remove(path_);
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to remove socket file: " << e.what();
    }
}

void Server::ForkChild() {
    CloseSocket();
}

void Server::SendMessage(ScriptOpc opc, std::vector<uint8_t> payload) {
    LOG_INFO << "opc: " << static_cast<int>(opc) << " payload: " << payload.size();
    if (client_sk_ >= 0) {
        std::vector<uint8_t> msg;
        msg.push_back(2 + payload.size());
        msg.push_back(opc);
        if (payload.size() > 0) {
            msg.insert(msg.end(), payload.begin(), payload.end());
        }
        ssize_t write_size = write(client_sk_, msg.data(), msg.size());
        if(write_size == -1){
            LOG_ERROR << "Failed to send message: " << strerror(errno);
        }
    } else {
        LOG_ERROR << "No client connected.";
    }
}

void Server::CloseSocket() {
    if (client_sk_ >= 0) {
        poller_.RemoveFd(client_sk_);
        (void)close(client_sk_);
        client_sk_ = -1;
    }
    if (listen_sk_ >= 0) {
        poller_.RemoveFd(listen_sk_);
        (void)close(listen_sk_);
        listen_sk_ = -1;
    }
}

void Server::HandleListenReadable() {
    LOG_INFO << "accepting client...";
    struct sockaddr_un add_in;
    socklen_t addlen = sizeof(add_in);
    int client_sk = accept(listen_sk_, reinterpret_cast<struct sockaddr *>(&add_in), &addlen);
    if (client_sk < 0) {
        LOG_ERROR << "accept error: " << strerror(errno);
        return;
    }

    if (client_sk_ >= 0) {
        LOG_INFO << "Already connected.";
        close(client_sk);
        return;
    }

    LOG_INFO << "Client connected! client_sk: " << client_sk;
    (void)SetClinetNotBlock(client_sk);
    client_sk_ = client_sk;

    poller_.AddFd(client_sk_, Poller::kEventReadable, [this](int client_events) {
        if (client_events & Poller::kEventReadable) {
            HandleClientReadable();
        }
    });
}

void Server::HandleClientReadable() {
    LOG_INFO << "in! ";

    uint8_t head[2];

    int ret = read(client_sk_, &head, sizeof(head));
    if (ret == 0) {
        LOG_INFO << "Client disconnected!";
        poller_.RemoveFd(client_sk_);
        (void)shutdown(client_sk_, SHUT_RDWR);
        (void)close(client_sk_);
        client_sk_ = -1;
        return;
    }

    if (ret != sizeof(head)) {
        LOG_ERROR << "read len error! ret: " << ret;
        return;
    }
    if (head[0] < 2) {
        LOG_ERROR << "len error! len: " << head[0];
        return;
    }

    uint8_t payload[256];
    uint8_t payload_size = head[0] - 2;
    if (payload_size > 0) {
        ret = read(client_sk_, payload, payload_size);
        if (ret != payload_size) {
            LOG_ERROR << "read len error: " << strerror(errno);
            return;
        }
    }

    switch (head[1]) {
    case ScriptOpc::kStartTask:
        if (StartTaskRequest) {
            LOG_INFO << "StartTaskRequest received.";
            auto path = std::string(payload, payload + payload_size);
            try {
                StartTaskRequest(path);
            } catch(ScriptOpc opc) {
                SendMessage(opc, path);
            } catch (const std::exception& e) {
                LOG_ERROR << "Failed to handle start task request: " << e.what();
                SendMessage(ScriptOpc::kUnKnown, path);
            } catch (...) {
                SendMessage(ScriptOpc::kUnKnown, path);
            }
        }
        break;
    case ScriptOpc::kStopTask:
        if (StopTaskRequest) {
            LOG_INFO << "StopTaskRequest received.";
            auto path = std::string(payload, payload + payload_size);
            try {
                StopTaskRequest(path);
            } catch (ScriptOpc opc) {
                LOG_ERROR << "Failed to handle stop task request: " << static_cast<int>(opc);
                SendMessage(opc, path);
            } catch (const std::exception& e) {
                LOG_ERROR << "Failed to handle stop task request: " << e.what();
                SendMessage(ScriptOpc::kUnKnown, path);
            } catch (...) {
                LOG_ERROR << "Failed to handle stop task request.";
                SendMessage(ScriptOpc::kUnKnown, path);
            }
        }
        break;
    default:
        LOG_ERROR << "Unknown cmd received: " << head[1];
        break;
    }
}

bool Server::SetClinetNotBlock(const int client_fd) {
    const int old_option = fcntl(client_fd, F_GETFL);
    const int new_option = old_option | O_NONBLOCK;
    if (fcntl(client_fd, F_SETFL, new_option) < 0) {
        LOG_ERROR << "fcntl error! strerror: " << strerror(errno);
        return false;
    }
    LOG_DEBUG << "old_option: " << old_option << "new_option: " << new_option;
    return true;
}

bool Server::ChangeResourceFileMode(const std::string& path) {
    const char *const env = getenv("NEUSAR_RESOURCE_FILE_CHMOD");
    if (env == NULL) {
        LOG_INFO << "NEUSAR_RESOURCE_FILE_CHMOD not set.";
        return true;
    }
    mode_t mode{};
    try {
        mode = std::stoul(env, nullptr, 8);
    } catch (std::invalid_argument &) {
        LOG_ERROR << "NEUSAR_RESOURCE_FILE_CHMOD throw invalid_argument. " << env;
        return false;
    } catch (std::out_of_range &) {
        LOG_ERROR << "NEUSAR_RESOURCE_FILE_CHMOD throw out_of_range. " << env;
        return false;
    }
    int ret = chmod(path.c_str(), mode);
    if (ret != 0) {
        LOG_ERROR << "ChangeResourceFileMode " << path << "to mode " << env << "failed.";
        return false;
    }
    LOG_INFO << "ChangeResourceFileMode " << path << "to mode " << env << "succeed.";
    return true;
}

} // namespace midware::diag::script
