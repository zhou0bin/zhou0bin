#include "uds_interface.h"

#include "uds_instance.h"

#include "log.h"

using UdsInstance = midware::diag::UdsInstance;

UdsInterface::UdsInterface(const std::string& config)
{
    UdsInstance::GetInstance().Initialize(config);
}

bool UdsInterface::Connect()
{
    LOG_INFO << "in! ";
    return UdsInstance::GetInstance().Connect();
}

bool UdsInterface::Disconnect()
{
    LOG_INFO << "in! ";
    return UdsInstance::GetInstance().Disconnect();
}

std::pair<int, std::vector<uint8_t> > UdsInterface::UdsTransmitAndReceive(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& request, const std::vector<uint8_t>& target_response)
{
    LOG_INFO << "in! ";
    std::vector<uint8_t> response;
    int ret = UdsInstance::GetInstance().TransmitAndReceive(source_address, target_address, request, response);
    if (ret == static_cast<int>(UdsResult::kPositiveResponse))
    {
        if ((response.size() < target_response.size()) || (!std::equal(target_response.cbegin(), target_response.cend(), response.cbegin())))
        {
            ret = static_cast<int>(UdsResult::kUdsResponseMismatch);
        }
    }
    return std::make_pair(ret, response);
}

int UdsInterface::UdsTransmitMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& request)
{
    LOG_INFO << "in! ";
    std::vector<uint8_t> response;
    int ret = UdsInstance::GetInstance().TransmitAndReceive(source_address, target_address, request, response);
    if (!response.empty())
    {
        LOG_WARN << LOG_RAW_BUFFER(response);
    }
    return ret;
}

int UdsInterface::UdsFunctionalTransmitMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& request)
{
    LOG_INFO << "in! ";
    std::vector<uint8_t> response;
    int ret = UdsInstance::GetInstance().TransmitAndReceive(source_address, target_address, request, response);
    if (!response.empty())
    {
        LOG_WARN << LOG_RAW_BUFFER(response);
    }
    return ret;
}

bool UdsInterface::UdsStartTestPresent(const uint16_t source_address, const uint16_t target_address, const bool is_response_suppressable, const int period)
{
    LOG_INFO << "in! ";
    return UdsInstance::GetInstance().StartTestPresent(source_address, target_address, is_response_suppressable, period);
}

bool UdsInterface::UdsStopTestPresent(const uint16_t source_address, const uint16_t target_address)
{
    LOG_INFO << "in! ";
    return UdsInstance::GetInstance().StopTestPresent(source_address, target_address);
}
