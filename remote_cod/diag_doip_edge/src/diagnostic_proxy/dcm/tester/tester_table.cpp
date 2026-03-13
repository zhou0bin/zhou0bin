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

#include "dcm/tester/tester_table.h"

#include "log.h"

#include "uds_transport/protocol_types_priv.h"
using asf::diag::uds_transport::UdsTransportProtocol;

namespace
{
constexpr uint8_t kDefaultLocalTesterPriority{0x01};
constexpr uint8_t kDefaultTesterPriority{0xFF};
}

namespace asf
{
namespace diag
{
namespace dcm
{
namespace tester
{

TesterTable::TesterTable()
    : local_tester_map_()
    , virtual_tester_map_()
{
}

void TesterTable::AddLocalTesterTable(const uint16_t tester_address, const uint8_t tester_priority)
{
    if (tester_address == 0) {
        return;
    }
    if (local_tester_map_.find(tester_address) != local_tester_map_.end()) {
        LOG_ERROR << __FUNCTION__ << "Local tester already set for address. ";
        return;
    }
    (void)local_tester_map_.insert(TesterMap::value_type(tester_address, tester_priority));
}

void TesterTable::AddVirtualTesterTable(const uint16_t tester_address, const uint8_t tester_priority)
{
    if ((tester_address == 0) || (tester_priority == 0)) {
        return;
    }
    if (virtual_tester_map_.find(tester_address) != virtual_tester_map_.end()) {
        LOG_ERROR << __FUNCTION__ << "Virtual tester already set for address. ";
        return;
    }
    (void)virtual_tester_map_.insert(TesterMap::value_type(tester_address, tester_priority));
}

uint8_t TesterTable::GetTesterType(const uint8_t tester_protocol, const uint16_t tester_address) const
{
    uint8_t tester_type {TesterType::kUndefined};
    if (tester_protocol == UdsTransportProtocol::DoIp)
    {
        if ((LocalTesterTableEmpty() == true) || (IsLocalTesterAddress(tester_address) == true))
        {
            tester_type = TesterType::kLocal;
        }
    }
    else if (tester_protocol == UdsTransportProtocol::UDSonNeusarNet)
    {
        if ((LocalTesterTableEmpty() == false) && (IsLocalTesterAddress(tester_address) == true))
        {
            tester_type = TesterType::kVirtualLocal;
        }
        else
        {
            if ((VirtualTesterTableEmpty() == true) || (VirtualTesterAddressValid(tester_address) == true))
            {
                tester_type = TesterType::kVirtual;
            }
        }
    }
    else
    {
        // kUndefined
    }
    return tester_type;
}

uint8_t TesterTable::GetTesterPriority(const uint8_t tester_type, const uint16_t tester_address) const
{
    if (tester_type == TesterType::kLocal)
    {
        return GetLocalTesterPriority(tester_address);
    }
    else
    {
        return GetVirtualTesterPriority(tester_address);
    }
}

bool TesterTable::LocalTesterTableEmpty() const
{
    return local_tester_map_.empty();
}

bool TesterTable::IsLocalTesterAddress(const uint16_t tester_address) const
{
    if (local_tester_map_.find(tester_address) != local_tester_map_.end()) {
        return true;
    }
    return false;
}

uint8_t TesterTable::GetLocalTesterPriority(const uint16_t tester_address) const
{
    const auto it(local_tester_map_.find(tester_address));
    if (it != local_tester_map_.end()) {
        return it->second;
    } else {
        // Nothing found? -> Return the "tester not supported" 
        return kDefaultLocalTesterPriority;
    }
}

bool TesterTable::VirtualTesterTableEmpty() const
{
    return virtual_tester_map_.empty();
}

bool TesterTable::VirtualTesterAddressValid(const uint16_t tester_address) const
{
    if (virtual_tester_map_.find(tester_address) != virtual_tester_map_.end()) {
        return true;
    }
    return false;
}

uint8_t TesterTable::GetVirtualTesterPriority(const uint16_t tester_address) const
{
    const auto it(virtual_tester_map_.find(tester_address));
    if (it != virtual_tester_map_.end()) {
        return it->second;
    } else {
        // Nothing found? -> Return the "tester not supported" 
        return kDefaultTesterPriority;
    }
}

} /* namespace tester */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
