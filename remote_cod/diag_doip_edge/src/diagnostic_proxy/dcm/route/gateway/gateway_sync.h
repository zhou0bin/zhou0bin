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

#ifndef DCM_ROUTE_GATEWAY_SYNC_H_
#define DCM_ROUTE_GATEWAY_SYNC_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "uds_transport/uds_message_ext.h"

#include "diag_proxy_common.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class GatewayRoute;

class GatewaySync
{
public:
    explicit GatewaySync(GatewayRoute& route);
    virtual ~GatewaySync() = default;

    void StartGatewaySync();

    void StopGatewaySync();

    bool GetSyncResult();

    void AddGatewayProcessor(const uint8_t processor_id);

    void GatewayRoutingTableSync();

    void GatewaySendSyncMessage();

    void GetewayRecvSyncMessage(const uds_transport::ByteVector& payload);

private:
    explicit GatewaySync(GatewaySync&&) = delete;
    GatewaySync(GatewaySync&) = delete;
    GatewaySync& operator=(GatewaySync&) = delete;
    GatewaySync& operator=(GatewaySync&&) = delete;

private:
    GatewayRoute& route_;

    std::atomic_bool exit_requested_;

    std::atomic_bool initialized_;

    std::set<uint8_t> init_set_;

    std::mutex mutex_;

    std::condition_variable condition_variable_;

    std::thread thread_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // DCM_ROUTE_GATEWAY_SYNC_H_
