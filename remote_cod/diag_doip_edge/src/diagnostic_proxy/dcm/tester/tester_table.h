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

#ifndef SOURCE_DCM_TESTER_TESTER_TABLE_H_
#define SOURCE_DCM_TESTER_TESTER_TABLE_H_

#include "diag_proxy_common.h"

// #include "ara/core/map.h"
#include <map>

namespace asf
{
namespace diag
{
namespace dcm
{
namespace tester
{

/**
 * \brief Class for tester table.
 */
class TesterTable
{
public:
    using TesterMap = std::map<uint16_t, uint8_t>;

public:
    explicit TesterTable();
    virtual ~TesterTable() = default;

    void AddLocalTesterTable(const uint16_t tester_address, const uint8_t tester_priority);

    void AddVirtualTesterTable(const uint16_t tester_address, const uint8_t tester_priority);

    uint8_t GetTesterType(const uint8_t tester_protocol, const uint16_t tester_address) const;

    uint8_t GetTesterPriority(const uint8_t tester_type, const uint16_t tester_address) const;

protected:
    bool LocalTesterTableEmpty() const;

    bool IsLocalTesterAddress(const uint16_t tester_address) const;

    uint8_t GetLocalTesterPriority(const uint16_t tester_address) const;

    bool VirtualTesterTableEmpty() const;

    bool VirtualTesterAddressValid(const uint16_t tester_address) const;

    uint8_t GetVirtualTesterPriority(const uint16_t tester_address) const;

private:
    explicit TesterTable(TesterTable&&) = delete;
    TesterTable(TesterTable&) = delete;
    TesterTable& operator=(TesterTable&) = delete;
    TesterTable& operator=(TesterTable&&) = delete;

private:
    TesterMap local_tester_map_;
    TesterMap virtual_tester_map_;

};

} /* namespace tester */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_TESTER_TESTER_TABLE_H_
