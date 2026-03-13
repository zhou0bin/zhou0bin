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

#ifndef MIDWARE_DIAG_SCRIPT_MANAGER_H
#define MIDWARE_DIAG_SCRIPT_MANAGER_H

#include <map>
#include <queue>
#include <chrono>
#include <memory>
#include <filesystem>

#include <midware/diag/script/loop.h>
#include <midware/diag/script/server.h>
#include <midware/diag/script/context.h>
#include <midware/diag/script/task.h>
#include <midware/diag/script/vehicle_data.h>
#include <midware/diag/script/tester.h>

namespace midware::diag::script {

class Manager: public Context {
public:
    explicit Manager(Poller& poller);
    virtual ~Manager();

public:
    virtual void Init();
    Poller& GetPoller() override;
    const VehicleData& GetVehicleData() const override;
    const std::string& GetPythonCommand() const override;
    const std::vector<std::filesystem::path>& GetScriptPaths() const override;
    void SendMessage(ScriptOpc opc, std::vector<uint8_t> payload) override;
    Future<int> AsyncFork(int priority) override;
    Future<int> AsyncWaitPid(int pid) override;
    void HandleTaskStopped(const std::filesystem::path& task_path) override;
    
private:
    void HandleChld();
    void HandleStartTaskRequest(const std::string& path);
    void HandleStopTaskRequest(const std::string& path);
    void Fork(Promise<int>& promise);
    void LoadPersistenceTasks();

private:
    using Clock = Task::Clock;

    struct ForkRequest {
        int priority;
        Clock::time_point request_time;
        Promise<int> pid;
        bool operator<(const ForkRequest& other) const noexcept;
    };

    using ForkQueue = std::priority_queue<ForkRequest>;
    using WaitPidMap = std::map<int, Promise<int>>;

    Poller& poller_;
    VehicleData vehicle_data_;
    std::string python_command_;
    std::vector<std::filesystem::path> script_paths_;
    std::shared_ptr<Server> server_;
    std::filesystem::path persistence_tasks_dir_;
    ForkQueue fork_queue_;
    WaitPidMap wait_pid_map_;
    size_t fork_queue_max_size_;
    size_t task_max_;
    int current_pid_;
    Signal sigchld_;
    std::map<std::filesystem::path, std::shared_ptr<Task>> tasks_;
    std::unique_ptr<Tester> tester_;
};

} // namespace midware::diag::script

#endif // MIDWARE_DIAG_SCRIPT_MANAGER_H
