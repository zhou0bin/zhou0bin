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

#ifndef UDS_INF_COMMON_UDS_COMMON_H__
#define UDS_INF_COMMON_UDS_COMMON_H__

#include <cstdint>
#include <string>
#include <set>
#include <vector>

namespace
{
constexpr size_t kFilePositionSize {8U};
constexpr size_t kDtcSize {3U};
constexpr uint32_t kUdsP2StarResolution {10U};
constexpr uint8_t kUdsNackSid {0x7fU};
constexpr uint8_t kUdsNackPending {0x78U};
const uint8_t kServiceWithSubfunctionSet[] = {0x10U, 0x11U, 0x19U, 0x27U, 0x28U, 0x29U, 0x31U, 0x38U, 0x3eU, 0x85U};
}

namespace midware
{
namespace diag
{

namespace UdsServiceID
{
    enum RequestSid : uint8_t
    {
        kSessionControl = 0x10,
        kEcuRest = 0x11,
        kClearDiagnosticInformation = 0x14,
        kReadDTCInformation = 0x19,
        kReadDataByIndentifier = 0x22,
        kSecurityAccess = 0x27,
        kCommunicationControl = 0x28,
        kAuthentication = 0x29,
        kWriteDataByIdentifier = 0x2e,
        kRoutineControl = 0x31,
        kRequestDownload = 0x34,
        kRequestUpload = 0x35,
        kTransferData = 0x36,
        kTransferExit = 0x37,
        kTequestFileTransfer = 0x38,
        kTesterPresent = 0x3e,
        kControlDTCSetting = 0x85
    };

    enum ResponseSid : uint8_t
    {
        kRSessionControl = 0x50,
        kREcuResResponset = 0x51,
        kRClearDiagnosticInformation = 0x54,
        kRReadDTCInformation = 0x59,
        kRReadDataByIndentifier = 0x62,
        kRSecurityAccess = 0x67,
        kRCommunicationControl = 0x68,
        kRAuthentication = 0x69,
        kRWriteDataByIdentifier = 0x6e,
        kRRoutineControResponsel = 0x71,
        kRRequestDownload = 0x74,
        kRRequestUpload = 0x75,
        kRTransferData = 0x76,
        kRTransferExit = 0x77,
        kRTequestFileTransfer = 0x78,
        kRTesterPresent = 0x7e,
        kRControlDTCSetting = 0xc5
    };
}

namespace UdsSubFunction
{
    enum SessionControl : uint8_t
    {
        kDefaultSession = 0x01,
        kProgrammingSession = 0x02,
        kExtendedSession = 0x03
    };
    enum EcuReset : uint8_t
    {
        kHardReset = 0x01,
        kKeyOffOnReset = 0x02,
        kSoftReset = 0x03
    };
    enum CommunicationControl : uint8_t
    {
        kEnableRxAndTx = 0x00,
        kEnableRxAndDisableTx = 0x01,
        kDisableRxAndEnableTx = 0x02,
        kDisableRxAndTx = 0x03
    };
    enum RoutineControl : uint8_t
    {
        kStartRoutine = 0x01,
        kStopRoutine = 0x02,
        kRequestRoutineResults = 0x03
    };
    enum TesterPresent : uint8_t
    {
        kNoSuppressPosRspMsgIndicationBit = 0x00,
        kSuppressPosRspMsgIndicationBit = 0x80
    };
    enum ControlDTCSetting : uint8_t
    {
        kOn = 0x01,
        kOff = 0x02
    };
    enum ReadDTCInformation : uint8_t
    {
        kReportNumberOfDTCByStatusMask = 0x01,
        kReportDTCByStatusMask = 0x02,
        kReportDTCSnapshotIdentification = 0x03,
        kReportDTCSnapshotRecordByDTCNumber = 0x04,
        kReportDTCExtDataRecordByDTCNumber = 0x06,
        kReportSupportedDTC = 0x0A
    };
    enum Authentication : uint8_t
    {
        kDeAuthenticate = 0x00,
        kVerifyCertificateUnidirectional = 0x01,
        kVerifyCertificateBidirectional = 0x02,
        kProofOfOwnership = 0x03,
        kTransmitCertificate = 0x04,
        kAuthenticationConfiguration = 0x08
    };
} //UdsSubFunction

namespace common
{
    enum ProcessorType : uint8_t
    {
        kParallel = 0x00,
        kSerial = 0x01
    };
}

} // diag
} // midware
#endif
