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

#include "dcm/route/gateway/gateway_sync.h"

#include "dcm/route/gateway/gateway_route.h"

#include "common/multi_byte_type.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

GatewaySync::GatewaySync(GatewayRoute& route)
    : route_(route)
    , exit_requested_(false)
    , initialized_(false)
    , init_set_()
    , mutex_()
    , condition_variable_()
{
    thread_ = std::thread([this]() { GatewayRoutingTableSync(); });
}

void GatewaySync::StartGatewaySync()
{
    condition_variable_.notify_all();
}

void GatewaySync::StopGatewaySync()
{
    exit_requested_.store(true, std::memory_order_release);
    condition_variable_.notify_all();
    if (thread_.joinable())
    {
        thread_.join();
    }
}

bool GatewaySync::GetSyncResult()
{
    return initialized_.load(std::memory_order_acquire);
}

void GatewaySync::AddGatewayProcessor(const uint8_t processor_id)
{
    const std::unique_lock<std::mutex> locker(mutex_);
    (void)init_set_.insert(processor_id);
}

void GatewaySync::GatewayRoutingTableSync()
{
    std::unique_lock<std::mutex> locker(mutex_);
    while (!exit_requested_.load(std::memory_order_acquire))
    {
        if (init_set_.empty())
        {
            condition_variable_.wait(locker);
            continue;
        }
        if (!init_set_.empty())
        {
            GatewaySendSyncMessage();
            locker.unlock();
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            locker.lock();
        }
    }
}

void GatewaySync::GatewaySendSyncMessage()
{
    for (auto i : init_set_)
    {
        const std::vector<uint8_t> message{i};
        route_.GatewayRouteSendCustomMessage(message);
    }
}

void GatewaySync::GetewayRecvSyncMessage(const uds_transport::ByteVector& payload)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (payload.size() < 3)
    {
        LOG_ERROR << __FUNCTION__ << "payload size error! size: " << payload.size();
        return;
    }
    uint8_t id{ payload.at(0) };
    uint16_t count{ 0 };
    SetByte(count, payload.at(1), 1);
    SetByte(count, payload.at(2), 0);
    if (count != 0)
    {
        // if (payload.size() != static_cast<size_t>((count * 2) + 3))
        const size_t expected_size = (static_cast<size_t>(count) * 2) + 3;
        if (payload.size() != expected_size)
        {
            LOG_ERROR << __FUNCTION__ << "payload size != 2n + 3. size: " << payload.size();
            return;
        }
        for (size_t i = 3; (i + 1) < payload.size(); i = i + 2)
        {
            uint16_t address{ 0 };
            SetByte(address, payload.at(i), 1);
            SetByte(address, payload.at(i + 1), 0);
            LOG_ERROR << __FUNCTION__ << "id: " << id  << "count: " << count << "address: " << LOG_HEX(address);
            route_.GatewayRoutingUpdated(address, id);
        }
        {
            const std::unique_lock<std::mutex> locker(mutex_);
            if (init_set_.find(id) != init_set_.end())
            {
                (void)init_set_.erase(id);
            }
            if (init_set_.empty())
            {
                initialized_.store(true, std::memory_order_release);
            }
        }
        if (initialized_.load(std::memory_order_acquire))
        {
            LOG_ERROR << __FUNCTION__ << "Initialized! ";
        }
    }
}

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
