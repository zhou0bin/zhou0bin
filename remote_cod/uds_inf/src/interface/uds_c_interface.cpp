#include "uds_c_interface.h"

#include "uds_c_processor.h"

#include <cstring>
#include <memory>
#include <mutex>
#include <map>

using UdsProcessorCtype = midware::diag::UdsProcessorCtype;

void Initialize(const char* data)
{
    UdsProcessorCtype::GetInstance().UdsInitialize(data);
}

void Offer(const int address)
{
    const auto sa = static_cast<uint16_t>(address);
    UdsProcessorCtype::GetInstance().UdsStart(sa);
}

void StopOffer(const int address)
{
    const auto sa = static_cast<uint16_t>(address);
    UdsProcessorCtype::GetInstance().UdsStop(sa);
}

void RegisterCallback(const int address, Callback callback)
{
    const auto sa = static_cast<uint16_t>(address);
    UdsProcessorCtype::GetInstance().RegisterUdsCallback(sa,
                                                         [callback](const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t> &message)
                                                         { callback((int)(source_address), (int)(target_address), message.size(), message.data()); });
}

bool Request(const int source_address, const int target_address, const int size, const unsigned char* data)
{
    const auto sa = static_cast<uint16_t>(source_address);
    const auto ta = static_cast<uint16_t>(target_address);
    std::vector<uint8_t> message;
    (void)message.insert(message.cend(), data, data + size);
    return UdsProcessorCtype::GetInstance().UdsRequest(sa, ta, message);
}
