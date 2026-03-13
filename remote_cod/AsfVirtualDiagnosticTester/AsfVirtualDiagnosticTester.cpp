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

// #include "GenData/Application_AsfVirtualDiagnosticTester.hpp"

/*************************************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!    << Begin of Modification is prohibited in this endifDHFArea >>          DO NOT CHANGE THIS COMMENT!
 ************************************************************************************************************************************************/
#endif
 /************************************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!    << Finish of Modification is prohibited in this endifDHFArea >>          DO NOT CHANGE THIS COMMENT!
 ************************************************************************************************************************************************/




// #include "ara/exec/execution_client.h"
// #include "ara/core/initialization.h"
#include <iostream>
#include <signal.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
/***********************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << Start of include and declaration area >>        DO NOT CHANGE THIS COMMENT!
 **********************************************************************************************************************************/
#include "midware/diag/virtual_diagnostics.h"
#ifdef HEALTH_RELIABILITY_CHAIN_OPTION
#include "ara/phm/supervised_entity.h"
#include "ara/phm/supervised_entities/AsfVirtualDiagnosticTester.h"
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
    ara::phm::SupervisedEntity<ara::phm::supervised_entities::AsfVirtualDiagnosticTester::Checkpoints> se1(ara::core::InstanceSpecifier("NeuSAR/AsfVirtualDiagnosticTester/AsfVirtualDiagnosticTester/RPort_PHM"));
    while ( Phm_report_flag )
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        se1.ReportCheckpoint(ara::phm::supervised_entities::AsfVirtualDiagnosticTester::Checkpoints::CP1);
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

    asf::vdi::GlobalStop();
/***********************************************************************************************************************************
 * [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of the code after catch signal >>           DO NOT CHANGE THIS COMMENT!
 **********************************************************************************************************************************/
}
int main(int argc, char* argv[])
{
    signal(SIGTERM, SignalHandler);
    OPENSSL_init_ssl(0, NULL);
    OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);

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
    pthread_setname_np(ReportHeartbeatToPhmThread.native_handle(), "ASF_DT_Phm");
#endif

    std::shared_ptr<asf::vdi::VirtualDiagnostics> virtual_diagnostics;
    try {
        virtual_diagnostics = std::make_shared<asf::vdi::VirtualDiagnostics>();
        virtual_diagnostics->Init();
    }
    catch(...) {}

    std::thread vt_thread(
    [&]()
    {
        try{
            while(true){
                if(!asf::vdi::GetGlobalStop()){
                    std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(2));
                }
                else{
                    virtual_diagnostics->Release();
                    break;
                }
            }
        }
        catch(...){}
    });
    vt_thread.join();

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

