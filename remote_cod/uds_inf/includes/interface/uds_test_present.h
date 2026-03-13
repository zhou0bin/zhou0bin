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

#ifndef UDS_INF_UDS_TEST_PRESENT_H__
#define UDS_INF_UDS_TEST_PRESENT_H__

#include "midware/diag/uds_app_com.h"

namespace midware
{
namespace diag
{

class UdsTestPresent
{
public:
    using TransmitCallback = std::function<int(const uint16_t , const std::vector<uint8_t>& , std::vector<uint8_t>& )>;

public:
    explicit UdsTestPresent();
    virtual ~UdsTestPresent() noexcept;

    bool Start(const uint16_t address, const bool is_response_suppressable, const int period);

    bool Stop(const uint16_t address);

    inline void RegisterTransmitCallback(const TransmitCallback callback)
    {
        callback_ = callback;
    }

protected:
    void TestPresent();

    bool SendTestPresent(const uint16_t address, const bool is_response_suppressable);

    bool SendTestPresent(const uint16_t address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response);

private:
    UdsTestPresent(const UdsTestPresent&) = delete;
    explicit UdsTestPresent(UdsTestPresent&&) = delete;
    UdsTestPresent& operator=(const UdsTestPresent&) = delete;
    UdsTestPresent& operator=(UdsTestPresent&&) = delete;

private:
    std::atomic<uint16_t> address_;

    std::atomic_bool is_response_suppressable_;

    std::atomic<int> period_;

    std::atomic_bool start_test_present_;

    std::atomic_bool exit_requested_;

    std::condition_variable condition_variable_;

    std::mutex mutex_;

    std::thread thread_;

    TransmitCallback callback_;

};

}
}

#endif // UDS_INF_UDS_TEST_PRESENT_H__
