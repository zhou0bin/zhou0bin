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

#ifndef SOURCE_COMMON_DIAGNOSTIC_PROXY_H_
#define SOURCE_COMMON_DIAGNOSTIC_PROXY_H_

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace asf
{
namespace diag
{
namespace common
{

/**
 * \brief Base class for DEM and DCM, providing common lifecycle management methods.
 */
class DiagnosticProxy
{
protected:
    /**
     * \brief contains the configuration data.
     */
    std::string config_;

    /**
     * \brief Flag to indicate that this component was asked to terminate.
     */
    std::atomic_bool exit_requested_;

    /**
     * \brief Mutex used for shutdown_condvar_.
     */
    std::mutex shutdown_mutex_;

    /**
     * \brief Condition Variable to notify the DiagnosticProxy that a shutdown was requested.
     */
    std::condition_variable shutdown_condvar_;

public:
    /**
     * \brief Constructor of the DiagnosticProxy.
     */
    DiagnosticProxy(const std::string& config);

    explicit DiagnosticProxy(DiagnosticProxy&&) = delete;
    DiagnosticProxy(DiagnosticProxy&) = delete;
    DiagnosticProxy& operator=(DiagnosticProxy&) = delete;
    DiagnosticProxy& operator=(DiagnosticProxy&&) = delete;
    /**
     * \brief Destructor of the DiagnosticProxy.
     */
    virtual ~DiagnosticProxy() = default;

    /**
     * \brief Entry point of the DiagnosticProxy.
     *
     * Calls Initialize() and Run(), then waits until exit_requested_ == true && shutdown_condvar_ is signaled,
     * then calls Shutdown(). It is assumed that Main() will be used as the entry point of a new thread.
     */
    virtual void Main();

    /**
     * \brief Signal the DiagnosticProxy that a shutdown was requested.
     *
     * Signals shutdown_condvar_ and sets exit_requested_ == true
     *
     */
    virtual void SignalShutdown();

    /**
     * \brief Perform initialization of the DiagnostigProxy.
     */
    virtual void Initialize() = 0;

    /**
     * \brief Execute the workload of the DiagnostigProxy.
     */
    virtual void Run() = 0;

    /**
     * \brief Shut the DiagnosticProxy down.
     *
     * Ends all connections and waits for spawned threads to terminate.
     */
    virtual void Shutdown() = 0;
};

} /* namespace common */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_COMMON_DIAGNOSTIC_PROXY_H_
