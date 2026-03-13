#ifndef UDS_INF_ASYNC_UDS_INTERFACE_H__
#define UDS_INF_ASYNC_UDS_INTERFACE_H__

#include "midware/diag/uds_app_com.h"

namespace midware
{
namespace diag
{
class AsyncUdsInterface
{
public:
    explicit AsyncUdsInterface(const uint16_t diagnostic_address);
    virtual ~AsyncUdsInterface() = default;

    void Offer() const;
    void StopOffer() const;

    bool Uds0x10Request(const uint16_t target_address, const Uds0x10RequestStruct& request);
    bool Uds0x27Request(const uint16_t target_address, const Uds0x27RequestStruct& request);
    bool Uds0x11Request(const uint16_t target_address, const Uds0x11RequestStruct& request);
    bool Uds0x28Request(const uint16_t target_address, const Uds0x28RequestStruct& request);
    bool Uds0x3ERequest(const uint16_t target_address, const Uds0x3ERequestStruct& request);
    bool Uds0x22Request(const uint16_t target_address, const Uds0x22RequestStruct& request);
    bool Uds0x2ERequest(const uint16_t target_address, const Uds0x2ERequestStruct& request);
    bool Uds0x31Request(const uint16_t target_address, const Uds0x31RequestStruct& request);
    bool Uds0x34Request(const uint16_t target_address, const Uds0x34RequestStruct& request);
    bool Uds0x35Request(const uint16_t target_address, const Uds0x35RequestStruct& request);
    bool Uds0x36Request(const uint16_t target_address, const Uds0x36RequestStruct& request);
    bool Uds0x37Request(const uint16_t target_address);
    bool Uds0x38Request(const uint16_t target_address, const Uds0x38RequestStruct& request);
    bool Uds0x2900Request(const uint16_t target_address);
    bool Uds0x2901Request(const uint16_t target_address, const Uds0x2901RequestStruct& request);
    bool Uds0x2902Request(const uint16_t target_address, const Uds0x2902RequestStruct& request);
    bool Uds0x2903Request(const uint16_t target_address, const Uds0x2903RequestStruct& request);
    bool Uds0x2904Request(const uint16_t target_address, const Uds0x2904RequestStruct& request);
    bool Uds0x2908Request(const uint16_t target_address);
    bool Uds0x14Request(const uint16_t target_address, const Uds0x14RequestStruct& request);
    bool Uds0x1901Request(const uint16_t target_address, const Uds0x1901RequestStruct& request);
    bool Uds0x1902Request(const uint16_t target_address, const Uds0x1902RequestStruct& request);
    bool Uds0x1903Request(const uint16_t target_address);
    bool Uds0x1904Request(const uint16_t target_address, const Uds0x1904RequestStruct& request);
    bool Uds0x1906Request(const uint16_t target_address, const Uds0x1906RequestStruct& request);
    bool Uds0x190ARequest(const uint16_t target_address);
    bool Uds0x85Request(const uint16_t target_address, const Uds0x85RequestStruct& request);

    void RegisterUds0x10ResponseCallback(const Uds0x10ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x27ResponseCallback(const Uds0x27ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x11ResponseCallback(const Uds0x11ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x28ResponseCallback(const Uds0x28ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x3EResponseCallback(const Uds0x3EResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x22ResponseCallback(const Uds0x22ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x2EResponseCallback(const Uds0x2EResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x31ResponseCallback(const Uds0x31ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x34ResponseCallback(const Uds0x34ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x35ResponseCallback(const Uds0x35ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x36ResponseCallback(const Uds0x36ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x37ResponseCallback(const Uds0x37ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x38ResponseCallback(const Uds0x38ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x2900ResponseCallback(const Uds0x2900ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x2901ResponseCallback(const Uds0x2901ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x2902ResponseCallback(const Uds0x2902ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x2903ResponseCallback(const Uds0x2903ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x2904ResponseCallback(const Uds0x2904ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x2908ResponseCallback(const Uds0x2908ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x14ResponseCallback(const Uds0x14ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x1901ResponseCallback(const Uds0x1901ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x1902ResponseCallback(const Uds0x1902ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x1903ResponseCallback(const Uds0x1903ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x1904ResponseCallback(const Uds0x1904ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x1906ResponseCallback(const Uds0x1906ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x190AResponseCallback(const Uds0x190AResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterUds0x85ResponseCallback(const Uds0x85ResponseCallback fun, const uint16_t target_address = kDefaultAddress);
    void RegisterNegativeResponseCallback(const NegativeResponseCallback fun, const uint16_t target_address = kDefaultAddress);

private:
    bool RequestMessage(const uint16_t target_address, const std::vector<uint8_t>& message) const;

    void HandleMessage(const uint16_t source_address, const std::vector<uint8_t>& message);

    void ParseUds0x10Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x27Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x11Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x28Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x3EResponse(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x22Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x2EResponse(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x31Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x34Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x35Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x36Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x37Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x38Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x2900Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x2901Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x2902Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x2903Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x2904Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x2908Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x14Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x1901Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x1902Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x1903Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x1904Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x1906Response(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x190AResponse(const uint16_t source_address, const std::vector<uint8_t>& message);
    void ParseUds0x85Response(const uint16_t source_address, const std::vector<uint8_t>& message);

    void ParseNegativeResponse(const uint16_t source_address, const std::vector<uint8_t>& message);

    explicit AsyncUdsInterface(AsyncUdsInterface&&) = delete;
    AsyncUdsInterface(AsyncUdsInterface&) = delete;
    AsyncUdsInterface& operator=(AsyncUdsInterface&) = delete;
    AsyncUdsInterface& operator=(AsyncUdsInterface&&) = delete;


private:
    uint16_t diagnostic_address_;

};

}
}

#endif
