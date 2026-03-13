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

#include "runtime_proxy.h"

// #include "ara/com/internal/vsomeip/vsomeip_connection.h"

namespace asf
{
namespace diag
{

void RuntimeProxy::MakeInstance()
{
    const std::unique_lock<std::mutex> locker_instance(instance_mutex_);
    if (instance_ != nullptr) {
        // Another thread was quicker, Instance already exists.
        return;
    }
    // create a new instance
    RuntimeProxy::instance_ = std::make_unique<RuntimeProxy>(PROXY_CONFIG_PATH);

    // Verify whether signal handler was registered successfully. Otherwise, abort
    // the application with an error.
    if (SIG_ERR == signal(SIGTERM, Runtime::SignalHandler)) {
        throw std::runtime_error(strerror(errno));
    }
}

RuntimeProxy& RuntimeProxy::GetInstance()
{
    if (instance_ == nullptr) {
        RuntimeProxy::MakeInstance();
    }
    return *dynamic_cast<RuntimeProxy*>(instance_.get());
}

void RuntimeProxy::Initialize()
{
    if (!initialized_) {
        (void)signal(SIGTERM, SignalHandler);
        initialized_ = true;
    }
}

void RuntimeProxy::Run()
{
    // Start Threads for Proxy
    threads_.reserve(kDcmThreadCount);
    (void)threads_.emplace_back(&asf::diag::dcm::DCM::Main, &dcm_);
    (void)pthread_setname_np(threads_[0].native_handle(), "ASF_DIAG_PROXY_DCM");
}

void RuntimeProxy::Shutdown()
{
    dcm_.SignalShutdown();

    for (std::thread& thread : threads_) {
        thread.join();
    }
}

}  // namespace diag
}  // namespace asf
