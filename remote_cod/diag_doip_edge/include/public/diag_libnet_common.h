#ifndef __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_COMMON_H__
#define __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_COMMON_H__

#include <stdint.h>

namespace asf
{
namespace diag
{

enum CommMsgType : uint16_t
{
    kDataMessage = 0,
    kOfferMessage = 1,
    kStopOfferMessage = 2
};

namespace msg_on_neusar_net
{
    enum DiagMsgType : uint16_t
    {
        kDiagnosticMessage = 0x8001,
        kDiagnosticAck = 0x8002,
        kDiagnosticNack = 0x8003
    };
    enum DoIpMsgType : uint16_t
    {
        kUpdateActivationLineState = 0x0101,
        kGetActivationLineState = 0x0102,
        kGetNetworkInterfaceId = 0x0103,
        kGetVin = 0x0104,
        kGetGidStatus = 0x0105,
        kGetDoIPPowerMode = 0x0106,
        kTriggerVehicleAnnouncement = 0x0107
    };
    enum AuthMsgType : uint16_t
    {
        kUpdateAuthMode = 0x0201,
        kGetAuthMode = 0x0202,
        kUpdateAuthResult = 0x0203,
        kGetAuthResult = 0x0204,
        kUpdateAuthStatus = 0x0205,
        kAuthRequest = 0x0206,
        kAuthResponse = 0x0207
    };
}

} //diag
} //asf

#endif // __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_COMMON_H__
