/*****************************************************************************
* @file     macro.h
* @brief    
* @author   kevin
* @date     2025/4/21
*****************************************************************************/

#ifndef VD_MACRO_H_
#define VD_MACRO_H_

#ifdef SECURE_STARTUP

#define USING_MODEL

#endif

#ifdef USING_MODEL
#define DATA_DSCR_FLODER "/data/DiagnosticScript/"
#define BIG_DATA_FLODER "/data/DiagnosticScript/BackupResult/"
#define DSCR_FLODER "/data/DiagnosticScript/"
#define ODX_FLODER "/data/DiagnosticScript/Odx/"
#define SCRIPT_FLODER "/data/DiagnosticScript/scriptFile/"
#define TEMP_SCRIPT_FOLDER "/data/DiagnosticScript/scriptFile/temporary/"
#define LOCAL_SCRIPT_FILE_FOLDER "/data/DiagnosticScript/scriptFile/fixed/"
#define SA_MAP_FILE_PATH "/opt/DiagnosticScript/scriptFile/fixed/Part-SA.json"
#define TIME_PARAMETER_FOLDER "/data/DiagnosticScript/engineTimeParameter/"
#define CONDITION_CHECK_FLODER "/data/DiagnosticScript/conditioncheck/"
#define CONDITION_CHECK_FILE_PATH "/data/DiagnosticScript/conditioncheck/condition_check.json"
#define VEHICLE_DATA_FILE_FLODER "/data/DiagnosticScript/vehicleData/"
#define VEHICLE_DATA_FILE_PATH "/data/DiagnosticScript/vehicleData/vehicle_data.json"
#define SCRIPT_PARAMETER_FILE_PATH "/data/DiagnosticScript/vehicleData/script_parameter.json"
#define VEHICLE_CONFIG_FOLDER "/data/DiagnosticScript/vehicleConfig/"
#define VEHICLE_VERSION_FOLDER "/data/DiagnosticScript/VehicleVersion/"
//#define SOCKET_CONFIG_FILE_PATH "/opt/DiagnosticScript/libs/util/DiagParsingConfig.json"
#define SOCKET_CONFIG_FILE_PATH "/opt/DiagnosticScript/libs/util/SecureDiagParsingConfig.json"
#define AES_CMAC_KEY_PATH "/opt/DiagnosticScript/libs/util/AES_CMAC_KEY.json"
#define DIAG_RESULT_FILE_FLODER "/data/DiagnosticScript/result/"
#define DIAG_RESULT_JSON_FILE_PATH "/data/DiagnosticScript/result/DiagResult.json"
#define DIAG_RESULT_FILE_PATH "/data/DiagnosticScript/result/DiagResult.tar.gz"
#define VEHICLE_DATA_FILE_BACKUP_FOLDER "/containers/Container06/work/common/opt/AsfVirtualDiagnosticTester/shared/VehicleData/"
#define VEHICLE_DATA_FILE_BACKUP_PATH "/containers/Container06/work/common/opt/AsfVirtualDiagnosticTester/shared/VehicleData/vehicle_data.json"
#define PUBLIC_KEY_SHA256_PATH "/containers/Container06/work/common/opt/AsfVirtualDiagnosticTester/shared/PublickeySha256.txt"
#define KEY_FOLDER "/containers/Container06/work/common/opt/AsfVirtualDiagnosticTester/shared/key/"
#define PUBLIC_KEY_PATH "/containers/Container06/work/common/opt/AsfVirtualDiagnosticTester/shared/key/Publickey.txt"
#define TEMP_STORAGE_FOLDER "/containers/Container06/work/common/opt/AsfVirtualDiagnosticTester/shared/temp/"
#define TEMP_SCRIPT_STORAGE_FOLDER "/containers/Container06/work/common/opt/AsfVirtualDiagnosticTester/temp/Script/"
#define OTA_CONFIG_FILES_FLODER "/ota/VTAP/"
// #define LIB_SA_PATH "/containers/Container06/rootfs/usr/lib/libSA.so"

#else

// #define DATA_DSCR_FLODER "/data/DiagnosticScript/"
// #define BIG_DATA_FLODER "/data/DiagnosticScript/BackupResult/"
#define OTA_CONFIG_FILES_FLODER "/ota/VTAP/"
// #define LIB_SA_PATH "/containers/NeuSARPlatform/rootfs/usr/lib/libSA.so"

#define DATA_DSCR_FLODER asf::vdi::macro::GetFilePatch("/data/DiagnosticScript/").c_str()
#define BIG_DATA_FLODER asf::vdi::macro::GetFilePatch("/data/DiagnosticScript/BackupResult/").c_str()
#define DSCR_FLODER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/").c_str()
#define ODX_FLODER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/Odx/").c_str()
#define SCRIPT_FLODER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/scriptFile/").c_str()
#define TEMP_SCRIPT_FOLDER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/scriptFile/temporary/").c_str()
#define EVENT_SCRIPT_FOLDER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/scriptFile/event/").c_str()
#define PERIOD_SCRIPT_FOLDER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/scriptFile/period/").c_str()
#define LOCAL_SCRIPT_FILE_FOLDER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/scriptFile/fixed/").c_str()
#define SA_MAP_FILE_PATH asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/scriptFile/fixed/Part-SA.json").c_str()
#define TIME_PARAMETER_FOLDER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/engineTimeParameter/").c_str()
#define CONDITION_CHECK_FLODER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/conditioncheck/").c_str()
#define CONDITION_CHECK_FILE_PATH asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/conditioncheck/condition_check.json").c_str()
#define VEHICLE_DATA_FILE_FLODER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/vehicleData/").c_str()
#define VEHICLE_DATA_FILE_PATH asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/vehicleData/vehicle_data.json").c_str()
#define SCRIPT_PARAMETER_FILE_PATH asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/vehicleData/script_parameter.json").c_str()
#define VEHICLE_CONFIG_FOLDER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/vehicleConfig/").c_str()
#define VEHICLE_VERSION_FOLDER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/VehicleVersion/").c_str()
#define SOCKET_CONFIG_FILE_PATH asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/libs/util/DiagParsingConfig.json").c_str()
#define AES_CMAC_KEY_PATH asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/libs/util/AES_CMAC_KEY.json").c_str()
// #define DIAG_RESULT_FILE_FLODER asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/result/").c_str()
// #define DIAG_RESULT_JSON_FILE_PATH asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/result/DiagResult.json").c_str()
// #define DIAG_RESULT_FILE_PATH asf::vdi::macro::GetFilePatch("/opt/DiagnosticScript/result/DiagResult.tar.gz").c_str()
#define VEHICLE_DATA_FILE_BACKUP_FOLDER asf::vdi::macro::GetFilePatch("/opt/AsfVirtualDiagnosticTester/shared/VehicleData/").c_str()
#define VEHICLE_DATA_FILE_BACKUP_PATH asf::vdi::macro::GetFilePatch("/opt/AsfVirtualDiagnosticTester/shared/VehicleData/vehicle_data.json").c_str()
#define PUBLIC_KEY_SHA256_PATH asf::vdi::macro::GetFilePatch("/opt/AsfVirtualDiagnosticTester/shared/PublickeySha256.txt").c_str()
#define KEY_FOLDER asf::vdi::macro::GetFilePatch("/opt/AsfVirtualDiagnosticTester/shared/key/").c_str()
#define PUBLIC_KEY_PATH asf::vdi::macro::GetFilePatch("/opt/AsfVirtualDiagnosticTester/shared/key/Publickey.txt").c_str()
#define TEMP_STORAGE_FOLDER asf::vdi::macro::GetFilePatch("/opt/AsfVirtualDiagnosticTester/shared/temp/").c_str()
#define TEMP_SCRIPT_STORAGE_FOLDER asf::vdi::macro::GetFilePatch("/opt/AsfVirtualDiagnosticTester/shared/temp/Script/").c_str()

namespace asf
{
namespace vdi
{
namespace macro
{

template<typename T>
std::string GetFilePatch(T path)
{
    std::string path_str;
    const auto path_env = getenv("NEUSAR_DEPLOYMENT_PATH");
    if (path_env != nullptr) {
        path_str = path_env;
    }
    path_str += path;
    return path_str;
}

}
}
}

#endif

#define SCRIPT_RUNING 1
#define SCRIPT_STOPPING 1
#define INTERACTING 1
#define MAINTAIN_NETWORK 1
#define LOCAL_DIAG_CLIENT_RUNNING 1
#define OTA_TASK_MAX_TIME 600

#endif // VD_MACRO_H_