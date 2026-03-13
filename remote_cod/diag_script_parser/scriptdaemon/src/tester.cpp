//
//
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <midware/diag/script/log.h>
#include <midware/diag/script/scopeguard.h>
#include <midware/diag/script/tester.h>

namespace midware::diag::script {

Tester::Tester(const std::string& socket_path)
    : thread_(&Tester::Thread, this, std::string(socket_path)) {
    LOG_INFO << "Start Tester Thread";
}

Tester::~Tester() {
    try {
        thread_.join();
        LOG_INFO << "Stop Tester Thread";
    } catch (...) {
    }
}

void Tester::Thread(std::string socket_path) {
    auto end_log_guard = MakeScopeGuard([] {
        LOG_INFO << "End of testing...";
    });

    LOG_INFO << "Sleeping...";
    std::this_thread::sleep_for(std::chrono::seconds(3));

    LOG_INFO << "Creating socket...";
    int sk = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sk < 0) {
        LOG_ERROR << "Failed to create socket: " << std::strerror(errno);
        return;
    }
    auto sk_guard = MakeScopeGuard([sk] {
        close(sk);
    });

    LOG_INFO << "Connecting socket...";
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path.c_str());
    if (connect(sk, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        LOG_ERROR << "Failed to connect socket: " << std::strerror(errno);
        return;
    }

    std::string task_path = "/home/shaomy/Projects/mw/diag_test/tasks/test1";

    LOG_INFO << "Sending start task request...";
    std::vector<uint8_t> request;
    request.push_back(2 + task_path.size());
    request.push_back(ScriptOpc::kStartTask);
    request.insert(request.end(), task_path.begin(), task_path.end());
    if (send(sk, request.data(), request.size(), 0) < 0) {
        LOG_ERROR << "Failed to send request: " << std::strerror(errno);
        return;
    }

    LOG_INFO << "Waiting for response...";
    uint8_t buf[1024] = { 0 };
    int ret = recv(sk, buf, sizeof(buf) - 1, 0);
    if (ret > 0) {
        LOG_INFO << "Got response: " << buf[1] << ", " << buf + 2;
    } else if (ret == 0) {
        LOG_INFO << "Got EOF";
    } else {
        LOG_ERROR << "Failed to recv response: " << std::strerror(errno);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
}

} // namespace midware::diag::script
