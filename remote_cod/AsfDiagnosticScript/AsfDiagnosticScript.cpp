/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *            This software is copyright protected and proprietary to Neusoft Reach.
 *            Neusoft Reach grants to you only those rights as set out in the license conditions.
 *            All other rights remain with Neusoft Reach.
 *  -------------------------------------------------------------------------------------------------------------------
 *********************************************************************************************************************/

/***********************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!    << Start of defaultHeaderFile reference define >>        DO NOT CHANGE THIS COMMENT!
 **********************************************************************************************************************************/
#if 1
 /**********************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!    << End of defaultHeaderFile reference define >>          DO NOT CHANGE THIS COMMENT!
 **********************************************************************************************************************************/

// #include "GenData/Application_AsfDiagnosticScript.hpp"

/*************************************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!    << Begin of Modification is prohibited in this endifDHFArea >>          DO NOT CHANGE THIS COMMENT!
 ************************************************************************************************************************************************/
#endif
 /************************************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!    << Finish of Modification is prohibited in this endifDHFArea >>          DO NOT CHANGE THIS COMMENT!
 ************************************************************************************************************************************************/




// #include "ara/exec/execution_client.h"
// #include "ara/core/initialization.h"
#include <signal.h>
/***********************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << Start of include and declaration area >>        DO NOT CHANGE THIS COMMENT!
 **********************************************************************************************************************************/
#include <midware/diag/script/log.h>
#include <midware/diag/script/scopeguard.h>
#include <midware/diag/script/daemon.h>

#ifdef HEALTH_RELIABILITY_CHAIN_OPTION
#include "ara/phm/supervised_entity.h"
#include "ara/phm/supervised_entities/AsfDiagnosticScript.h"
bool Phm_report_flag = false;
#endif
 /**********************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of include and declaration area >>          DO NOT CHANGE THIS COMMENT!
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << Start of function implementation >>             DO NOT CHANGE THIS COMMENT!
 **********************************************************************************************************************************/

#ifdef HEALTH_RELIABILITY_CHAIN_OPTION
void ReportHeartbeatToPhm()
{
    ara::phm::SupervisedEntity<ara::phm::supervised_entities::AsfDiagnosticScript::Checkpoints> se1(ara::core::InstanceSpecifier("NeuSAR/AsfDiagnosticScript/AsfDiagnosticScript/RPort_PHM"));
    while ( Phm_report_flag )
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        se1.ReportCheckpoint(ara::phm::supervised_entities::AsfDiagnosticScript::Checkpoints::CP1);
    }
}
#endif
/***********************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of function implementation >>               DO NOT CHANGE THIS COMMENT!
 **********************************************************************************************************************************/

void SignalHandler(int signum)
{
    //DO something after catch signal SIGTERM
/***********************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << Start of the code after catch signal >>         DO NOT CHANGE THIS COMMENT!
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of the code after catch signal >>           DO NOT CHANGE THIS COMMENT!
 **********************************************************************************************************************************/
}
int main(int argc, char* argv[])
{
    // // application init
    // ara::core::Result<void> init_result = ara::core::Initialize();
    // if (!init_result.HasValue()) {
    //     // initialize failed! some init handler throw exception!
    // }


    
/***********************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << Start of code implementation >>                 DO NOT CHANGE THIS COMMENT!
 **********************************************************************************************************************************/

    // // report application state
    // ara::exec::ExecutionClient exec_client;
    // (void)exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

#ifdef HEALTH_RELIABILITY_CHAIN_OPTION
    Phm_report_flag = true;
    std::thread ReportHeartbeatToPhmThread = std::thread(&ReportHeartbeatToPhm);
    pthread_setname_np(ReportHeartbeatToPhmThread.native_handle(), "ASF_DS_Phm");
#endif

    try {
        midware::diag::script::Signal::BlockAll();

        LOG_INFO << "Initializing...";

        midware::diag::script::Poller poller;

        midware::diag::script::Signal stopsigs(poller);
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGINT);
        sigaddset(&sigset, SIGTERM);

        stopsigs.Start(sigset, [&poller](int sig) {
            LOG_INFO << "Stopping by signal: " << sig;
            poller.Stop();
        });

        LOG_INFO << "Creating daemon...";
        midware::diag::script::Daemon daemon(poller);
        daemon.Init();

        LOG_INFO << "Running...";
        poller.Run();
    } catch (const std::exception& e) {
        LOG_ERROR << "App exception: " << e.what();
    } catch (...) {
        LOG_ERROR << "App exception: unknown";
    }

    LOG_INFO << "Exiting AsfDiagnosticScript Daemon...";

#ifdef HEALTH_RELIABILITY_CHAIN_OPTION
    Phm_report_flag = false;
    if (ReportHeartbeatToPhmThread.joinable())
    {
        ReportHeartbeatToPhmThread.join();
    }
#endif

    // // report application state
    // (void)exec_client.ReportExecutionState(ara::exec::ExecutionState::kTerminating);

/***********************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of code implementation >>                   DO NOT CHANGE THIS COMMENT!
 **********************************************************************************************************************************/

    // // application deinit
    // ara::core::Result<void> deinit_result = ara::core::Deinitialize();
    // if (!deinit_result.HasValue()) {
    //     // deinitialize failed! some deinit handler throw exception!
    // }

    return 0;
}

