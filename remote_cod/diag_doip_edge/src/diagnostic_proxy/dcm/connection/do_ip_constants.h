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

/// @file
/// @brief Common place for DoIp constants and helper methods as defined in ISO 13400-2:2012

#ifndef ARA_DIAG_DCM_CONNECTION_DO_IP_CONSTANTS_H_
#define ARA_DIAG_DCM_CONNECTION_DO_IP_CONSTANTS_H_

#include <memory>

#include "common/random_number_generator.h"
#include "asf/diag/uds_transport/protocol_types.h"
using asf::diag::uds_transport::ByteVector;

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{
namespace doip
{

/// @brief Further action codes for GID as of table 20 of ISO 13400-2:2012
enum class GIDFurtherActions : uint8_t
{
    kNone = 0x00,
    kRoutingActivationRequired = 0x10
};

/// @brief Sync status of GID/VIN as of table 20 of ISO 13400-2:2012
enum class GIDSyncStatus : uint8_t
{
    kSynced = 0x00,
    kIncomplete = 0x10
};

/// @brief Default as of table 40 of ISO 13400-2:2012
const ByteVector kDefaultEntityIdentification(6, 0x00);

/// @brief Default as of table 40 of ISO 13400-2:2012
const ByteVector kDefaultGroupIdentification_00(6, 0x00);
const ByteVector kDefaultGroupIdentification_FF(6, 0xff);

/// @brief Default as of table 40 of ISO 13400-2:2012
const ByteVector kDefaultVehicleIdentificationNumber_00(17, 0x00);
const ByteVector kDefaultVehicleIdentificationNumber_FF(17, 0xff);

/// @brief Number of repeated Vehicle Announcement Messages as of table 38 of ISO 13400-2:2012
constexpr uint8_t kAnnounceNum = 3U;

/// @brief Time interval of repeated Vehicle Announcement Messages as of table 38 of ISO 13400-2:2012
constexpr uint16_t kAnnounceInterval = 500U;

/// @brief Random wait time generator for DoIpAnnounceWait()
const std::unique_ptr<common::RandomNumberGenerator> kDoIpAnnounceWaitGenerator
    = std::make_unique<common::RandomNumberGenerator>(0, kAnnounceInterval);

// @brief Timeout for the TCP Alive Check as of table 38 of ISO 13400-2:2012
extern const uint16_t kTcpAliveCheckTimeout;

// @brief Timeout for the TCP General Inactivity as of table 38 of ISO 13400-2:2012
extern const uint16_t kTcpGeneralInactivityTimeout; 

// @brief Timeout for the TCP Initial Inactivity as of table 38 of ISO 13400-2:2012
extern const uint16_t kTcpInitialInactivityTimeout;

// @brief Timeout for the active line timer
extern const uint16_t kActiveLineTimeout;

// @brief Timeout for the get network interface id timer
extern const uint16_t kNetworkInterfaceIdTimeout;

/// @brief Timeout for the diagnostic message timer
extern const uint16_t kDiagnosticMessageTimeout;

/// @brief Calculates a random wait time from [0,500] as specified in ISO 13400-2:2012. Table 38
///
/// @return The wait time. Note: this shall be interpreted as milliseconds, as specified in the ISO standard!
int DoIpAnnounceWait();

const uint16_t External_Test_Equipment_Addr_Min = 0x0E00;
const uint16_t External_Test_Equipment_Addr_Max = 0x0FFF;

}  // namespace doip
}  // namespace connection
}  // namespace dcm
}  // namespace diag
}  // namespace asf

#endif  // ARA_DIAG_DCM_CONNECTION_DO_IP_CONSTANTS_H_
