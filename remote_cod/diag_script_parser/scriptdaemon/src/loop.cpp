//
//
#include <cassert>
#include <system_error>

#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <midware/diag/script/log.h>
#include <midware/diag/script/loop.h>

namespace midware::diag::script {

///////////////////////////////////////////////////////////////////////////////
// Poller
///////////////////////////////////////////////////////////////////////////////

Poller::Poller() {
    int ret = pipe(cmd_pipe_);
    if (ret < 0) {
        throw std::system_error(errno, std::system_category());
    }
    LOG_INFO << "cmd_pipe: " << cmd_pipe_[0] << cmd_pipe_[1];
}

Poller::~Poller() {
    close(cmd_pipe_[0]);
    close(cmd_pipe_[1]);
}

void Poller::AddFd(int fd, int events, Callback cb) {
    assert(fd >= 0);
    assert(cb);
    added_fds_[fd] = { events, std::move(cb) };
}

void Poller::RemoveFd(int fd) {
    assert(fd >= 0);
    removed_fds_.insert(fd);
}

void Poller::Run() {
    run_ = true;
    while (run_) {
        RunTaskQueue();
        int max_fd = Prepare();
        int ret = select(max_fd + 1, &fds_r_, &fds_w_, &fds_e_, nullptr);
        if (ret < 0) {
            if (errno != EINTR) {
                throw std::system_error(errno, std::system_category());
            }
        } else if (ret > 0) {
            Dispatch();
        } else {
            // timeout, continue
        }
    }
}

void Poller::Stop() const noexcept {
    int fd = cmd_pipe_[1];
    if (fd >= 0) {
        ssize_t write_size = write(fd, "x", 1);
        if(write_size == -1){
            LOG_ERROR << "Poller::Stop write cmd_pipe failed";
        }
    }
}

Future<void> Poller::Schedule(Task&& task) {
    Promise<void> promise;
    auto future = promise.GetFuture();
    if (task_queue_.size() < kTaskQueueMax) {
        std::unique_lock<std::mutex> lock(task_queue_mutex_);
        task_queue_.emplace_back(std::move(task), std::move(promise));
    } else {
        LOG_ERROR << "Task queue is full.";
        promise.Set(std::make_exception_ptr(std::runtime_error("Task queue is full.")));
    }
    return future;
}

int Poller::Prepare() {
    for (auto it = removed_fds_.begin(); it != removed_fds_.end(); ++it) {
        fds_.erase(*it);
    }
    removed_fds_.clear();

    for (auto it = added_fds_.begin(); it != added_fds_.end(); ++it) {
        fds_[it->first] = std::move(it->second);
    }
    added_fds_.clear();

    int max_fd = -1;

    FD_ZERO(&fds_r_);
    FD_ZERO(&fds_w_);
    FD_ZERO(&fds_e_);

    FD_SET(cmd_pipe_[0], &fds_r_);
    if (max_fd < cmd_pipe_[0]) {
        max_fd = cmd_pipe_[0];
    }

    for (auto& it: fds_) {
        //LOG_DEBUG << "fd " << it.first;
        if (it.second.events & kEventReadable) {
            FD_SET(it.first, &fds_r_);
        }
        if (it.second.events & kEventWritable) {
            FD_SET(it.first, &fds_w_);
        }
        FD_SET(it.first, &fds_e_);
        if (max_fd < it.first) {
            max_fd = it.first;
        }
    }

    return max_fd;
}

void Poller::Dispatch() {
    if (FD_ISSET(cmd_pipe_[0], &fds_r_)) {
        char c = 0;
        run_ = (read(cmd_pipe_[0], &c, 1) == 1) && (c != 'x');
    }

    for (auto& it: fds_) {
        int events = 0;
        if ((it.second.events & kEventReadable) && FD_ISSET(it.first, &fds_r_)) {
            events |= kEventReadable;
        }
        if ((it.second.events & kEventWritable) && FD_ISSET(it.first, &fds_w_)) {
            events |= kEventWritable;
        }
        if (FD_ISSET(it.first, &fds_e_)) {
            events |= kEventException;
        }
        if (events != 0) {
            LOG_DEBUG << "fd: " << it.first << ", events: " << events;
            it.second.cb(events);
        }
    }
}

void Poller::Wakeup() {
    ssize_t write_size = write(cmd_pipe_[1], "w", 1);
    if(write_size == -1){
        LOG_ERROR << "Poller::Wakeup write cmd_pipe failed";
    }
}

void Poller::RunTaskQueue() {
    while (true) {
        std::pair<Task, Promise<void>> task_info;
        {
            std::unique_lock<std::mutex> lock(task_queue_mutex_);
            if (!task_queue_.empty()) {
                task_info = std::move(task_queue_.front());
                task_queue_.pop_front();
            } else {
                break;
            }
        }
        try {
            task_info.first();
            task_info.second.Set();
        } catch (...) {
            task_info.second.Set(std::current_exception());
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Timer
///////////////////////////////////////////////////////////////////////////////

Timer::Timer(Poller& poller)
    : poller_(poller)
    , timerfd_(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)) {
    if (timerfd_ < 0) {
        throw std::system_error(errno, std::system_category());
    }
    LOG_DEBUG << "timerfd: " << timerfd_;
}

Timer::~Timer() {
    Stop();
    close(timerfd_);
    timerfd_ = -1;
}

void Timer::Start(Clock::duration timeout, Clock::duration repeat, Callback cb) {
    assert(cb);

    struct itimerspec its;
    memset(&its, 0, sizeof(its));

    auto timeout_ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
    if (timeout_ms > 0) {
        its.it_value.tv_sec  = timeout_ms / 1000;
        its.it_value.tv_nsec = (timeout_ms % 1000) * 1000000;
    } else {
        its.it_value.tv_nsec = 1;
    }
    
    if (repeat != Clock::duration::min()) {
        auto repeat_ms = std::chrono::duration_cast<std::chrono::milliseconds>(repeat).count();
        its.it_interval.tv_sec  = repeat_ms / 1000;
        its.it_interval.tv_nsec = (repeat_ms % 1000) * 1000000;
    }

    LOG_DEBUG << "timeout " << its.it_value.tv_sec << "."  << its.it_value.tv_nsec;
    LOG_DEBUG << "repest " << its.it_interval.tv_sec << "."  << its.it_interval.tv_nsec;

    int ret = timerfd_settime(timerfd_, 0, &its, nullptr);
    if (ret < 0) {
        throw std::system_error(errno, std::system_category());
    }

    poller_.AddFd(timerfd_, Poller::kEventReadable, [this, user_cb = std::move(cb)](int events) {
        //LOG_DEBUG << __func__;
        if (events & Poller::kEventReadable) {
            uint64_t exp;
            ssize_t read_size = read(timerfd_, &exp, sizeof(exp));
            if (read_size > 0) {
                user_cb();
            }
        }
    });
}

void Timer::Stop() {
    if (timerfd_ >= 0) {
        struct itimerspec its;
        memset(&its, 0, sizeof(its));
        (void)timerfd_settime(timerfd_, 0, &its, nullptr);
        poller_.RemoveFd(timerfd_);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Signal::Context
///////////////////////////////////////////////////////////////////////////////

Signal::Context::Context(Poller& poller)
    : poller_(poller)
    , signalfd_(-1) {
}

Signal::Context::~Context() {
    if (signalfd_ >= 0) {
        close(signalfd_);
    }
}

void Signal::Context::Add(Signal* p) {
    signals_.insert(p);
    Update();
}

void Signal::Context::Remove(Signal* p) {
    auto it = signals_.find(p);
    if (it != signals_.end()) {
        signals_.erase(it);
        Update();
    }
}

void Signal::Context::Update() {
    sigset_t sigset;
    sigemptyset(&sigset);

    for (auto& it : signals_) {
        for (int sig = 1; sig < NSIG; ++sig) {
            if (sigismember(&it->sigset_, sig)) {
                sigaddset(&sigset, sig);
            }
        }
    }

    if (signalfd_ >= 0 && sigisemptyset(&sigset)) {
        poller_.RemoveFd(signalfd_);
        close(signalfd_);
        signalfd_ = -1;
        return;
    }

    signalfd_ = signalfd(signalfd_, &sigset, SFD_NONBLOCK);
    if (signalfd_ < 0) {
        return;
    }

    LOG_DEBUG << "singalfd: " << signalfd_;
    
    poller_.AddFd(signalfd_, Poller::kEventReadable, [this](int events) {
        if (events & Poller::kEventReadable) {
            struct signalfd_siginfo si;
            auto ret = read(signalfd_, &si, sizeof(si));
            if (ret > 0) {
                for (auto& it : signals_) {
                    if (sigismember(&it->sigset_, si.ssi_signo)) {
                        it->cb_(si.ssi_signo);
                    }
                }
            }
        }
    });
}

///////////////////////////////////////////////////////////////////////////////
// Signal
///////////////////////////////////////////////////////////////////////////////

std::weak_ptr<Signal::Context> Signal::weak_context_;

Signal::Signal(Poller& poller)
    : poller_(poller)
    , context_(GetContext(poller)) {
}

Signal::~Signal() {
    Stop();
}

void Signal::Start(uint32_t signo, Callback cb) {
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, signo);
    Start(sigset, std::move(cb));
}

void Signal::Start(const sigset_t& sigset, Callback cb) {
    assert(!sigisemptyset(&sigset));
    assert(cb);
    sigset_ = sigset;
    cb_ = std::move(cb);
    context_->Add(this);
}

void Signal::Stop() {
    context_->Remove(this);
}

void Signal::BlockAll() noexcept {
    sigset_t allsigs;
    sigfillset(&allsigs);
    (void)sigprocmask(SIG_BLOCK, &allsigs, nullptr);
    (void)pthread_sigmask(SIG_BLOCK, &allsigs, nullptr);
}

void Signal::UnblockAll() noexcept {
    sigset_t allsigs;
    sigfillset(&allsigs);
    (void)sigprocmask(SIG_UNBLOCK, &allsigs, nullptr);
    (void)pthread_sigmask(SIG_UNBLOCK, &allsigs, nullptr);
}

std::shared_ptr<Signal::Context> Signal::GetContext(Poller& poller) {
    auto context = weak_context_.lock();
    if (context) {
        assert(&poller == &context->GetPoller());
    } else {
        context = std::make_shared<Context>(poller);
        weak_context_ = context;
    }
    return context;
}

} // namespace midware::diag::script
