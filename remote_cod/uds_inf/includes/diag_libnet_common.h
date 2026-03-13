#ifndef __ASF_DIAG_UDS_INF_COMMUNICATION_DIAG_LIBNET_COMMON_H__
#define __ASF_DIAG_UDS_INF_COMMUNICATION_DIAG_LIBNET_COMMON_H__

#include <cstdint>

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
}

} // diag
} // asf

#endif // __ASF_DIAG_UDS_INF_COMMUNICATION_DIAG_LIBNET_COMMON_H__
