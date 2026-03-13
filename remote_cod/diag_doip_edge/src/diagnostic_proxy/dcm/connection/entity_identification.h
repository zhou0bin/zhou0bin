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

#ifndef ARA_DIAG_DCM_CONNECTION_ENTITY_IDENTIFICATION_H_
#define ARA_DIAG_DCM_CONNECTION_ENTITY_IDENTIFICATION_H_

#include "asf/diag/uds_transport/protocol_types.h"
#include <arpa/inet.h>

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

/// @brief Base class for entity identification.
class EntityIdentification
{
public:
    EntityIdentification() = default;
    explicit EntityIdentification(EntityIdentification&&) = delete;
    EntityIdentification(EntityIdentification&) = delete;
    EntityIdentification& operator=(EntityIdentification&) = delete;
    EntityIdentification& operator=(EntityIdentification&&) = delete;
    virtual ~EntityIdentification() = default;

    /// @brief Gets a MAC (DoIp Entity Identification) for a given IPv4 address
    ///
    /// @param[in] givenSocket A socket to which a specific NIC device must already be bound to
    ///                        in order to get the MAC from it using getsockopt()
    /// @param[in,out] eid entity identification buffer
    /// @return Returns true if output value is valid.
    virtual bool GetEntityIdentification(const int givenSocket, const struct in_addr local_addr, asf::diag::uds_transport::ByteVector& eid);  // NOLINT

protected:
    /// @brief Fills the EID buffer with default values as described in ISO13400-2, Table 40
    ///
    /// @param[in,out] eid The buffer to be filled with default values
    virtual void FillBufferWithInvalidValue(asf::diag::uds_transport::ByteVector& eid);
};

}  // namespace connection
}  // namespace dcm
}  // namespace diag
}  // namespace asf

#endif  // ARA_DIAG_DCM_CONNECTION_ENTITY_IDENTIFICATION_H_
