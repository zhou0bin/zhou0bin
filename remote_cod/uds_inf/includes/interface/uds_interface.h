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
#ifndef UDS_INTERFACE_UDS_INTERFACE_H__
#define UDS_INTERFACE_UDS_INTERFACE_H__

#include "uds_common.h"

class UdsInterface
{
public:
    explicit UdsInterface(const std::string& config = "");

    virtual ~UdsInterface() = default;

    bool Connect();

    bool Disconnect();

    std::pair<int, std::vector<uint8_t> > UdsTransmitAndReceive(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& request, const std::vector<uint8_t>& target_response);

    int UdsTransmitMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& request);

    int UdsFunctionalTransmitMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& request);

    bool UdsStartTestPresent(const uint16_t source_address, const uint16_t target_address, const bool is_response_suppressable, const int period);

    bool UdsStopTestPresent(const uint16_t source_address, const uint16_t target_address);

};

#endif // UDS_INTERFACE_UDS_INTERFACE_H__
