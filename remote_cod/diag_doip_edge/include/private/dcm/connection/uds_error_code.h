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

#ifndef LIB_LIBDMCOMMON_INCLUDE_DCM_CONNECTION_UDS_ERROR_CODE_H_
#define LIB_LIBDMCOMMON_INCLUDE_DCM_CONNECTION_UDS_ERROR_CODE_H_

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

/**
 * \brief Enumerator with Uds negative response codes, positive response is needed for manufacturer/supplier indication
 */
enum UdsNegativeResponseCode : uint8_t
{
    kPositiveResponse = 0x00,
    kGeneralReject = 0x10,
    kServiceNotSupported = 0x11,
    kSubFunctionNotSupported = 0x12,
    kIncorrectMessageLengthOrInvalidFormat = 0x13,
    kBusyRepeatRequest = 0x21,
    kConditionsNotCorrect = 0x22,
    kRequestSequenceError = 0x24,
    kNoResponseFromSubnetComponent = 0x25,
    kFailurePreventsExecutionOfRequestedAction = 0x26,
    kRequestOutOfRange = 0x31,
    kSecurityAccessDenied = 0x33,
    kInvalidKey = 0x35,
    kExceededNumberOfAttempts = 0x36,
    kRequiredTimeDelayNotExpired = 0x37,
    kUploadDownloadNotAccepted = 0x70,
    kTransferDataSuspended = 0x71,
    kGeneralProgrammingFailure = 0x72,
    kWrongBlockSequenceCounter = 0x73,
    kRequestCorrectlyReceived_responsePending = 0x78,
    kSubfunctionNotSupportedInActiveSession = 0x7E,
    kServiceNotSupportedInActiveSession = 0x7F,
    kRpmTooHigh = 0x81,
    kRpmTooLow = 0x82,
    kEngineIsRunning = 0x83,
    kEngineIsNotRunning = 0x84,
    kEngineRunTimeTooLow = 0x85,
    kTemperatureTooHigh = 0x86,
    kTemperatureTooLow = 0x87,
    kVehicleSpeedTooHigh = 0x88,
    kVehicleSpeedTooLow = 0x89,
    kThrottlePedalTooHigh = 0x8A,
    kThrottlePedalTooLow = 0x8B,
    kTransmissionRangeNotInNeutral = 0x8C,
    kTransmissionRangeNotInGear = 0x8D,
    kBrakeSwitchNotClosed = 0x8F,
    kShifterLeverNotInPark = 0x90,
    kTorqueConverterClutchLocked = 0x91,
    kVoltageTooHigh = 0x92,
    kVoltageTooLow = 0x93,
    kResourceTemporarilyNotAvailable = 0x94,
    kNoProcessingNoResponse = 0xFF
};

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // LIB_LIBDMCOMMON_INCLUDE_DCM_CONNECTION_UDS_ERROR_CODE_H_
