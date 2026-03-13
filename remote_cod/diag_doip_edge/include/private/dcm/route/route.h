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

#ifndef DCM_ROUTE_ROUTE_H_
#define DCM_ROUTE_ROUTE_H_

#include <cstdint>
#include <atomic>

#include "uds_transport/uds_message_ext.h"

// #include "ara/core/map.h"
#include <map>

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class Route
{
public:
    /**
     * \brief A shared pointer to a Route object.
     */
    typedef std::shared_ptr<Route> Ptr;

    /**
     * \brief Constructor for Route. 
     */
    explicit Route(const uint8_t type);
    virtual ~Route() = default;

    explicit Route(Route&&) = delete;
    Route(const Route&) = delete;
    Route& operator=(const Route&) = delete;
    Route& operator=(Route&&) = delete;

    virtual void Initialize() = 0;

    virtual void Deinitialize() = 0;

    virtual bool CheckAndHandleMessage(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message) = 0;

protected:
    inline void SetRoutingState(const bool state)
    {
        routing_.store(state, std::memory_order_release);
    }

    inline bool GetRoutingState() const
    {
        return routing_.load(std::memory_order_acquire);
    }

protected:
    uint8_t type_;

private:
    static std::atomic_bool routing_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif /* DCM_ROUTE_ROUTE_H_ */
