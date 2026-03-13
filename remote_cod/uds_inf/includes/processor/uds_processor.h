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

#ifndef UDS_INF_INTERFACE_UDS_PROCESSOR_H__
#define UDS_INF_INTERFACE_UDS_PROCESSOR_H__

#include "uds_processor_serial.h"

#include "diag_libnet_uds_client_channel.h"

#include "midware/diag/uds_app_com.h"

namespace midware
{
namespace diag
{

class UdsProcessor
{
public:
    using UdsClientChannel = asf::diag::communication::UdsClientChannel;
    using DiagMsgType = asf::diag::msg_on_neusar_net::DiagMsgType;

public:
    /**
     * \brief Constructor for UdsProcessor.
     */
    explicit UdsProcessor();
    virtual ~UdsProcessor() = default;

    void InitUdsChannel(const std::string& uds_channel);

    void CreateUdsProcessorSerial(const uint16_t diagnostic_address);

    UdsProcessorSerial::Ptr GetUdsProcessorSerial();

    void ReceiveProxyMessage(const uint16_t type, const std::vector<uint8_t>& data);

    void HandleDiagnosticMessage(const std::vector<uint8_t>& data);

private:
    UdsProcessor(const UdsProcessor&) = delete;
    explicit UdsProcessor(UdsProcessor&&) = delete;
    UdsProcessor& operator=(const UdsProcessor&) = delete;
    UdsProcessor& operator=(UdsProcessor&&) = delete;

private:
    std::shared_ptr<UdsClientChannel> channel_;

    UdsProcessorSerial::Ptr processor_;

};

} // diag
} // midware

#endif // UDS_INF_INTERFACE_UDS_PROCESSOR_H__
