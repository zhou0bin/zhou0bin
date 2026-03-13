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

#ifndef UDS_INF_UDS_PROCESSOR_CTYPE_H__
#define UDS_INF_UDS_PROCESSOR_CTYPE_H__

#include "diag_libnet_uds_client_channel.h"

#include "midware/diag/uds_app_com.h"

namespace midware
{
namespace diag
{

class UdsProcessorCtype
{
public:
    using UdsClientChannel = asf::diag::communication::UdsClientChannel;

    using UdsMsg = std::tuple<uint16_t, uint16_t, std::vector<uint8_t> >;
    using UdsMsgQueue = std::queue<UdsMsg>;

    using UdsCallback = std::function<void(const uint16_t , const uint16_t , const std::vector<uint8_t>& )>;
    using UdsCallbackMap = std::map<uint16_t, UdsCallback>;

public:
    static UdsProcessorCtype& GetInstance()
    {
        static UdsProcessorCtype* instance = nullptr;
        if (!instance)
        {
            static std::once_flag flag;
            std::call_once(flag, [&](){ instance = new (std::nothrow) UdsProcessorCtype(); });
        }
        return *instance;
    }

    void UdsInitialize(std::string uds_channel);

    void UdsStart(const uint16_t source_address);

    void UdsStop(const uint16_t source_address);

    bool UdsRequest(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& data);

    inline void RegisterUdsCallback(const uint16_t address, const UdsCallback uds_callback)
    {
        uds_callback_map_[address] = uds_callback;
    }

protected:
    void ReceiveProxyMessage(const uint16_t type, const std::vector<uint8_t>& data);
    void ReceiveMessages();

    void HandleMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload);

    bool UdsMsgQueueIsEmpty();
    void ClearUdsMsgQueue();
    void AddUdsMsg(const UdsMsg& msg);
    void GetUdsMsg(UdsMsg& msg);

private:
    explicit UdsProcessorCtype();
    virtual ~UdsProcessorCtype() noexcept;
    UdsProcessorCtype(const UdsProcessorCtype&) = delete;
    explicit UdsProcessorCtype(UdsProcessorCtype&&) = delete;
    UdsProcessorCtype& operator=(const UdsProcessorCtype&) = delete;
    UdsProcessorCtype& operator=(UdsProcessorCtype&&) = delete;

private:
    std::unique_ptr<UdsClientChannel> channel_;

    std::atomic_bool initialized_;
    std::atomic_bool exit_requested_;

    UdsMsgQueue uds_msg_queue_;
    std::mutex uds_msg_queue_mutex_;
    std::condition_variable condition_variable_;

    std::thread thread_;

    UdsCallbackMap uds_callback_map_;

};

} // diag
} // midware

#endif // UDS_INF_UDS_PROCESSOR_CTYPE_H__
