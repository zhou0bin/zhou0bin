// --------------------------------------------------------------------------
// |                _     _              _____         _____                |
// |               |  \  | |            / ____|  /\   |  __ \               |
// |               | | \ | |  __       | (___   /  \  | |__) |              |
// |               | |\ \| | /__\|   |  \___ \ / /\ \ |  _  /               |
// |               | | \ \ ||    |   |   ___) / /__\ \| | \ \               |
// |               |_|  \_\| \__/ \_/|/|_____/________\_|  \_\              |
// |                                                                        |
// --------------------------------------------------------------------------
// COPYRIGHT
// --------------------------------------------------------------------------
//
// This software is copyright protected and proprietary to Neusoft Reach.
// Neusoft Reach grants to you only those rights as set out in the license 
// conditions.
// All other rights remain with Neusoft Reach.
// --------------------------------------------------------------------------

#ifndef UDS_INF_INTERFACE_UDS_PROCESSOR_MANAGER_H__
#define UDS_INF_INTERFACE_UDS_PROCESSOR_MANAGER_H__

#include "diag_libnet_uds_client_channel.h"
#include "uds_processor_parallel.h"
#include "uds_processor_serial.h"

namespace midware
{
namespace diag
{

class UdsProcessorManager
{
public:
    using UdsClientChannel = asf::diag::communication::UdsClientChannel;
    using UdsProcessorTypeMap = std::map<uint16_t, uint8_t>;
    using UdsProcessorParallelMap = std::map<uint16_t, UdsProcessorParallel::Ptr>;
    using UdsProcessorSerialMap = std::map<uint16_t, UdsProcessorSerial::Ptr>;

public:
    static UdsProcessorManager& GetInstance()
    {
        static UdsProcessorManager* instance = nullptr;
        if (!instance)
        {
            static std::once_flag flag;
            std::call_once(flag, [&](){ instance = new (std::nothrow) UdsProcessorManager(); });
        }
        return *instance;
    }

    void SetUdsProcessorType(const uint16_t diagnostic_address, const uint8_t type);

    UdsProcessorParallel::Ptr FindOrCreateUdsProcessorParallel(const uint16_t diagnostic_address);

    UdsProcessorSerial::Ptr FindOrCreateUdsProcessorSerial(const uint16_t diagnostic_address);

protected:
    UdsProcessorParallel::Ptr GetUdsProcessorParallel(const uint16_t diagnostic_address);

    UdsProcessorSerial::Ptr GetUdsProcessorSerial(const uint16_t diagnostic_address);

    void ReceiveProxyMessage(const uint16_t type, const std::vector<uint8_t>& data);

    void HandleDiagnosticMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload);

private:
    explicit UdsProcessorManager();
    virtual ~UdsProcessorManager() noexcept;
    UdsProcessorManager(const UdsProcessorManager&) = delete;
    explicit UdsProcessorManager(UdsProcessorManager&&) = delete;
    UdsProcessorManager& operator=(const UdsProcessorManager&) = delete;
    UdsProcessorManager& operator=(UdsProcessorManager&&) = delete;

private:
    std::string channel_path_;
    std::unique_ptr<UdsClientChannel> channel_;

    UdsProcessorTypeMap uds_processor_type_map_;
    UdsProcessorParallelMap uds_processor_parallel_map_;
    UdsProcessorSerialMap uds_processor_serial_map_;
    std::mutex uds_processor_type_mutex_;
    std::mutex uds_processor_parallel_mutex_;
    std::mutex uds_processor_serial_mutex_;

};

} // diag
} // midware

#endif // UDS_INF_INTERFACE_UDS_PROCESSOR_H__
