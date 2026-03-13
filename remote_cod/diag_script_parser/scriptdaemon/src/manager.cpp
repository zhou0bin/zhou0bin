//
//
#include <sys/wait.h>

#include <rapidjson/istreamwrapper.h>

#include <midware/diag/script/log.h>
#include <midware/diag/script/manager.h>

namespace midware::diag::script {

bool Manager::ForkRequest::operator<(const ForkRequest& other) const noexcept {
    if (this == &other) {
        return false;
    }
    if (priority < other.priority) {
        return true;
    }
    if (priority > other.priority) {
        return false;
    }
    return (request_time < other.request_time);
}

Manager::Manager(Poller& poller)
    : poller_(poller)
    , vehicle_data_(GetVehicleDataPath())
    , current_pid_(-1)
    , sigchld_(poller) {
}

Manager::~Manager() {
    LOG_INFO << "Stopping manager...";
    server_->Stop();
    sigchld_.Stop();
}

void Manager::Init() {
#ifdef SECURE_STARTUP
    LOG_ERROR << "Secure Startup! ";
#else
    LOG_ERROR << "Unsecure Startup! ";
#endif

    const std::string config_path = GetConfigPath();
    LOG_INFO << "Config path: " << config_path;

    std::ifstream ifs(config_path);
    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document doc;
    doc.ParseStream(isw);
    if (!doc.IsObject()) {
        LOG_ERROR << "Invalid task json file: not a object.";
        throw std::invalid_argument("The task json file is not a object.");
    }

    std::string socket_path;
    auto socket_path_it = doc.FindMember("Socket_Pip_File");
    if (socket_path_it != doc.MemberEnd() && socket_path_it->value.IsString()) {
        socket_path = socket_path_it->value.GetString();
        LOG_INFO << "socket_path: " << socket_path;
    } else {
        LOG_ERROR << "Invalid task json file: no socket_path.";
        throw std::invalid_argument("Invalid task json file: no socket_path.");
    }

    (void)unlink(socket_path.c_str());
    server_ = std::make_shared<Server>(poller_, socket_path);
    server_->StartTaskRequest = [this](const std::string& path) {
        HandleStartTaskRequest(path);
    };
    server_->StopTaskRequest = [this](const std::string& path) {
        HandleStopTaskRequest(path);
    };

    auto task_number_it = doc.FindMember("Task_Number");
    if (task_number_it != doc.MemberEnd() && task_number_it->value.IsInt()) {
        int task_number = task_number_it->value.GetInt();
        LOG_INFO << "task_number: " << task_number;
        fork_queue_max_size_ = std::clamp(static_cast<size_t>(task_number), static_cast<size_t>(1), static_cast<size_t>(100));
        task_max_ = fork_queue_max_size_ + 50;
    } else {
        LOG_ERROR << "Invalid task json file: no task_number.";
        throw std::invalid_argument("Invalid task json file: no task_number.");
    }

    LOG_INFO << "Loading python command...";
    auto python_command_it = doc.FindMember("Python_Command");
    if (python_command_it != doc.MemberEnd() && python_command_it->value.IsString()) {
        python_command_ = python_command_it->value.GetString();
        LOG_INFO << "python_command: " << python_command_;
    } else {
        LOG_WARN << "No python command, use default.";
        python_command_ = "python3";
    }

    LOG_INFO << "Loading script paths...";
    
    auto script_paths_it = doc.FindMember("ScriptPaths");
    if (script_paths_it != doc.MemberEnd() && script_paths_it->value.IsArray()) {
        for (auto& script_path: script_paths_it->value.GetArray()) {
            if (script_path.IsString()) {
                LOG_INFO << "script_path: " << script_path.GetString();
                script_paths_.push_back(script_path.GetString());
            }
        }
    }

    LOG_INFO << "Loading persistence tasks...";
    auto persistence_task_dir_it = doc.FindMember("PersistenceTasksDir");
    if (persistence_task_dir_it == doc.MemberEnd() || !persistence_task_dir_it->value.IsString()) {
        LOG_ERROR << "Invalid task json file: no persistence tasks dir.";
        throw std::invalid_argument("Invalid task json file: no persistence tasks dir.");
    }
    persistence_tasks_dir_ = persistence_task_dir_it->value.GetString();
    LOG_INFO << "Persistence tasks dir: " << persistence_tasks_dir_.string();

    sigchld_.Start(SIGCHLD, [this](int) {
        LOG_INFO << "SIGCHLD";
        HandleChld();
    });

    LOG_INFO << "Starting server...";
    server_->Start();

    bool test = false;
    ParseJson(config_path.c_str(), "Test", test);
    if (test) {
        tester_ = std::make_unique<Tester>(socket_path);
    }

    try {
        LoadPersistenceTasks();
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to load persistence tasks: " << e.what();
    } catch (...) {
        LOG_ERROR << "Failed to load persistence tasks.";
    }
}

Poller& Manager::GetPoller() {
    return poller_;
}

const VehicleData& Manager::GetVehicleData() const {
    return vehicle_data_;
}

const std::string& Manager::GetPythonCommand() const {
    return python_command_;
}

const std::vector<std::filesystem::path>& Manager::GetScriptPaths() const {
    return script_paths_;
}

void Manager::SendMessage(ScriptOpc opc, std::vector<uint8_t> payload) {
    server_->SendMessage(opc, payload);
}

Future<int> Manager::AsyncFork(int priority) {
    if (current_pid_ > 0) {
        LOG_INFO << "Async forking...";
        Promise<int> promise;
        auto future = promise.GetFuture();
        fork_queue_.push({ priority, Clock::now(), std::move(promise) });
        return future;
    }
    
    LOG_INFO << "Forking...";
    Promise<int> promise;
    Fork(promise);

    return promise.GetFuture();
}

Future<int> Manager::AsyncWaitPid(int pid) {
    LOG_INFO << "Async waiting pid: " << pid;
    Promise<int> promise;
    auto future = promise.GetFuture();
    wait_pid_map_[pid] = std::move(promise);
    return future;
}

void Manager::HandleTaskStopped(const std::filesystem::path& task_path) {
    try {
        LOG_INFO << "Task " << task_path.string() << " stopped, removing it.";
        poller_.Schedule([this, task_path] {
            try {
                (void)std::filesystem::remove(persistence_tasks_dir_ / task_path.filename());
            } catch (...) {
            }
            auto task_it = tasks_.find(task_path);
            if (task_it != tasks_.end()) {
                tasks_.erase(task_it);
            }
            server_->SendMessage(ScriptOpc::kStopSuccess, task_path);
        });
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to wait task stopped: " << task_path.string() << ": " << e.what();
    } catch (...) {
        LOG_ERROR << "Failed to wait task stopped: " << task_path.string();
    }
}

void Manager::HandleChld() {
    LOG_INFO << "Handling SIGCHLD, current_pid_: " << current_pid_;
    if (current_pid_ > 0) {
        int status = 0;
        auto pid = waitpid(current_pid_, &status, WNOHANG);
        if (pid == current_pid_) {
            LOG_DEBUG << "WaitPid ok.";
            current_pid_ = -1;
            auto it = wait_pid_map_.find(pid);
            if (it != wait_pid_map_.end()) {
                LOG_DEBUG << "Complete async waitpid...";
                it->second.Set(status);
                wait_pid_map_.erase(it);
            }
        } else {
            LOG_WARN << "Failed to waitpid: " << strerror(errno);
        }
    } else {
        LOG_WARN << "No child process exited.";
    }

    while (!fork_queue_.empty()) {
        auto fr = std::move(const_cast<ForkRequest&>(fork_queue_.top()));
        fork_queue_.pop();
        if (fr.pid.IsPending()) {
            Fork(fr.pid);
        } else {
            LOG_INFO << "Fork request already handled.";
        }
    }
}

void Manager::HandleStartTaskRequest(const std::string& path) {
    LOG_ERROR << "HandleStartTaskRequest: " << path;

    server_->SendMessage(ScriptOpc::kWaiting, path);

    auto task_path = std::filesystem::canonical(path);
    auto it = tasks_.find(task_path);
    if (it != tasks_.end()) {
        LOG_ERROR << "Task already exists: " << task_path.string();
        server_->SendMessage(ScriptOpc::kUnKnown, path);
        throw std::runtime_error("Task already exists.");
    }

    if (tasks_.size() >= task_max_) {
        LOG_ERROR << "Reached max tasks limit.";
        server_->SendMessage(ScriptOpc::kTooMany, path);
        throw std::runtime_error("Reached max tasks limit.");
    }

    auto task = std::make_shared<Task>(*this, task_path);
    if (task->GetType() != Task::Type::kTemp) {
        LOG_INFO << "Creating persistence symlink for task: " << task_path.string();
        try {
            bool created = std::filesystem::create_directories(persistence_tasks_dir_);
            if(!created){
                LOG_ERROR << "Failed to create persistence tasks dir: " << persistence_tasks_dir_.string();
            }
        } catch (...) {
            LOG_ERROR << "Failed to create persistence tasks dir: " << persistence_tasks_dir_.string();
        }
        try {
            std::filesystem::create_symlink(task_path, persistence_tasks_dir_ / task_path.filename());
        } catch (...) {
            LOG_ERROR << "Failed to create persistence symlink for task: " << task_path.string();
        }
    }

    task->Start();
    tasks_[task_path] = task;
}

void Manager::HandleStopTaskRequest(const std::string& path) {
    LOG_ERROR << ": " << path;
    std::filesystem::path task_path(std::filesystem::absolute(path));
    auto it = tasks_.find(task_path);
    if (it != tasks_.end()) {
        try {
            server_->SendMessage(ScriptOpc::kStoping, path);
            it->second->Stop();
        } catch (ScriptOpc opc) {
            server_->SendMessage(opc, path);
        } catch (const std::exception& e) {
            LOG_ERROR << "Failed to stop task: " << e.what();
            server_->SendMessage(ScriptOpc::kUnKnown, path);
        } catch (...) {
            LOG_ERROR << "Failed to stop task.";
            server_->SendMessage(ScriptOpc::kUnKnown, path);
        }
    } else {
        LOG_WARN << "No such task: " << task_path.string();
        server_->SendMessage(ScriptOpc::kUnKnown, path);
    }
}

void Manager::Fork(Promise<int>& pid) {
    LOG_INFO << "Forking process...";
    assert(current_pid_ == -1);
    current_pid_ = fork();
    if (current_pid_ > 0) {
        LOG_INFO << "pid: " << current_pid_;
        pid.Set(current_pid_);
    } else if (current_pid_ == 0) {
        server_->ForkChild();
        pid.Set(current_pid_);
    } else {
        pid.Set(std::make_exception_ptr(std::system_error(errno, std::system_category())));
    }
}

void Manager::LoadPersistenceTasks() {
    for (auto& dent: std::filesystem::directory_iterator(persistence_tasks_dir_)) {
        try {
            LOG_INFO << "Loading persistence task: " << dent.path().string();
            if (tasks_.size() >= task_max_) {
                LOG_ERROR << "Reached max tasks limit.";
                break;
            }
            if (!std::filesystem::is_symlink(dent)) {
                LOG_WARN << "Not a symlink: " << dent.path().string() << ", skipped.";
                continue;
            }
            auto task_dir = std::filesystem::read_symlink(dent);
            if (!std::filesystem::is_directory(task_dir)) {
                LOG_WARN << "Not a directory: " << task_dir.string() << ", skipped.";
                continue;
            }
            auto task = std::make_shared<Task>(*this, task_dir);
            task->Start();
            tasks_[task->GetPath()] = task;
        } catch (const std::exception& e) {
            LOG_ERROR << "Failed to load persistence task: " << dent.path().string() << ", " << e.what();
        } catch (...) {
            LOG_ERROR << "Failed to load persistence task: " << dent.path().string();
        }
    }
}

} // namespace midware::diag::script