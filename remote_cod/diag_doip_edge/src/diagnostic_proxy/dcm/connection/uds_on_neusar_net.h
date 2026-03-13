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

#ifndef SOURCE_DCM_CONNECTION_UDS_ON_NEUSAR_NET_H_
#define SOURCE_DCM_CONNECTION_UDS_ON_NEUSAR_NET_H_

#include <cstdint>
#include <cstring>

#include "uds_transport/uds_message_ext.h"

#include "asf/diag/uds_transport/protocol_handler.h"
#include "asf/diag/uds_transport/protocol_mgr.h"
#include "asf/diag/uds_transport/protocol_types.h"

#include "diag_proxy_common.h"

#include "diag_libnet_common.h"

#include "communication/diag_libnet_server.h"

// #include "ara/core/map.h"
#include <map>

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

namespace uds_on_neusar_net
{
    enum UdsNack : uint8_t
    {
        // kUdsInvalidSourceAddress = 0x02,
        // kUdsUnknownTargetAddress = 0x03,
        // kUdsDiagMessageTooLage = 0x04,
        kUdsOutOfMemoryToStore = 0x05,
        // kUdsTargetUnreachable = 0x06,
        // kUdsUnknownNetwork = 0x07,
        kUdsTransportProtocolError = 0x08
    };
}

/**
 * Uds Protocol Handler for Uds on Neusar Net
 *
 */
class UdsOnNeusarNet : public asf::diag::uds_transport::UdsTransportProtocolHandler
{
public:
    using UdsMessageConstPtr = uds_transport::UdsMessageConstPtr;
    using ChannelID = uds_transport::ChannelID;
    using UdsClientMap = std::map<uint32_t, uint64_t>;
    using DiagMsgQueue = std::queue<std::vector<uint8_t> >;

public:
    ///
    /// @brief Constructor for a new UdsOnNeusarNet.
    ///
    /// @param[in] config The config object.
    ///
    explicit UdsOnNeusarNet(const std::string& config, uds_transport::UdsTransportProtocolMgr& transport_protocol_mgr);
    explicit UdsOnNeusarNet(UdsOnNeusarNet&&) = delete;
    UdsOnNeusarNet(UdsOnNeusarNet&) = delete;
    UdsOnNeusarNet& operator=(UdsOnNeusarNet&) = delete;
    UdsOnNeusarNet& operator=(UdsOnNeusarNet&&) = delete;
    virtual ~UdsOnNeusarNet() override = default;

    virtual InitializationResult Initialize() override;

    virtual void Start() override;

    virtual void Stop() override;

    virtual bool NotifyReestablishment(const ChannelID channelId) override;

    virtual void Transmit(UdsMessageConstPtr const message, const ChannelID channelId) override;

    virtual void TransmitAck(const uint16_t sa, const uint16_t ta, const ChannelID channelId) override;

    virtual void TransmitNack(const uint16_t sa, const uint16_t ta, const uint8_t nack_code, const ChannelID channelId) override;

private:
    void ReceiveProxyMessage(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& payload);
    void ClientAvaliable(const uint64_t client, const bool avaliable);
    uint64_t GetClientBySpecifier(const uint32_t address);

    void HandleDiagnosticMessage(const std::vector<uint8_t>& message);

protected:
    uint16_t diagnostic_address_;
    uint16_t functional_address_;

private:
    std::shared_ptr<communication::Server> uds_on_neusar_net_server_;
    std::string uds_on_neusar_net_path_com_;

    UdsClientMap client_map_;
    std::mutex client_map_mutex_;

    std::string config_;

};

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_CONNECTION_UDS_ON_NEUSAR_NET_H_
