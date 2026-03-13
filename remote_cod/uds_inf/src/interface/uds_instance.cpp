#include "uds_instance.h"

#include "uds_common.h"

#include "log.h"

namespace midware
{
namespace diag
{

UdsInstance::UdsInstance()
    : processor_(nullptr)
    , serial_(nullptr)
    , test_persent_(nullptr)
{
}

void UdsInstance::Initialize(const std::string& config, const uint16_t diagnostic_address)
{
    LOG_ERROR << __FUNCTION__ << "config: " << config << "diagnostic_address: " << LOG_HEX(diagnostic_address);
    processor_ = std::make_shared<UdsProcessor>();
    if (processor_ != nullptr)
    {
        processor_->InitUdsChannel(config);
        processor_->CreateUdsProcessorSerial(diagnostic_address);
        serial_ = processor_->GetUdsProcessorSerial();
        test_persent_ = std::make_unique<UdsTestPresent>();
        if (test_persent_ != nullptr)
        {
            test_persent_->RegisterTransmitCallback([this](const uint16_t address, const std::vector<uint8_t> &request, std::vector<uint8_t> &response) -> int
                                                    { return SendTestPresent(address, request, response); });
        }
    }
}

void UdsInstance::SetTimeout(const int p2, const int p2_star)
{
    LOG_ERROR << __FUNCTION__ << "p2: " << p2 << "p2_star: " << p2_star;
    if (serial_ != nullptr)
    {
        serial_->SetTimeoutPeriod(p2, p2_star, 0);
    }
}

bool UdsInstance::Connect()
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (serial_ != nullptr)
    {
        serial_->Start();
        return true;
    }
    return false;
}

bool UdsInstance::Disconnect()
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (serial_ != nullptr)
    {
        serial_->Stop();
        return true;
    }
    return false;
}

int UdsInstance::TransmitAndReceive(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response)
{
    LOG_ERROR << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    int result {0};
    if (serial_ != nullptr)
    {
        auto ret = serial_->UdsRequest(target_address, request, response);
        result = static_cast<int>(ret);
        LOG_ERROR << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    return result;
}

int UdsInstance::TransmitMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& request)
{
    LOG_ERROR << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    int result {0};
    if (serial_ != nullptr)
    {
        std::vector<uint8_t> response;
        auto ret = serial_->UdsRequest(target_address, request, response);
        if (!response.empty())
        {
            LOG_WARN << "response is not empty! ";
        }
        result = static_cast<int>(ret);
        LOG_ERROR << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    return result;
}

int UdsInstance::FunctionalTransmitMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& request)
{
    LOG_ERROR << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    int result {0};
    if (serial_ != nullptr)
    {
        std::vector<uint8_t> response;
        auto ret = serial_->UdsRequest(target_address, request, response);
        if (!response.empty())
        {
            LOG_WARN << "response is not empty! ";
        }
        result = static_cast<int>(ret);
        LOG_ERROR << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    return result;
}

bool UdsInstance::StartTestPresent(const uint16_t source_address, const uint16_t target_address, const bool is_response_suppressable, const int period)
{
    LOG_ERROR << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    if (test_persent_ != nullptr)
    {
        return test_persent_->Start(target_address, is_response_suppressable, period);
    }
    return false;
}

bool UdsInstance::StopTestPresent(const uint16_t source_address, const uint16_t target_address)
{
    LOG_ERROR << __FUNCTION__ << "source_address: " << LOG_HEX(source_address) << "target_address: " << LOG_HEX(target_address);
    if (test_persent_ != nullptr)
    {
        return test_persent_->Stop(target_address);
    }
    return false;
}

int UdsInstance::SendTestPresent(const uint16_t address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response)
{
    LOG_ERROR << __FUNCTION__ << "address: " << LOG_HEX(address);
    int result {0};
    if (serial_ != nullptr)
    {
        auto ret = serial_->UdsRequest(address, request, response);
        result = static_cast<int>(ret);
        LOG_ERROR << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    return result;
}

} // diag
} // midware
