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

#ifndef UDS_INF_INTERFACE_UDS_PROCESSOR_BASE_H__
#define UDS_INF_INTERFACE_UDS_PROCESSOR_BASE_H__

#include "diag_libnet_uds_client_channel.h"

#include "midware/diag/uds_app_com.h"

namespace midware
{
namespace diag
{

class UdsProcessorBase
{
public:
    using UdsClientChannel = asf::diag::communication::UdsClientChannel;
    using DiagMsgType = asf::diag::msg_on_neusar_net::DiagMsgType;

public:
    /**
     * \brief Constructor for UdsProcessorBase.
     */
    explicit UdsProcessorBase(UdsClientChannel& channel, const uint16_t diagnostic_address);
    virtual ~UdsProcessorBase() = default;

    void Start();

    void Stop();

protected:
    bool SendData(const uint16_t address, const std::vector<uint8_t>& data);

private:
    void ClientAvaliable(const bool avaliable);

    UdsProcessorBase(const UdsProcessorBase&) = delete;
    explicit UdsProcessorBase(UdsProcessorBase&&) = delete;
    UdsProcessorBase& operator=(const UdsProcessorBase&) = delete;
    UdsProcessorBase& operator=(UdsProcessorBase&&) = delete;

private:
    UdsClientChannel& channel_;

    uint16_t diagnostic_address_;

    std::atomic_bool offer_status_;

};

} // diag
} // midware

#endif // UDS_INF_INTERFACE_UDS_PROCESSOR_BASE_H__
