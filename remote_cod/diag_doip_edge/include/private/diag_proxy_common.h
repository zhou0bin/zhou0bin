#ifndef DIAG_PROXY_COMMON_H_
#define DIAG_PROXY_COMMON_H_
#include <unistd.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <cstring>
#include <memory>
#include <set>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>

namespace asf
{
namespace diag
{

const uint8_t kRouteSet[] = {0x00U, 0x01U, 0x02U};

enum RouteType : uint8_t
{
    kPartition = 0x00,
    kGateway = 0x01,
    kNode = 0x02
};

enum PartitionEnum : uint8_t
{
    kPartitionDcm = 0x01
};

enum GatewayEnum : uint8_t
{
    kGatewayM = 0x01
};

enum TesterType : uint8_t
{
    kUndefined = 0x00,
    kLocal = 0x01,
    kVirtual = 0x02,
    kVirtualLocal = 0x03
};

enum ArbitrateResult : uint8_t
{
    kArbitratePass = 0x01,
    kArbitrateReject = 0x02,
    kArbitratePending = 0x03
};

enum ArbitrateState : uint8_t
{
    kArbitrateInitial = 0x01,
    kArbitrateWorking = 0x02,
    kArbitrateKeeping = 0x03
};

enum ArbitrateMessageCtrl : uint8_t
{
    kArbitratRequestSend = 0x01,
    kArbitratRequestPending = 0x78,
    kArbitratRequestBusy = 0x94
};

enum AuthenticateResult : uint8_t
{
    kAuthenticateInitial = 0x00,
    kAuthenticateSuccess = 0x01,
    kAuthenticateFailure = 0x02
};

enum AuthenticationResult : uint8_t
{
    kAuthenticationGeneralReject = 0x01,
    kDeAuthenticationSuccessful = 0x10,
    kOwnershipVerificationNecessary = 0x11,
    kAuthenticationComplete = 0x12
};

enum AuthenticationState : uint8_t
{
    kInitial = 0x00,
    kSuccess = 0x01,
    kFailure = 0x02,
    kProcessing = 0x03
};

enum AuthenticationMode : uint8_t
{
    kAuthClose = 0x00,
    kAuthService29 = 0x01,
    kAuthService27 = 0x02
};

enum FactoryMode : uint8_t
{
    kInitialMode = 0x00,
    kFactoryMode = 0xFF
};

struct GidStatus {
    std::vector<uint8_t> GID;
    uint8_t furtherActionReq;
    uint8_t syncStatus;
};

enum class PowerModeType : uint8_t {
    kNotReady = 0x00,
    kReady = 0x01,
    kNotSupported = 0x02
};

} //diag
} //asf
#endif
