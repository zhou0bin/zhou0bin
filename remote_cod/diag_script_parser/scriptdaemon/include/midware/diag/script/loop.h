//
//
#ifndef __DIAG_SCRIPT_LOOP_H__
#define __DIAG_SCRIPT_LOOP_H__

#include <set>
#include <map>
#include <deque>
#include <mutex>
#include <chrono>
#include <memory>
#include <functional>

#include <sys/select.h>
#include <sys/timerfd.h>
#include <sys/signalfd.h>

#include <midware/diag/script/common.h>
#include <midware/diag/script/async.h>

namespace midware::diag::script {

///////////////////////////////////////////////////////////////////////////////
// Poller
///////////////////////////////////////////////////////////////////////////////

class Poller {
public:
    enum {
        kEventReadable = 0x1,
        kEventWritable = 0x2,
        kEventException = 0x4
    };

    using Task = std::function<void ()>;
    using Callback = std::function<void (int)>;

public:
    Poller();
    virtual ~Poller();

public:
    void AddFd(int fd, int events, Callback cb);
    void RemoveFd(int fd);
    void Run();
    void Stop() const noexcept;
    Future<void> Schedule(Task&& task);

private:
    int Prepare();
    void Dispatch();
    void Wakeup();
    void RunTaskQueue();

private:
    struct FdInfo {
        int events;
        Callback cb;
    };

    using TaskQueue = std::deque<std::pair<Task, Promise<void>>>;
    static constexpr size_t kTaskQueueMax = 1024;

    bool run_;
    int cmd_pipe_[2];
    std::map<int, FdInfo> fds_;
    std::map<int, FdInfo> added_fds_;
    std::set<int> removed_fds_;
    fd_set fds_r_;
    fd_set fds_w_;
    fd_set fds_e_;
    TaskQueue task_queue_;
    std::mutex task_queue_mutex_;
};

///////////////////////////////////////////////////////////////////////////////
// Timer
///////////////////////////////////////////////////////////////////////////////

class Timer {
public:
    using Clock = std::chrono::steady_clock;
    using Callback = std::function<void ()>;

public:
    Timer(Poller& poller);
    virtual ~Timer();

public:
    void Start(Clock::duration timeout, Clock::duration repeat, Callback cb);
    void Stop();

private:
    Poller& poller_;
    int timerfd_;
};

///////////////////////////////////////////////////////////////////////////////
// Signal
///////////////////////////////////////////////////////////////////////////////

class Signal {
public:
    using Callback = std::function<void (int)>;

public:
    Signal(Poller& poller);
    virtual ~Signal();

public:
    void Start(uint32_t signo, Callback cb);
    void Start(const sigset_t& sigset, Callback cb);
    void Stop();
    static void BlockAll() noexcept;
    static void UnblockAll() noexcept;

private:
    class Context final {
    public:
        Context(Poller& selector);
        ~Context();

    public:
        Poller& GetPoller() {
            return poller_;
        }

        void Add(Signal* p);
        void Remove(Signal* p);

    private:
        void Update();

    private:
        Poller& poller_;
        int signalfd_;
        std::set<Signal*> signals_;
    };

    static std::shared_ptr<Context> GetContext(Poller& poller);

private:
    Poller& poller_;
    std::shared_ptr<Context> context_;
    sigset_t sigset_;
    Callback cb_;
    static std::weak_ptr<Context> weak_context_;
};

} // namespace midware::diag::script

#endif // __DIAG_SCRIPT_LOOP_H__
