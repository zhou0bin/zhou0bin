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

#include "common/diagnostic_proxy.h"

#include "log.h"

#include <fstream>
#include <csignal>

namespace asf
{
namespace diag
{
namespace common
{

DiagnosticProxy::DiagnosticProxy(const std::string& config)
    : config_(config)
    , exit_requested_(false)
    , shutdown_mutex_()
    , shutdown_condvar_()
{
}

void DiagnosticProxy::Main()
{
    LOG_ERROR << __FUNCTION__ << "in! ";

    sigset_t signals;
    (void)sigfillset(&signals);
    (void)pthread_sigmask(SIG_SETMASK, &signals, NULL);

    std::ifstream ifs(config_);
    LOG_ERROR << __FUNCTION__ << "Config good: " << ifs.good() << "Path: " << config_;

    // Setup the modules
    try {
        Initialize();
    } catch (std::exception &e) {
        LOG_FATAL << __FUNCTION__ << "Initialize() Catch exception: " << e.what();
        exit_requested_ = true;
    }

    // Run, if we weren't asked to quit in the meantime
    if (!exit_requested_.load(std::memory_order_relaxed)) {
        try {
            Run();
        } catch (std::exception &e) {
            LOG_FATAL << __FUNCTION__ << "Run() Catch exception: " << e.what();
            exit_requested_ = true;
        }
    } else {
        LOG_INFO << __FUNCTION__ << "Skipping Run()! ";
    }

    // Wait until a shutdown is signaled
    {
        std::unique_lock<std::mutex> locker_shutdown{shutdown_mutex_};
        while (!exit_requested_.load(std::memory_order_relaxed)) {
            shutdown_condvar_.wait(locker_shutdown);
            LOG_ERROR << __FUNCTION__ << "Causing SignalShutdown()! ";
        }
    }

    // Perform the shutdown
    Shutdown();

    LOG_ERROR << __FUNCTION__ << "out! ";
}

void DiagnosticProxy::SignalShutdown()
{
    {
        const std::unique_lock<std::mutex> locker_shutdown{shutdown_mutex_};
        exit_requested_.store(true, std::memory_order_relaxed);
    }
    shutdown_condvar_.notify_all();
}

} /* namespace common */
} /* namespace diag */
} /* namespace asf */
