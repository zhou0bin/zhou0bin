#include "uds_processor_base.h"

#include "log.h"

namespace midware
{
namespace diag
{

UdsProcessorBase::UdsProcessorBase(UdsClientChannel& channel, const uint16_t diagnostic_address)
    : channel_(channel)
    , diagnostic_address_(diagnostic_address)
    , offer_status_(false)
{
}

void UdsProcessorBase::Start()
{
    if (offer_status_.load(std::memory_order_relaxed) == false)
    {
        offer_status_.store(true, std::memory_order_relaxed);
        channel_.SetAvaliableNotifier(diagnostic_address_, [this](const bool avaliable) { ClientAvaliable(avaliable); });
        channel_.Offer(diagnostic_address_);
    }
}

void UdsProcessorBase::Stop()
{
    if (offer_status_.load(std::memory_order_relaxed) == true)
    {
        offer_status_.store(false, std::memory_order_relaxed);
        channel_.StopOffer(diagnostic_address_);
    }
}

void UdsProcessorBase::ClientAvaliable(const bool avaliable)
{
    if ((offer_status_.load(std::memory_order_relaxed) == true) && (avaliable == true))
    {
        channel_.SendOffer(diagnostic_address_);
    }
}

bool UdsProcessorBase::SendData(const uint16_t address, const std::vector<uint8_t>& data)
{
    if (offer_status_.load(std::memory_order_relaxed) == false)
    {
        LOG_ERROR << __FUNCTION__ << "no offer! ";
        return false;
    }
    std::vector<uint8_t> uds{};
    uds.push_back(GetByte(diagnostic_address_, 1U));
    uds.push_back(GetByte(diagnostic_address_, 0U));
    uds.push_back(GetByte(address, 1U));
    uds.push_back(GetByte(address, 0U));
    (void)uds.insert(uds.cend(), data.begin(), data.end());
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(address) << "size: " << data.size() << "payload: " << LOG_RAW_BUFFER(data);
    if (channel_.SendData(DiagMsgType::kDiagnosticMessage, uds) == false)
    {
        LOG_ERROR << __FUNCTION__ << "failed! ";
        return false;
    }
    LOG_ERROR << __FUNCTION__ << "success! ";
    return true;
}

} // diag
} // midware
