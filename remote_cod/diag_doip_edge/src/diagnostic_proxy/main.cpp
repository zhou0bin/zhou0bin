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

/**
 * \file
 * \brief Main entry point of the diagnostic manager (that is dcm and dem functionality).
 */

#include "runtime_proxy.h"

#ifdef USE_ARA
#include "ara/exec/execution_client.h"
#ifdef NEUSAR_AP_R2111
#include "ara/core/initialization.h"
#endif
#endif

#ifndef WITHOUT_PHM
#include "ara/phm/supervised_entity.h"
#include "AsfDiagnosticProxy.h"
bool Phm_report_flag = false;

#ifdef NEUSAR_AP_R2111
void ReportHeartbeatToPhm()
{
    ara::phm::SupervisedEntity<ara::phm::supervised_entities::AsfDiagnosticProxy::Checkpoints> se1(ara::core::InstanceSpecifier("NeuSAR/AsfDiagnosticProxy/AsfDiagnosticProxy/RPort_PHM"));
    while ( Phm_report_flag )
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        se1.ReportCheckpoint(ara::phm::supervised_entities::AsfDiagnosticProxy::Checkpoints::CP1);
    }
}
#else
void ReportHeartbeatToPhm()
{
    ara::phm::SupervisedEntity se1(instanceId_SE_AsfDiagnosticProxy_AsfDiagnosticProxy_RPort_PHM);
    while ( Phm_report_flag )
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        se1.ReportCheckpoint(AsfDiagnosticProxy_CP1);
    }
}
#endif
#endif

/**
 * \brief Main entry point of the diagnostic Manager.
 *
 * This function forwards all operation to the lifecycle methods of asf::diag::Runtime.
 */
int main()
{
#ifdef USE_ARA
#ifdef NEUSAR_AP_R2111
    (void)ara::core::Initialize();
#endif
    const ara::exec::ExecutionClient exec_client;
    (void)exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);
#endif

#ifndef WITHOUT_PHM
    Phm_report_flag = true;
    std::thread ReportHeartbeatToPhmThread = std::thread(&ReportHeartbeatToPhm);
    (void)pthread_setname_np(ReportHeartbeatToPhmThread.native_handle(), "ASF_DP_Phm");
#endif

    asf::diag::RuntimeProxy& runtime(asf::diag::RuntimeProxy::GetInstance());
    runtime.Initialize();
    runtime.Run();
    runtime.WaitUntilTermination();
    runtime.Shutdown();

#ifndef WITHOUT_PHM
    Phm_report_flag = false;
    if (ReportHeartbeatToPhmThread.joinable()) {
        ReportHeartbeatToPhmThread.join();
    }
#endif

#ifdef USE_ARA
    (void)exec_client.ReportExecutionState(ara::exec::ExecutionState::kTerminating);
#ifdef NEUSAR_AP_R2111
    (void)ara::core::Deinitialize();
#endif
#endif

    return (0);
}
