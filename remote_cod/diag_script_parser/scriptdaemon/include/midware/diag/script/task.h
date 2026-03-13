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

#ifndef __SF_DIAG_SCRIPT_TASK_H__
#define __SF_DIAG_SCRIPT_TASK_H__

#include <deque>
#include <filesystem>

#include <midware/diag/script/common.h>
#include <midware/diag/script/context.h>
#include <midware/diag/script/condition.h>

namespace midware::diag::script {

class Task {
public:
    using Clock = std::chrono::steady_clock;
    
    enum class Type {
        kInvalid = 0,
        kTemp,
        kPeriodic,
        kEvent
    };

public:
    Task(Context& context, const std::filesystem::path& path);
    Task(const Task& ) = delete;
    Task(Task&& ) = delete;
    virtual ~Task();

public:
    Task& operator=(const Task& ) = delete;
    Task& operator=(Task&& ) = delete;

    const std::filesystem::path& GetPath() const noexcept {
        return path_;
    }

    Type GetType() const noexcept {
        return type_;
    }

    bool IsRunning() const noexcept {
        return (pid_ > 0);
    }

    void Start();
    void Stop();

private:
    void SetStopped(ScriptOpc opc = ScriptOpc::kStopSuccess);
    void OnUserAuthAccepted();
    void StartProcess();
    void HandleForked(int pid);
    void HandleExited(int status);
    void BuildResultFile();
    void WriteResult();
    void SendResult(ScriptOpc result);

    [[noreturn]] void Child() const noexcept;

private:
    Context& context_;
    std::string sys_paths_;
    std::filesystem::path path_;
    std::filesystem::path script_path_;
    Type type_;
    bool started_;
    int priority_;
    Clock::duration periodic_;
    Timer periodic_timer_;
    ConditionSet conditions_;
    Future<bool> async_user_auth_;
    Future<void> async_wait_conditions_;
    Future<int> async_fork_;
    Future<int> async_waitpid_;
    int pid_{-1};
    ScriptOpc result_;
    bool result_sent_;
};

} // namespace namespace midware::diag::script

#endif //__SF_DIAG_SCRIPT_TASK_H__
