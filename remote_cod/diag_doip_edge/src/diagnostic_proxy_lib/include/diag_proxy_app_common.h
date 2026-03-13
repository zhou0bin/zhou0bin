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

#ifndef DIAG_PROXY_APP_COMMON_H_
#define DIAG_PROXY_APP_COMMON_H_

#include <unistd.h>
#include <cstring>
#include <vector>
#include <condition_variable>
#include <mutex>

// #include "ara/core/future.h"
// #include "ara/core/result.h"
#include <future>

using InstanceSpecifier = std::string;

namespace midware
{
namespace diag
{

struct GidStatus {
    std::vector<uint8_t> GID;
    uint8_t furtherActionReq;
    uint8_t syncStatus;
};

struct VinNumber {
    std::vector<uint8_t> VIN;
};

enum class PowerModeType : uint8_t {
    kNotReady = 0x00,
    kReady = 0x01,
    kNotSupported = 0x02
};

enum AuthenticationState : uint8_t
{
    kInitial = 0x00,
    kSuccess = 0x01,
    kFailure = 0x02,
    kProcessing = 0x03,
    kDoipDiagProcessing = 0x04
};

enum ReportFactoryMode : uint8_t
{
    kInitialMode = 0x00,
    kFactoryMode = 0xFF
};

}
}

#endif // DIAG_PROXY_APP_COMMON_H_
