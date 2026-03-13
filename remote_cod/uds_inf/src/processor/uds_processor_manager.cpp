#include "uds_processor_manager.h"

#include "uds_common.h"

#include "log.h"

namespace midware
{
namespace diag
{

using asf::diag::msg_on_neusar_net::DiagMsgType;

UdsProcessorManager::UdsProcessorManager()
    : channel_path_("/tmp/ASF_UDS_ON_NEUSAR_NET_Interface")
{
    channel_ = std::make_unique<UdsClientChannel>(channel_path_);
    if (channel_ != nullptr)
    {
        channel_->SetMessageNotifier([this](const uint16_t type, const std::vector<uint8_t>& data)
                                     { ReceiveProxyMessage(type, data); });
    }
}

UdsProcessorManager::~UdsProcessorManager()
{
    try {
        uds_processor_parallel_map_.clear();
        uds_processor_serial_map_.clear();
    } catch (...) {
    }
}

void UdsProcessorManager::SetUdsProcessorType(const uint16_t diagnostic_address, const uint8_t type)
{
    const std::lock_guard<std::mutex> locker_upt(uds_processor_type_mutex_);
    uds_processor_type_map_[diagnostic_address] = type;
}

UdsProcessorParallel::Ptr UdsProcessorManager::FindOrCreateUdsProcessorParallel(const uint16_t diagnostic_address)
{
    const std::lock_guard<std::mutex> locker_upp(uds_processor_parallel_mutex_);
    const uint16_t key{diagnostic_address};
    UdsProcessorParallelMap::iterator it(uds_processor_parallel_map_.find(key));
    if (it == uds_processor_parallel_map_.end())
    {
        const std::pair<UdsProcessorParallelMap::iterator, bool> result{
            uds_processor_parallel_map_.insert({key, std::make_shared<UdsProcessorParallel>(*channel_, key)})};
        it = result.first;
    }
    return it->second;
}

UdsProcessorSerial::Ptr UdsProcessorManager::FindOrCreateUdsProcessorSerial(const uint16_t diagnostic_address)
{
    const std::lock_guard<std::mutex> locker_ups(uds_processor_serial_mutex_);
    const uint16_t key{diagnostic_address};
    UdsProcessorSerialMap::iterator it(uds_processor_serial_map_.find(key));
    if (it == uds_processor_serial_map_.end())
    {
        const std::pair<UdsProcessorSerialMap::iterator, bool> result{
            uds_processor_serial_map_.insert({key, std::make_shared<UdsProcessorSerial>(*channel_, key)})};
        it = result.first;
    }
    return it->second;
}

UdsProcessorParallel::Ptr UdsProcessorManager::GetUdsProcessorParallel(const uint16_t diagnostic_address)
{
    const std::lock_guard<std::mutex> locker_upp(uds_processor_parallel_mutex_);
    const uint16_t key{diagnostic_address};
    UdsProcessorParallelMap::iterator it(uds_processor_parallel_map_.find(key));
    if (it != uds_processor_parallel_map_.end())
    {
        return it->second;
    }
    return nullptr;
}

UdsProcessorSerial::Ptr UdsProcessorManager::GetUdsProcessorSerial(const uint16_t diagnostic_address)
{
    const std::lock_guard<std::mutex> locker_ups(uds_processor_serial_mutex_);
    const uint16_t key{diagnostic_address};
    UdsProcessorSerialMap::iterator it(uds_processor_serial_map_.find(key));
    if (it != uds_processor_serial_map_.end())
    {
        return it->second;
    }
    return nullptr;
}

void UdsProcessorManager::ReceiveProxyMessage(const uint16_t type, const std::vector<uint8_t>& data)
{
    LOG_INFO << __FUNCTION__ << "type: " << LOG_HEX(type);
    if (DiagMsgType::kDiagnosticMessage == type)
    {
        if (data.size() < 5)
        {
            LOG_ERROR << __FUNCTION__ << "data size < 5! ";
            return;
        }
        uint16_t source_address{ 0 };
        uint16_t target_address{ 0 };
        SetByte(source_address, data.at(0), 1);
        SetByte(source_address, data.at(1), 0);
        SetByte(target_address, data.at(2), 1);
        SetByte(target_address, data.at(3), 0);
        const std::vector<uint8_t> payload{data.begin() + 4, data.end()};
        LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(source_address) << "ta: " << LOG_HEX(target_address) << "size: " << payload.size() << "payload: " << LOG_RAW_BUFFER(payload);
        HandleDiagnosticMessage(source_address, target_address, payload);
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

void UdsProcessorManager::HandleDiagnosticMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    uint8_t processor_type = common::ProcessorType::kParallel;
    {
        const std::lock_guard<std::mutex> locker_upt(uds_processor_type_mutex_);
        const auto result = uds_processor_type_map_.find(target_address);
        if (result != uds_processor_type_map_.end())
        {
            processor_type = uds_processor_type_map_[target_address];
        }
    }
    LOG_INFO  << __FUNCTION__ << "processor type: " << static_cast<unsigned>(processor_type);
    if (processor_type == common::ProcessorType::kParallel)
    {
        const auto processor = GetUdsProcessorParallel(target_address);
        if (processor != nullptr)
        {
            processor->UdsResponse(source_address, payload);
        }
    }
    else
    {
        const auto processor = GetUdsProcessorSerial(target_address);
        if (processor != nullptr)
        {
            processor->UdsResponse(source_address, payload);
        }
    }
    LOG_INFO << __FUNCTION__ << "out! ";
}

} // diag
} // midware
