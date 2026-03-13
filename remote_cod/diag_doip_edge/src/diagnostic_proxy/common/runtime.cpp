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

#include "runtime.h"

#include <unistd.h>
#include <csignal>
#include <exception>
#include <iostream>
#include <cstring>
#include <memory>
#include <thread>

namespace asf
{
namespace diag
{

std::mutex Runtime::instance_mutex_;
std::unique_ptr<Runtime> Runtime::instance_{nullptr};

Runtime::Runtime()
    : initialized_(false)
    , exit_requested_(false)
    , threads_()
    , selfpipe_{}
{
    if (-1 == pipe(selfpipe_)) {
        throw std::runtime_error(strerror(errno));
    }
}

Runtime::~Runtime()
{
    // Stop signal handling
    (void)signal(SIGTERM, SIG_DFL);

    try {
        (void)close(selfpipe_[0]);
        (void)close(selfpipe_[1]);
    } catch (...) {
    }
}

void Runtime::HandleSignal(const int signum)
{
    if (SIGTERM == signum)
    {
        exit_requested_.store(true, std::memory_order_relaxed);
        if (-1 == write(selfpipe_[1], "\0", 1))
        {
            throw std::runtime_error(strerror(errno));
        }
    }
}

void Runtime::SignalHandler(const int signum)
{
    Runtime::GetInstance().HandleSignal(signum);
}

void Runtime::WaitUntilTermination()
{
    while (!exit_requested_.load(std::memory_order_relaxed))
    {
        int buffer;
        // Blocking read on selfpipe_
        const ssize_t bytesRead(read(selfpipe_[0], &buffer, sizeof(int)));
        if (bytesRead > 0)
        {
            // selfpipe_ is only used to signal an exit request.
            exit_requested_.store(true, std::memory_order_relaxed);
        }
        else
        {
            throw std::runtime_error(strerror(errno));
        }
    }
}

Runtime& Runtime::GetInstance()
{
    return *instance_.get();
}

} /* namespace diag */
} /* namespace asf */
