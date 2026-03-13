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

#ifndef __SF_DIAG_SCRIPT_COMMON_H__
#define __SF_DIAG_SCRIPT_COMMON_H__

#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <vector>

#include "parse_json.h"

namespace midware
{
namespace diag
{
namespace script
{
using socket_fd = int;

enum ScriptType : uint8_t
{
    kCloud = 0x01U,
    kLocal = 0x02U,
    kStop  = 0x03U,
    kNone  = 0x04U
};

enum ScriptOpc : uint8_t
{
   kTooMany     = 0X01,//request queue is full
   kExcutedOver  = 0X02,//script had executed
   kExcutedFaile     = 0X03,//script had not executed
   kNoscript = 0X04,//find no script
   kNoZIP    = 0X05,//the scriptzip does not exist
   kConditionNotAllow = 0X06,//conditions do not allow
   kDecfaile = 0X07,//reserved fields: "decompression failed"
   kStopSuccess = 0x08,//stop script
   kFindNoScriptTh = 0x09,//find no script thread
   kScriptRunning = 0x0A,//script is running
   kUnKnown  = 0x0B,//unknown err
   kStoping = 0x0C,
   kIncomplete = 0x0D,

   kStartTask = 0x0E,
   kStopTask = 0x0F,

   kWaiting = 0x10
};

void GlobalInitialize();
bool Initialized();
bool IsSecureStartup();
std::string GetRootPath();
std::string GetConfigPath();
std::string GetStopFlagPath();
std::string GetCloudScriptPath();
std::string GetLocalScriptPath();
bool FileAndDirInit();
void FileMoveToData(const std::string& sourcepath);
void GetScriptFilePath(const std::string& py, const std::string& guid, std::string& path);
bool GetConditionFilePath(const std::string& py, const std::string& guid, std::string& path);
void GetBackupConditionFilePath(std::string& path);
std::string GetVehicleDataPath();
std::string GetDiagResultPath();
void GetConditionPath(std::string& vehicle_data, std::string& script_parameter, std::string& tmp_check_result, std::string& diag_result);
std::string GetConfig(const char* const key);

std::vector<uint8_t> MakeMessage(const std::string& str, const uint8_t type, const uint8_t error);
void DisplaySendData(const std::vector<uint8_t>& msg);

ssize_t Write(const socket_fd sockfd, const std::vector<uint8_t>& msg);
void Write(const socket_fd sockfd, unsigned char * buff, size_t count);
ssize_t Read(const socket_fd sockfd, std::vector<uint8_t>& msg);
ssize_t Read(const socket_fd sockfd, unsigned char * buff, size_t count);
void Close(const socket_fd sockfd);

} // script
} // diag
} // midware

#endif
