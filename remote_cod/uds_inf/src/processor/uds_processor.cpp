#include "uds_processor.h"

#include "uds_common.h"

#include "log.h"

namespace midware
{
namespace diag
{

UdsProcessor::UdsProcessor()
    : channel_(nullptr)
    , processor_(nullptr)
{
}

void UdsProcessor::InitUdsChannel(const std::string& uds_channel)
{
    channel_ = std::make_shared<UdsClientChannel>(uds_channel);
    if (channel_ != nullptr)
    {
        channel_->SetMessageNotifier([this](const uint16_t type, const std::vector<uint8_t> &data)
                                     { ReceiveProxyMessage(type, data); });
    }
}

void UdsProcessor::CreateUdsProcessorSerial(const uint16_t diagnostic_address)
{
    if (channel_ != nullptr)
    {
        processor_ = std::make_shared<UdsProcessorSerial>(*channel_, diagnostic_address);
    }
}

UdsProcessorSerial::Ptr UdsProcessor::GetUdsProcessorSerial()
{
    return processor_;
}

void UdsProcessor::ReceiveProxyMessage(const uint16_t type, const std::vector<uint8_t>& data)
{
    LOG_INFO << __FUNCTION__ << "type: " << LOG_HEX(type);
    if (DiagMsgType::kDiagnosticMessage == type)
    {
        HandleDiagnosticMessage(data);
    }
    else if (DiagMsgType::kDiagnosticAck == type)
    {
        LOG_VERBOSE << __FUNCTION__ << "kDiagnosticAck! ";
    }
    else if (DiagMsgType::kDiagnosticNack == type)
    {
        LOG_VERBOSE << __FUNCTION__ << "kDiagnosticNack! ";
    }
    else
    {
        LOG_ERROR << __FUNCTION__ << "unknow type! type: " << LOG_HEX(type);
    }
}

void UdsProcessor::HandleDiagnosticMessage(const std::vector<uint8_t>& data)
{
    if (data.size() < 5)
    {
        LOG_ERROR << __FUNCTION__ << "data size < 5! ";
        return;
    }
    uint16_t source_address{0};
    uint16_t target_address{0};
    SetByte(source_address, data.at(0), 1);
    SetByte(source_address, data.at(1), 0);
    SetByte(target_address, data.at(2), 1);
    SetByte(target_address, data.at(3), 0);
    const std::vector<uint8_t> payload{data.begin() + 4, data.end()};
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(source_address) << "ta: " << LOG_HEX(target_address) << "size: " << payload.size() << "payload: " << LOG_RAW_BUFFER(payload);
    if (processor_ != nullptr)
    {
        processor_->UdsResponse(source_address, payload);
    }
}

} // diag
} // midware
