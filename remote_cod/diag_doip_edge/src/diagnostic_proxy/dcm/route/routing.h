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

#ifndef SOURCE_DCM_ROUTING_H_
#define SOURCE_DCM_ROUTING_H_

#include "dcm/route/routing_table.h"
#include "dcm/route/routing_timer.h"

#include "dcm/route/route_factory.h"

#include "uds_transport/uds_message_ext.h"

namespace asf::diag::dcm::conversation
{
    class ConversationManager;
}

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

class Routing
{
public:
    using logicalAddr = uint16_t;
    using RoutingMsg = std::tuple<uint8_t, uds_transport::UdsMessageExt>;
    using ProcessorInfo = std::tuple<uint8_t, uint8_t, uint16_t>;

    using RoutingTimerMap = std::map<uint16_t, RoutingTimer::Ptr>;

    using RecevingFinishFunc = std::function<void(const uint16_t)>;
    using RecevingFinishFuncMap = std::map<uint16_t, RecevingFinishFunc>;

public:
    explicit Routing(conversation::ConversationManager& conversation_manager, const std::string& config);
    virtual ~Routing() = default;

    virtual void Initialize();
    virtual void Deinitialize();

    RoutingTable& GetRoutingTable();

    logicalAddr GetLogicalAddr();

    logicalAddr GetServiceAddr();

    logicalAddr GetFunctionalAddr();

    ProcessorInfo GetProcessorInfo(const uds_transport::UdsMessageExt& message);

    RoutingTimer::Ptr GetRoutingTimer(const uint16_t processor_address);

    void RoutingMessage(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void TransmitDiagnosticMsg(const uint8_t conversation_id, const uint16_t source_address, const uds_transport::ByteVector& payload);

    void TransmitDiagnosticAck(const uint8_t conversation_id, const uint16_t source_address);

    void TransmitDiagnosticNack(const uint8_t conversation_id, const uint16_t source_address, const uint8_t nack_code);

    void ProcessorRecevingFinish(const uint16_t processor_address);

    void ProcessorRecevingTimeout(const uint16_t processor_address);

    void Subscribe(const uint16_t processor_address, const RecevingFinishFunc fun);

    conversation::ConversationManager& GetConversationManager()
    {
        return conversation_manager_;
    }

private:
    void RoutingMessageQueue();

    void HandleRoutingMessage(const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    bool SendRoutingMessage(const uint8_t processor_type, const uint8_t conversation_id, const uds_transport::UdsMessageExt& message);

    void SendPartitionSyncMessage(const uint8_t conversation_id, const uint8_t processor_id, const uds_transport::UdsMessageExt& message);

    void TransmitPartitionSyncMsg(const uint8_t conversation_id, const uds_transport::ByteVector& payload);

    void HandleRecevingFinish(const uint16_t processor_address);

    void CreateRoutingTimer(const uint16_t processor_address);

    void AddRoutingMessage(RoutingMsg message);

    RoutingMsg GetRoutingMessage();

    void ClearRoutingMessageQueue();

    bool RoutingMessageQueueIsEmpty();

protected:
    /**
     * \brief The ConversationManager used by the routing.
     */
    conversation::ConversationManager& conversation_manager_;

private:
    std::string config_;
    RoutingTable routing_table_;
    logicalAddr logical_address_;
    logicalAddr service_address_;
    logicalAddr functional_address_;
    int p2_;
    int p2_star_;
    int transmission_delay_;
    std::atomic_bool exit_requested_;
    std::mutex mutex_;
    std::condition_variable condition_variable_;
    std::queue<RoutingMsg> queue_;
    std::thread thread_;
    std::mutex routing_timer_mutex_;
    RoutingTimerMap routing_timer_map_;
    RecevingFinishFuncMap callback_map_;

};

} /* namespace route */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_ROUTING_H_
