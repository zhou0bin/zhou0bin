/*****************************************************************************
* @file     main.cpp
* @brief    虚拟诊断仪主程序
* @date     2025/04/21
*****************************************************************************/
#if 0
#include <signal.h>

#include <mutex>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <atomic>
#include <memory>

#include "ara/exec/execution_client.h"
#include "ara/phm/com/utils/instance_specifier.h"
#include "ara/phm/supervised_entity.h"
#include "ara/phm/common.h"

#include "virtual_diag/virtual_diagnostics.h"

#define RUNNING 1
bool isStopSig = false;

void SignalHandler(int signum)
{
    global_stop_flag = true;
}

int main(int argc, char* argv[])
{
    std::chrono::time_point<std::chrono::steady_clock,std::chrono::milliseconds> start =
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
    long start_time_stamp = start.time_since_epoch().count();

    signal(SIGTERM, SignalHandler);
    
    ara::exec::ExecutionClient exec_client;

    ara::log::Logger& loggerInstance = ara::log::CreateLogger("APER", "the per application debug log.", ara::log::LogLevel::kDebug);
    extern ara::core::Result<void> InitDBByJsonGeneral(::ara::log::Logger& logger);
    auto initDbResult = InitDBByJsonGeneral(loggerInstance);
    if (!initDbResult.HasValue()) {
        loggerInstance.LogError() << "InitDBByJsonGeneral error:" << initDbResult.Error().Message();
        return -1;
    }

    auto result = ara::per::UpdatePersistency();
    if (!result)
    {
        loggerInstance.LogError() << "UpdatePersistency Error!";
        return -1;
    }
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);
    std::shared_ptr<asf::vdi::VirtualDiagnostics> virtual_diagnostics;
    try {
        virtual_diagnostics = std::make_shared<asf::vdi::VirtualDiagnostics>();
        virtual_diagnostics->Init();
    }
    catch(...) {
        LOGERROR << "VirtualDiagnostics init error.";
    }
    std::chrono::time_point<std::chrono::steady_clock,std::chrono::milliseconds> end =
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
    long end_time_stamp =  end.time_since_epoch().count();
    long spend_time = end_time_stamp - start_time_stamp;

    LOGINFO << "VirtualDiagnostics init finish time stamp: " << end_time_stamp;
    LOGINFO << "VirtualDiagnostics initialization spend time: " << spend_time;

    //*****************************************************PHM********************************************************************
    using namespace ara::phm::com::utils;

    std::thread phm_thread(
    [&]()
    {
        try{
            // ara::phm::SupervisedEntity Instance(instanceId_SE_AsfVirtualDiagnosticTester_AsfVirtualDiagnosticTester_RPort_PHM);
            while(RUNNING){
                if(!global_stop_flag){
                    // Instance.ReportCheckpoint(AsfVirtualDiagnosticTester_CP1);
                    std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(2));
                }
                else{
                    virtual_diagnostics->Release();
                    break;
                }
            }
        }
        catch(...){
            LOGERROR << "phm thread error.";
        }
    });
    phm_thread.join();
    // report application state
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kTerminating);
    return 0;
}
#endif