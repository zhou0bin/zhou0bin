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

#ifndef UDS_INF_UDS_INSTANCE_H__
#define UDS_INF_UDS_INSTANCE_H__

#include "uds_test_present.h"

#include "uds_processor.h"

namespace midware
{
namespace diag
{

class UdsInstance
{
public:
    static UdsInstance& GetInstance()
    {
        static UdsInstance* instance = nullptr;
        if (!instance)
        {
            static std::once_flag flag;
            std::call_once(flag, [&](){ instance = new (std::nothrow) UdsInstance(); });
        }
        return *instance;
    }

    void Initialize(const std::string& config, const uint16_t diagnostic_address = 0x0e80);

    void SetTimeout(const int p2, const int p2_star);

    bool Connect();

    bool Disconnect();

    int TransmitAndReceive(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response);

    int TransmitMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& request);

    int FunctionalTransmitMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& request);

    bool StartTestPresent(const uint16_t source_address, const uint16_t target_address, const bool is_response_suppressable, const int period);

    bool StopTestPresent(const uint16_t source_address, const uint16_t target_address);

protected:
    int SendTestPresent(const uint16_t address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response);

private:
    explicit UdsInstance();
    virtual ~UdsInstance() = default;
    UdsInstance(const UdsInstance&) = delete;
    explicit UdsInstance(UdsInstance&&) = delete;
    UdsInstance& operator=(const UdsInstance&) = delete;
    UdsInstance& operator=(UdsInstance&&) = delete;

private:
    std::shared_ptr<UdsProcessor> processor_;

    std::shared_ptr<UdsProcessorSerial> serial_;

    std::shared_ptr<UdsTestPresent> test_persent_;

};

} // diag
} // midware

#endif // UDS_INF_UDS_INSTANCE_H__
