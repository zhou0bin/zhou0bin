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

#ifndef ASF_DIAG_PROXY_INCLUDE_COMMON_MULTI_BYTE_TYPE_H_
#define ASF_DIAG_PROXY_INCLUDE_COMMON_MULTI_BYTE_TYPE_H_

#include <stdexcept>
#include <type_traits>

namespace asf
{
namespace diag
{

/**
 * \brief Return a single byte of the stored value_ .
 *
 * \param byte_offset The offset of the byte to return. byte_offset == 0 denotes the lowest byte.
 * \returns The byte from value_ denoted by byte_offset.
 */
template <typename T>
uint8_t GetByte(const T& value, const size_t byte_offset)
{
    if (byte_offset >= sizeof(T)) {
        throw std::out_of_range("MultiByteType byte_offset out of range");
    }
    return static_cast<uint8_t>(((value >> (byte_offset * 8)) & 0xFFU));
}

/**
 * \brief Set the byte at the given offset to the given value.
 *
 * \param value Reference to the value to modify.
 * \param byte_value The new value to store.
 * \param byte_offset The offset to store the new value at. byte_offset == 0 denotes the lowest byte.
 */
template <typename T, typename U = typename std::make_unsigned<T>::type>
void SetByte(T& value, const uint8_t byte_value, const size_t byte_offset)
{
    if (byte_offset >= sizeof(T)) {
        throw std::out_of_range("MultiByteType byte_offset out of range");
    }

    U clear_mask{0xFFU};
    clear_mask = static_cast<U>(clear_mask << (byte_offset * 8U));
    clear_mask = static_cast<U>(~clear_mask);
    value &= clear_mask;

    U temp{byte_value};
    temp = static_cast<U>(temp << (byte_offset * 8U));
    value |= temp;
}

} /* namespace diag */
} /* namespace asf */

#endif /* ASF_DIAG_PROXY_INCLUDE_COMMON_MULTI_BYTE_TYPE_H_ */
