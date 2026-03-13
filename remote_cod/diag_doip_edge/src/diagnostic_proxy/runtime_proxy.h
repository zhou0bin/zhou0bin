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

#ifndef SOURCE_RUNTIME_PROXY_H_
#define SOURCE_RUNTIME_PROXY_H_

#include <unistd.h>
#include <csignal>
#include <exception>
#include <iostream>
#include <cstring>
#include <memory>
#include <thread>

#include "common/runtime.h"

#include "dcm/diagnostic_communication_manager.h"

namespace asf
{
namespace diag
{


/**
 * \brief definition of count of DCM threads
 */
static const uint8_t kDcmThreadCount = 1;

/**
 * \brief class which provides the basic runtime environment for dcm and dem.
 */
class RuntimeProxy : public diag::Runtime
{
public:
    /**
     * \brief Constructor of RuntimeProxy.
     *
     * \param config The config object used by DCM and DEM.
     */
    explicit RuntimeProxy(const std::string& config)
        : dcm_(config)
    {
    }
    ~RuntimeProxy() override
    {
    }

    /**
     * Delete default copy constructor: References between DCM and DEM would be erroneous.
     */
    RuntimeProxy(const RuntimeProxy&) = delete;
    explicit RuntimeProxy(RuntimeProxy&&) = delete;
    RuntimeProxy& operator=(const RuntimeProxy& other) = delete;
    RuntimeProxy& operator=(const RuntimeProxy&& other) = delete;

    /**
     * \brief Creates an instance of the Runtime.
     *
     * Fills the instance_ pointer and installs a signal handler. This method is threadsafe. It is guaranteed that at
     * most one Runtime instance is created, no matter how may times this Function is called in parallel.
     *
     * If instance_ != nullptr, this method does nothing.
     */
    static void MakeInstance();

    /**
     * \brief Return the Runtime instance.
     */
    static RuntimeProxy& GetInstance();

    /**
     * read in configuration information and initialize runtime from it.
     */
    void Initialize() override ;

    /**
     * Start services. After this method has been finished successfully, dcm is:
     * - able to serve UDS diag request comming in from protocol adapters.
     * - able to handle handle diag requests initiated from local clients.
     */
    void Run() override;

    /**
     * stops service handling (incoming and outgoing UDS requests). Method returns
     * as soon as services have been stopped.
     */
    void Shutdown() override;

    /**
     * Returns the Interface to DCM instance
     */
    dcm::DCM& GetDCMInterface()
    {
        return dcm_;
    }

protected:
    /**
     * \brief The Diagnostic Communication Manager.
     */
    asf::diag::dcm::DCM dcm_;
};

}  // namespace diag
}  // namespace asf

#endif  // SOURCE_RUNTIME_PROXY_H_
