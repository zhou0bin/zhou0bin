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

/// @file

#ifndef APD_ARA_DIAG_UDS_TRANSPORT_UDS_MESSAGE_EXT_HPP_
#define APD_ARA_DIAG_UDS_TRANSPORT_UDS_MESSAGE_EXT_HPP_

#include <cstdint>  // uint8_t
#include <ostream>  // std::ostream

// #include "ara/core/optional.h"
#include <optional>

#include "asf/diag/uds_transport/protocol_types.h"
#include "asf/diag/uds_transport/uds_message.h"

namespace asf
{
namespace diag
{
namespace uds_transport
{

class UdsMessageExt : public UdsMessage
{
public:
    UdsMessageExt(const Address source_address,
        const Address target_address,
        const asf::diag::uds_transport::ByteVector& payload = {});
    explicit UdsMessageExt(UdsMessageExt&&) = default;
    UdsMessageExt(const UdsMessageExt&) = default;
    UdsMessageExt& operator=(const UdsMessageExt&) = default;
    UdsMessageExt& operator=(UdsMessageExt&&) = default;
    ~UdsMessageExt() override = default;

    /**
     * \brief Check whether the message is a negative response message
     * \return True if the message is a negative response, false otherwise
     */
    bool IsNegativeResponseMessage() const;
    /**
     * \brief Check whether the message is a positive response message
     * \return True if the message is a positive response, false otherwise
     */
    bool IsPositiveResponseMessage() const;
    /**
     * \brief Get the original request service id
     * \return The original request service id of the uds message
     */
    std::optional<uint8_t> GetRequestSid() const;
    std::optional<uint8_t> GetMessageSid() const;
    /**
     * \brief Get the NRC of the uds message.
     * \return The negative response code of the uds message.
     */
    std::optional<uint8_t> GetNrc() const;
    /**
     * \brief Check if the suppress positive response bit is set.
     */
    bool ShouldSuppressPositiveResponse() const;
    /**
     * \brief Get the sub-function of the uds message.
     * \return The sub-function of the uds message.
     */
    std::optional<uint8_t> GetSubFunction() const;
};

}  // namespace uds_transport
}  // namespace diag
}  // namespace asf

#endif  // APD_ARA_DIAG_UDS_TRANSPORT_UDS_MESSAGE_EXT_HPP_
