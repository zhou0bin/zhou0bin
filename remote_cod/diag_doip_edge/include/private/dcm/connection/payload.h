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

#ifndef LIB_LIBDMCOMMON_INCLUDE_DCM_CONNECTION_PAYLOAD_H_
#define LIB_LIBDMCOMMON_INCLUDE_DCM_CONNECTION_PAYLOAD_H_

#include <cstddef>
#include <cstdint>

#include "asf/diag/uds_transport/protocol_types.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

/**
 *  /brief This is the base class containing payload related data
 */
template <typename PayloadLengthType>
class PayloadOwner
{
public:
    /**
     * \brief Type of the payloadLength field.
     */
    typedef PayloadLengthType PayloadLength;

    /**
     * \brief Constructor of PayloadOwner.
     *
     * \param begin Begin of the payload.
     * \param end End of the payload.
     */
    template <typename InputIterator>
    PayloadOwner(InputIterator begin, const InputIterator& end)
    {
        (void)payload_.insert(payload_.begin(), begin, end);
    }

    /**
     * \brief Constructor of PayloadOwner.
     *
     * \param payload The payload.
     */
    explicit PayloadOwner(const uds_transport::ByteVector& payload)
    {
        payload_.reserve(payload.size());
        (void)payload_.insert(payload_.begin(), payload.begin(), payload.end());
    }

    PayloadOwner() = default;
protected:
    explicit PayloadOwner(PayloadOwner&&) = delete;
    PayloadOwner(const PayloadOwner&) = default;
    PayloadOwner& operator=(const PayloadOwner&) = delete;
    PayloadOwner& operator=(PayloadOwner&&) = default;
public:
    virtual ~PayloadOwner() = default;

    /**
     * payload of DoIp and Uds messages.
     */
    uds_transport::ByteVector payload_;

    /**
     * Returns size in Bytes of rawPayload.
     */
    inline size_t GetSize() const
    {
        return payload_.size();
    }

    /**
     * \brief This resizes the payload field.
     *
     * \param payload_length This is the size to which the payload field is adjusted.
     * \param force Not used in the current implementation.
     */
    inline virtual void SetPayloadLength(const PayloadLength payload_length, bool /*force*/ = false)
    {
        payload_.resize(payload_length);
    }
};

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif /* LIB_LIBDMCOMMON_INCLUDE_DCM_CONNECTION_PAYLOAD_H_ */
