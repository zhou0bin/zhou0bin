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

#ifndef UDS_INF_INTERFACE_UDS_PROCESSOR_SERIAL_H__
#define UDS_INF_INTERFACE_UDS_PROCESSOR_SERIAL_H__

#include "uds_processor_base.h"

namespace midware
{
namespace diag
{

class UdsProcessorSerial : public UdsProcessorBase
{
public:
    /**
     * \brief A shared pointer to a UdsProcessorSerial object.
     */
    typedef std::shared_ptr<UdsProcessorSerial> Ptr;

public:
    /**
     * \brief Constructor for UdsProcessorSerial. 
     */
    explicit UdsProcessorSerial(UdsClientChannel& channel, const uint16_t diagnostic_address);
    virtual ~UdsProcessorSerial() noexcept;

    void SetTimeoutPeriod(const int p2, const int p2_star, const int max_num_of_pending);

    UdsResult UdsRequest(const uint16_t address, const std::vector<uint8_t>& request);

    UdsResult UdsRequest(const uint16_t address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response);

    void UdsResponse(const uint16_t address, const std::vector<uint8_t>& data);

private:
    UdsResult UdsSendAndRecv(const uint16_t target_address, const uint16_t target_ecu_address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response);

    void HandleUdsResponse(const uint16_t address, const std::vector<uint8_t>& data);

    void CheckUdsResponse(uint8_t& type, std::vector<uint8_t>& data);

    bool SuppressPositiveResponse(const std::vector<uint8_t>& data);

    void AddUdsResponse(std::vector<uint8_t> data);

    void GetUdsResponse(std::vector<uint8_t>& data);

    void ClearUdsResponseQueue();

    bool UdsResponseQueueIsEmpty();

    UdsProcessorSerial(const UdsProcessorSerial&) = delete;
    explicit UdsProcessorSerial(UdsProcessorSerial&&) = delete;
    UdsProcessorSerial& operator=(const UdsProcessorSerial&) = delete;
    UdsProcessorSerial& operator=(UdsProcessorSerial&&) = delete;

private:
    std::atomic<int> p2_;
    std::atomic<int> p2_star_;
    std::atomic<int> max_num_of_pending_;

    std::atomic<uint16_t> receving_address_;
    std::atomic<uint8_t> receving_sid_;

    std::mutex mutex_;
    std::mutex mutex_cv_;
    std::condition_variable condition_variable_;

    std::queue<std::vector<uint8_t> > response_queue_;

};

} // diag
} // midware

#endif // UDS_INF_INTERFACE_UDS_PROCESSOR_SERIAL_H__
