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

#ifndef UDS_INF_INTERFACE_UDS_PROCESSOR_PARALLEL_H__
#define UDS_INF_INTERFACE_UDS_PROCESSOR_PARALLEL_H__

#include "uds_processor_base.h"

namespace midware
{
namespace diag
{

class UdsProcessorParallel : public UdsProcessorBase
{
public:
    /**
     * \brief A shared pointer to a UdsProcessorParallel object.
     */
    typedef std::shared_ptr<UdsProcessorParallel> Ptr;

    using UdsMsg = std::tuple<uint16_t, std::vector<uint8_t> >;
    using UdsMsgQueue = std::queue<UdsMsg>;

    using Callback = std::function<void(const uint16_t , const std::vector<uint8_t>& )>;

public:
    /**
     * \brief Constructor for UdsProcessorParallel. 
     */
    explicit UdsProcessorParallel(UdsClientChannel& channel, const uint16_t diagnostic_address);
    virtual ~UdsProcessorParallel() noexcept;

    bool UdsRequest(const uint16_t address, const std::vector<uint8_t>& data);

    void UdsResponse(const uint16_t address, const std::vector<uint8_t>& data);

    inline void RegisterUdsResponseCallback(const Callback callback)
    {
        callback_ = callback;
    }

protected:
    void ReceiveMessages();

    void HandleMessage(const uint16_t address, const std::vector<uint8_t>& data);

    void AddUdsMsg(UdsMsg msg);

    void GetUdsMsg(UdsMsg& msg);

    void ClearUdsMsgQueue();

    bool UdsMsgQueueIsEmpty();

private:
    UdsProcessorParallel(const UdsProcessorParallel&) = delete;
    explicit UdsProcessorParallel(UdsProcessorParallel&&) = delete;
    UdsProcessorParallel& operator=(const UdsProcessorParallel&) = delete;
    UdsProcessorParallel& operator=(UdsProcessorParallel&&) = delete;

private:
    std::atomic_bool exit_requested_;

    std::mutex mutex_;

    std::mutex mutex_cv_;

    std::condition_variable condition_variable_;

    UdsMsgQueue uds_msg_queue_;

    Callback callback_;

    std::thread thread_;

};

} // diag
} // midware

#endif // UDS_INF_INTERFACE_UDS_PROCESSOR_PARALLEL_H__
