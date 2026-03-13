#ifndef UDS_INF_SYNC_UDS_INTERFACE_H__
#define UDS_INF_SYNC_UDS_INTERFACE_H__

#include "midware/diag/uds_app_com.h"

namespace midware
{
namespace diag
{
class SyncUdsInterface
{
public:
    explicit SyncUdsInterface(const uint16_t diagnostic_address);
    virtual ~SyncUdsInterface() = default;

    void Offer() const;
    void StopOffer() const;

    void SetTimeoutPeriod(const uint16_t p2 = 500, const uint16_t p2_star = 300, const uint16_t max_num_of_pending = 0);

    UdsResult Uds0x10(const uint16_t target_address, const Uds0x10RequestStruct& request, Uds0x10ResponseStruct& response, NegativeResponseStruct& negative_response);

    UdsResult Uds0x27(const uint16_t target_address, const Uds0x27RequestStruct& request, Uds0x27ResponseStruct& response, NegativeResponseStruct& negative_response);

    UdsResult Uds0x11(const uint16_t target_address, const Uds0x11RequestStruct& request, Uds0x11ResponseStruct& response, NegativeResponseStruct& negative_response);

    UdsResult Uds0x28(const uint16_t target_address, const Uds0x28RequestStruct& request, Uds0x28ResponseStruct& response, NegativeResponseStruct& negative_response);

    UdsResult Uds0x3E(const uint16_t target_address, const Uds0x3ERequestStruct& request, Uds0x3EResponseStruct& response, NegativeResponseStruct& negative_response);

    UdsResult Uds0x22(const uint16_t target_address, const Uds0x22RequestStruct& request, Uds0x22ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x2E(const uint16_t target_address, const Uds0x2ERequestStruct& request, Uds0x2EResponseStruct& response, NegativeResponseStruct& negative_response);

    UdsResult Uds0x31(const uint16_t target_address, const Uds0x31RequestStruct& request, Uds0x31ResponseStruct& response, NegativeResponseStruct& negative_response);

    UdsResult Uds0x34(const uint16_t target_address, const Uds0x34RequestStruct& request, Uds0x34ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x35(const uint16_t target_address, const Uds0x35RequestStruct& request, Uds0x35ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x36(const uint16_t target_address, const Uds0x36RequestStruct& request, Uds0x36ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x37(const uint16_t target_address, Uds0x37ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x38(const uint16_t target_address, const Uds0x38RequestStruct& request, Uds0x38ResponseStruct& response, NegativeResponseStruct& negative_response);

    UdsResult Uds0x2900(const uint16_t target_address, Uds0x29ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x2901(const uint16_t target_address, const Uds0x2901RequestStruct& request, Uds0x2901ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x2902(const uint16_t target_address, const Uds0x2902RequestStruct& request, Uds0x2902ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x2903(const uint16_t target_address, const Uds0x2903RequestStruct& request, Uds0x2903ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x2904(const uint16_t target_address, const Uds0x2904RequestStruct& request, Uds0x29ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x2908(const uint16_t target_address, Uds0x29ResponseStruct& response, NegativeResponseStruct& negative_response);

    UdsResult Uds0x14(const uint16_t target_address, const Uds0x14RequestStruct& request, Uds0x14ResponseStruct& response, NegativeResponseStruct& negative_response);

    UdsResult Uds0x1901(const uint16_t target_address, const Uds0x1901RequestStruct& request, Uds0x1901ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x1902(const uint16_t target_address, const Uds0x1902RequestStruct& request, Uds0x1902ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x1903(const uint16_t target_address, Uds0x1903ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x1904(const uint16_t target_address, const Uds0x1904RequestStruct& request, Uds0x1904ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x1906(const uint16_t target_address, const Uds0x1906RequestStruct& request, Uds0x1906ResponseStruct& response, NegativeResponseStruct& negative_response);
    UdsResult Uds0x190A(const uint16_t target_address, Uds0x190AResponseStruct& response, NegativeResponseStruct& negative_response);

    UdsResult Uds0x85(const uint16_t target_address, const Uds0x85RequestStruct& request, Uds0x85ResponseStruct& response, NegativeResponseStruct& negative_response);

    UdsResult UdsCommon(const uint16_t target_address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response);

private:
    UdsResult RequestMessage(const uint16_t target_address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response) const;

    UdsResult RequestMessage(const uint16_t target_address, const std::vector<uint8_t>& request) const;

    explicit SyncUdsInterface(SyncUdsInterface&&) = delete;
    SyncUdsInterface(const SyncUdsInterface&) = delete;
    SyncUdsInterface& operator=(const SyncUdsInterface&) = delete;
    SyncUdsInterface& operator=(SyncUdsInterface&&) = delete;

private:
    uint16_t diagnostic_address_;

};

}
}

#endif
