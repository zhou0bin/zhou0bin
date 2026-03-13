//
//
#include <fstream>

#include <sys/wait.h>
#include <sys/prctl.h>
#include <unistd.h>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>

#include <midware/diag/script/log.h>
#include <midware/diag/script/scopeguard.h>
#include <midware/diag/script/parse_json.h>
#include <midware/diag/script/task.h>

#define PYTHON_PIPE_R 0 
#define PYTHON_PIPE_W 1
#define PYTHON_PID_LOG_FLAG 0b00000000
#define PYTHON_INIT 0b00000001
#define PYTHON_MODU 0b00000010
#define PYTHON_FUNC 0b00000100
#define PYTHON_RUN  0b00001000
#define PYTHON_EXCEPT 0b00010000
#define PYTHON_INIT_EXIT 201
#define PYTHON_MODU_EXIT 202
#define PYTHON_FUNC_EXIT 203
#define PYTHON_NORMAL_EXIT 0
#define PYTHON_EXCEPT_EXIT 205
#define PYTHON_PID_START 1U

#ifdef __GNUC__
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

namespace midware::diag::script {

Task::Task(Context& context, const std::filesystem::path& path)
    : context_(context)
    , path_(std::filesystem::canonical(path))
    , type_(Type::kTemp)
    , started_(false)
    , priority_(0)
    , periodic_timer_(context.GetPoller())
    , conditions_(context, path / "condition_check.json")
    , result_(ScriptOpc::kExcutedOver)
    , result_sent_(false) {
    LOG_INFO << path_.string();

    // script_path_ = path_ / (path_.filename().string() + ".py");
    // LOG_INFO << "script path: " << script_path_;
    // if (!std::filesystem::exists(script_path_)) {
    //     LOG_INFO << " script " << script_path_.string() << " is not existed.";
    //     throw ScriptOpc::kNoscript;
    // }

    // 查找 path_ 路径下的.py 文件
    bool script_found = false;
    for (const auto& entry : std::filesystem::directory_iterator(path_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".py") {
            script_path_ = entry.path();
            script_found = true;
            LOG_INFO << "script path: " << script_path_;
            break;
        }
    }
    if (!script_found) {
        LOG_INFO << " script in " << path_.string() << " is not existed.";
        throw ScriptOpc::kNoscript;
    }

    auto task_config_path = path_ / "task_config.json";
    LOG_INFO << "Loading " << task_config_path.string() << "...";
    std::ifstream ifs(task_config_path);
    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document doc;
    doc.ParseStream(isw);
    if (!doc.IsObject()) {
        LOG_ERROR << "Invalid task json file: not a object.";
        throw ScriptOpc::kNoscript;
    }

    int type;
    auto type_it = doc.FindMember("excutionMode");
    if (type_it != doc.MemberEnd() && type_it->value.IsInt()) {
        type = type_it->value.GetInt();
        LOG_INFO << "type: " << type;
        if (type == 1) {
            type_ = Type::kTemp;
        } else if (type == 2) {
            type_ = Type::kEvent;
        } else if (type == 3) {
            type_ = Type::kPeriodic;
            auto periodic_it = doc.FindMember("taskPeriod");
            if (periodic_it != doc.MemberEnd() && periodic_it->value.IsInt()) {
                int periodic = periodic_it->value.GetInt();
                LOG_INFO << "periodic: " << periodic;
                auto v = periodic < 1 ? 1 : periodic;
                periodic_ = std::chrono::seconds(v);
            } else {
                LOG_ERROR << "Invalid periodic task json file: no periodic.";
                throw ScriptOpc::kNoscript;
            }
        } else {
            LOG_ERROR << "Invalid task json file: invalid type: " << type;
            throw ScriptOpc::kNoscript;
        }
    } else {
        LOG_WARN << "No type specified, assuming temp.";
    }

    LOG_INFO << "Loading priority...";
    auto priority_it = doc.FindMember("taskPriority");
    if (priority_it != doc.MemberEnd() && priority_it->value.IsInt()) {
        int priority = priority_it->value.GetInt();
        LOG_INFO << "priority: " << priority;
        priority_ = std::clamp(priority, 0, 100);
    } else {
        LOG_WARN << "No priority specified, assuming 0.";
    }

    sys_paths_ = path_.string();
    auto script_paths = context_.GetScriptPaths();
    for (auto script_path: script_paths) {
        sys_paths_ += ":" + script_path.string();
    }
}

Task::~Task() {
    LOG_INFO << path_.string();
    try {
        Stop();
        SendResult(result_);
    } catch(...) {
    }
}

void Task::Start() {
    LOG_INFO << path_.string();

    if (started_) {
        LOG_ERROR << "Already started.";
        return;
    }

    started_ = true;
    // assert(!async_user_auth_);

    // if (conditions_.IsUserAuth()) {
    //     async_user_auth_ = context_.AsyncUserAuth(script_path_.filename()).Then([this](auto& future) {
    //         try {
    //             if (future.Get()) {
    //                 OnUserAuthAccepted();
    //             } else {
    //                 SetStopped(ScriptOpc::kConditionNotAllow); // TODO needs a new opc?
    //             }
    //         } catch (const std::exception& e) {
    //             LOG_ERROR << "User auth failed: " << e.what();
    //             SetStopped(ScriptOpc::kUnKnown);
    //         } catch (...) {
    //             LOG_ERROR << "User auth failed: unknown error.";
    //             SetStopped(ScriptOpc::kUnKnown);
    //         }
    //     });
    // } else {
        OnUserAuthAccepted();
    // }
}

void Task::Stop() {
    LOG_INFO << path_.string();

    if (!started_) {
        LOG_ERROR << "Not started.";
        return;
    }

    // if (async_user_auth_.IsPending()) {
    //     LOG_INFO << "Canceling user auth...";
    //     async_user_auth_.Cancel();
    // }

    if (async_fork_.IsPending()) {
        LOG_INFO << "Canceling fork...";
        async_fork_.Cancel();
    }

    if (async_waitpid_.IsPending()) {
        LOG_INFO << "Canceling waitpid...";
        async_waitpid_.Cancel();
    }

    if (pid_ > 0) {
        LOG_INFO << "Killing process: " << pid_ << " ...";
        int kill_ret = kill(pid_, SIGTERM);
        if (kill_ret == -1) {
            LOG_ERROR << __func__  << "Failed to kill process: " << pid_ << " : " << strerror(errno);
        } else {
            LOG_INFO << __func__ << "Successfully killed process: " << pid_;
        }
        
        pid_ = -1;
    }
 
    switch (type_) {
    case Type::kPeriodic:
        periodic_timer_.Stop();
        break;
    case Type::kEvent:
        if (async_wait_conditions_.IsPending()) {
            async_wait_conditions_.Cancel();
        }
        break;
    case Type::kTemp:
        SetStopped();
        break;
    default:
        break;
    }
}

void Task::SetStopped(ScriptOpc opc) {
    if (started_) {
        started_ = false;
        context_.HandleTaskStopped(path_);
        (void)opc; // SendResult(opc);
    }
}

void Task::OnUserAuthAccepted() {
    switch (type_) {
    case Type::kTemp:
        LOG_INFO << "Starting temp task...";
        StartProcess();
        break;
    case Type::kPeriodic:
        LOG_INFO << "Starting periodic task...";
        periodic_timer_.Start(std::chrono::seconds(0), periodic_, [this] {
            StartProcess();
        });
        break;
    case Type::kEvent:
        LOG_INFO << "Starting event task...";
        async_wait_conditions_ = conditions_.AsyncWait().Then([this](auto& future) {
            try {
                future.Get();
                StartProcess();
            } catch (...) {
            }
            //SetStopped();
        });
        break;
    default:
        assert(false);
        break;
    }
}

void Task::StartProcess() {
    LOG_INFO << path_.string();

    if (pid_ > 0) {
        LOG_WARN << "Prev execution is not finished: " << pid_;
        return;
    }

    if (type_ == Type::kTemp || type_ == Type::kPeriodic) {
        conditions_.UpdateAllValues();
    }

    if (!conditions_.Check()) {
        LOG_ERROR << "Conditions not met.";
        SendResult(ScriptOpc::kConditionNotAllow);
        return;
    }

    try {
        async_fork_ = context_.AsyncFork(priority_).Then([this](auto& future) {
            try {
                HandleForked(future.Get());
            } catch(ScriptOpc opc) {
                SendResult(opc);
            } catch (const std::exception& e) {
                LOG_ERROR << "Fork failed: " << e.what();
                SendResult(ScriptOpc::kUnKnown);
            } catch (...) {
                LOG_ERROR << "Fork failed.";
                SendResult(ScriptOpc::kUnKnown);
            }
        });
    } catch (ScriptOpc opc) {
        SendResult(opc);
    } catch (const std::exception& e) {
        LOG_ERROR << "Async fork error: " << strerror(errno);
        SendResult(ScriptOpc::kUnKnown);
    } catch (...) {
        LOG_ERROR << "Async fork error.";
        SendResult(ScriptOpc::kUnKnown);
    }
}

void Task::HandleForked(int pid) {
    LOG_INFO << __func__ << path_.string() << " pid: " << pid;
    if (pid < 0) {
        LOG_ERROR << "Fork failed.";
        SendResult(ScriptOpc::kExcutedFaile);
        return;
    }

    if (pid == 0) {
        Child();
        return;
    }

    context_.SendMessage(ScriptOpc::kScriptRunning, path_.string());

    pid_ = pid;

    LOG_INFO << "Forked child process pid: " << pid;
    async_waitpid_ = context_.AsyncWaitPid(pid).Then([this](auto& waitpid_future) {
        try {
            HandleExited(waitpid_future.Get());
        } catch (const std::exception& e) {
            LOG_ERROR << "Failed to waitpid " << ": " << e.what();
        } catch (...) {
            LOG_ERROR << "Failed to waitpid.";
        }
    });

    LOG_INFO << "Parent Process! pid: " << pid;
}

void Task::HandleExited(int status) {
    LOG_INFO << path_.string();

    pid_ = -1;

    // auto stop_guard = MakeScopeGuard([this] {
    //     if (type_ == Type::kTemp || type_ == Type::kEvent /* TODO || type_ == Type::kPeriodic */) {
    //         SetStopped();
    //     }
    // });

    auto opc = ScriptOpc::kExcutedFaile;
    auto guard = MakeScopeGuard([this, &opc] {
        SendResult(opc);
    });

    if (WIFEXITED(status)) {
        auto ret = WEXITSTATUS(status);
        LOG_INFO << "Child Process Exited: " << ret;
        if (ret == 0) {
            try {
                BuildResultFile();
                opc = ScriptOpc::kExcutedOver;
            } catch (const std::exception& e) {
                LOG_ERROR << "Failed to build result file: " << e.what();
            } catch (...) {
                LOG_ERROR << "Failed to build result file.";
            }
        } else {
            LOG_ERROR << "Python Fail: " << ret;
        }
    } else if (WIFSIGNALED(status)) {
        LOG_WARN << "Child Process Killed: " << WTERMSIG(status);
    } else if (WIFSTOPPED(status)) {
        LOG_WARN << "Child Process Stopped: " << WSTOPSIG(status);
    } else {
        LOG_WARN << "Child Process Ended.";
    }
}

void Task::BuildResultFile() {
    LOG_INFO << path_.string();

    auto result_path = path_ / "result";

    try {
        std::filesystem::remove_all(result_path);
    } catch (...) {
    }

    try {
        std::filesystem::create_directories(result_path);
    } catch (...) {
    }

    auto fail_guard = MakeScopeGuard([result_path] {
        (void)std::filesystem::remove_all(result_path);
        (void)std::filesystem::remove(GetDiagResultPath());
    });

    LOG_INFO << "Loading results...";
    std::ifstream ifs(GetDiagResultPath());
    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document doc;
    doc.ParseStream(isw);
    if (!doc.IsObject()) {
        throw std::runtime_error("Generated tempcheckresult.json is not a object.");
    }
    
    LOG_INFO << "Generating final results...";
    std::ofstream ofs(result_path / "DiagResult.json");
    rapidjson::OStreamWrapper osw(ofs);
    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
    (void)writer.SetIndent(' ', 4);
    (void)writer.SetFormatOptions(rapidjson::kFormatDefault);

    (void)writer.StartObject();

    LOG_DEBUG << "Writing vehicle data...";
    context_.GetVehicleData().Dump(writer);

    LOG_DEBUG << "Writing result...";
    (void)writer.Key("Result");
    (void)writer.StartObject();

    (void)writer.Key("Location");
    (void)writer.String("Init");

    (void)writer.Key("TotalResult");
    auto result_it = doc.FindMember("Result");
    if (result_it != doc.MemberEnd() && result_it->value.IsObject()) {
        auto total_result_it = result_it->value.FindMember("TotalResult");
        if (total_result_it != result_it->value.MemberEnd() && total_result_it->value.IsString()) {
            (void)writer.String(total_result_it->value.GetString());
        } else {
            (void)writer.String("NOK");
        }
    } else {
        (void)writer.String("NOK");
    }

    (void)writer.Key("ReleaseNumber");
    (void)writer.String("1611900407");

    (void)writer.Key("SessionID");
    (void)writer.String("123");

    (void)writer.Key("PackageName");
    (void)writer.String(path_.c_str());

    LOG_DEBUG << "Writing conditions...";
    (void)writer.Key("ConditionChecks");
    (void)writer.StartObject();
    conditions_.Dump(writer);
    (void)writer.EndObject();

    if (result_it != doc.MemberEnd() && result_it->value.IsObject()) {
        auto stat_blocks_it = result_it->value.FindMember("StatBlocks");
        if (stat_blocks_it != result_it->value.MemberEnd() && stat_blocks_it->value.IsArray()) {
            (void)writer.Key("StatBlocks");
            stat_blocks_it->value.Accept(writer);
        } else {
            LOG_WARN << "No 'StatBlocks' found in result.";
        }
    } else {
        LOG_WARN << "No 'Result' found in result.";
    }

    (void)writer.EndObject();
    (void)writer.EndObject();

    fail_guard.Dismiss();
}

void Task::SendResult(ScriptOpc result) {
    if (!result_sent_ || type_ == Type::kPeriodic) {
        LOG_INFO << static_cast<int>(result) << ": " << path_.string();
        result_sent_ = true;
        context_.SendMessage(result, path_.string());
    } else {
        LOG_WARN << "Result already sent.";
    }
}

[[noreturn]] void Task::Child() const noexcept {
    if (prctl(PR_SET_PDEATHSIG, SIGTERM) < 0) {
        int ret = write(STDERR_FILENO, "<<<DiagTask>>> Failed to set pdeathsig.\n", 41);
        if(ret){

        }
    }
    
    if (chdir(path_.c_str()) < 0) {
        int ret = write(STDERR_FILENO, "<<<DiagTask>>> Failed to change directory.\n", 44);
        if(ret){

        }
    }
    
    auto command = context_.GetPythonCommand().c_str();
    int ret = write(STDOUT_FILENO, "<<<DiagTask>>> Executing...\n", 29);
    if(ret){

    }
    setenv("PYTHONPATH", sys_paths_.c_str(), 1);
    execlp(command, command, script_path_.c_str(), nullptr);
    int ret1 = write(STDERR_FILENO, "<<<DiagTask>>> Failed to exec python script.\n", 46);
    if(ret1){

    }
    _exit(PYTHON_EXCEPT_EXIT);
}

} // namespace midware::diag::script