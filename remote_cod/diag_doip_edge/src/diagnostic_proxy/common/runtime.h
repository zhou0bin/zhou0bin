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

#ifndef SOURCE_COMMON_RUNTIME_H_
#define SOURCE_COMMON_RUNTIME_H_

#include <atomic>
#include <exception>
#include <cstring>
#include <memory>
#include <mutex>
#include <thread>

#include "common/config.h"

// #include "ara/core/vector.h"
#include <vector>

namespace asf
{
namespace diag
{

/**
 *  \brief class Runtime is a singleton, which represents the running instance of the
 *  Dcm part of the diagnostics module.
 *  \details It basically provides methods for controlling the life cycle of the dcm
 *  part of diagnostics - that is: initializing, starting/running and stopping.
 */
class Runtime
{
protected:
    /**
     * \brief Flag to indicate that the Runtime object is initialized correctly.
     */
    bool initialized_{false};

    /**
     * \brief Flag to indicate whether this Application has been asked to terminate.
     */
    std::atomic_bool exit_requested_{false};

    /**
     * \brief Container to hold all Threads spawned by this object.
     */
    std::vector<std::thread> threads_{};

    /**
     * \brief A pair of pipes used by SignalHandler() to notify WaitSignal() of an incoming signal.
     */
    int selfpipe_[2]{};

public:
    explicit Runtime();
    virtual ~Runtime() noexcept;

    /**
     * \brief Performs one-time setup of the application.
     */
    virtual void Initialize() = 0;

    /**
     * \brief Executes the workload of the application.
     *
     * This method spawns threads for DEM::Main() and DCM::Main().
     */
    virtual void Run() = 0;

    /**
     * \brief Perform an orderly shutdown of the application.
     *
     * This method shuts down all communication and waits for all threads to terminate.
     */
    virtual void Shutdown() = 0;

    /**
     * \brief Return the Runtime instance.
     */
    static Runtime& GetInstance();

    /**
     * \brief Getter for the initialization status.
     */
    bool IsInitialized() const
    {
        return initialized_;
    }

    /**
     * \brief Blocks the application until it is explicitly asked to terminate.
     *
     * The blocking wait is implemented by a blocking wait on a pipe. HandleSignal() catches the SIGTERM signal and
     * converts it into a token sent via the pipe.
     */
    void WaitUntilTermination();

protected:
    /**
     * \brief Signal handler to catch signals sent to the process.
     *
     * This handler delegates to HandleSignal(int).
     */
    static void SignalHandler(const int signum);

    /**
     * \brief Pointer to the instance of the AdaptiveAutosarApplication.
     *
     * Used by SignalHandler(int) to delegate to HandleSignal(int). Note: instance_ is a guard against misconfiguring
     * the signal handler. It is not an implementation of the singleton pattern!
     */
    static std::unique_ptr<Runtime> instance_;

    /**
     * \brief Mutex used to protect concurrent write access to instance_.
     *
     * Note that concurrent read is possible without protection by the mutex. The rationale is that instance_ is
     * conceptually write-once, i.e., it will be written once by MakeInstance() and is read-only from then on out.
     */
    static std::mutex instance_mutex_;

    /**
     * \brief Signal handler to catch signals sent to the AdaptiveAutosarApplication.
     *
     * This signal handler only handles SIGTERM, which asks the application to terminate.
     * It forwards the notification that a SIGTERM occurred using selfpipe.
     */
    void HandleSignal(const int signum);

private:
    /*
     * Ensure that objects of this type are not copyable and not movable.
     */
    Runtime(Runtime&& other) = delete;
    Runtime(const Runtime& other) = delete;
    Runtime& operator=(Runtime&& other) = delete;
    Runtime& operator=(const Runtime& other) = delete;
};

} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_COMMON_RUNTIME_H_
