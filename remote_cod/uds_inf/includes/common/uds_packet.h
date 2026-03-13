// -----------------------------------------------------------------------
// |             _     _              _____         _____                |
// |            |  \  | |            / ____|  /\   |  __ \               |
// |            | | \ | |  __       | (___   /  \  | |__) |              |
// |            | |\ \| | /__\|   |  \___ \ / /\ \ |  _  /               |
// |            | | \ \ ||    |   |   ___) / /__\ \| | \ \               |
// |            |_|  \_\| \__/ \_/|/|_____/________\_|  \_\              |
// |                                                                     |
// -----------------------------------------------------------------------
// COPYRIGHT
// -----------------------------------------------------------------------
//
// This software is copyright protected and proprietary to Neusoft Reach.
// Neusoft Reach grants to you only those rights as set out in the license
// conditions.
// All other rights remain with Neusoft Reach.
// -----------------------------------------------------------------------

#ifndef UDS_INF_COMMON_UDS_PACKET_H__
#define UDS_INF_COMMON_UDS_PACKET_H__

#include <cstdint>
#include <vector>

namespace midware
{
namespace diag
{

class UdsPacket
{
public:    
    using os_vec_type = std::pair<std::vector<uint8_t>&, size_t>;

public:
    explicit UdsPacket()
        : packet_()
        , seek_(0U)
    {
    }

    explicit UdsPacket(const std::vector<uint8_t>& packet)
        : packet_(packet)
        , seek_(0U)
    {
    }

    virtual ~UdsPacket() = default;

    inline std::vector<uint8_t>& Packet()
    {
        return packet_;
    }

    inline const std::vector<uint8_t>& Packet() const
    {
        return packet_;
    }

    inline std::size_t Size() const
    {
        return packet_.size();
    }

    inline void Seek(const size_t _pos) const
    {
        seek_ = _pos;
    }

    inline void ReSeek() const
    {
        seek_ = 0U;
    }

    inline size_t CurrentSeek() const
    {
        return seek_;
    }

    inline void PushSeek(const size_t _count) const
    {
        seek_ += _count;
    }

    inline void BackSeek(const size_t _count) const
    {
        seek_ -= _count;
    }

    UdsPacket& operator << (const bool state)
    {
        packet_.push_back(static_cast<uint8_t>(state));
        return *this;
    }

    UdsPacket& operator << (const uint8_t bit8)
    {
        packet_.push_back(bit8);
        return *this;
    }

    UdsPacket& operator << (const uint16_t bit16)
    {
        packet_.push_back(static_cast<uint8_t>(bit16 >> 8));
        packet_.push_back(static_cast<uint8_t>(bit16));
        return *this;
    }

    UdsPacket& operator << (const uint32_t bit32)
    {
        packet_.push_back(static_cast<uint8_t>(bit32 >> 24));
        packet_.push_back(static_cast<uint8_t>(bit32 >> 16));
        packet_.push_back(static_cast<uint8_t>(bit32 >> 8));
        packet_.push_back(static_cast<uint8_t>(bit32));
        return *this;
    }

    UdsPacket& operator << (const std::vector<uint8_t>& bytes)
    {
        (void)packet_.insert(packet_.cend(), bytes.begin(), bytes.end());
        return *this;
    }

    UdsPacket& operator >> (bool& state)
    {
        if (seek_ < packet_.size())
        {
            state = packet_[seek_];
            ++seek_;
        }
        return *this;
    }

    UdsPacket& operator >> (uint8_t& bit8)
    {
        if (seek_ < packet_.size())
        {
            bit8 = packet_[seek_];
            ++seek_;
        }
        return *this;
    }

    UdsPacket& operator >> (uint16_t& bit16)
    {
        if ((seek_ + 2U) <= packet_.size())
        {
            bit16 = static_cast<uint16_t>((static_cast<uint32_t>(packet_[seek_]) << 8) | static_cast<uint32_t>(packet_[seek_ + 1U]));
            seek_ += 2U;
        }
        return *this;
    }

    UdsPacket& operator >> (uint32_t& bit32)
    {
        if ((seek_ + 4U) <= packet_.size())
        {
            bit32 = (static_cast<uint32_t>(packet_[seek_]) << 24)
                 | (static_cast<uint32_t>(packet_[seek_ + 1U]) << 16)
                 | (static_cast<uint32_t>(packet_[seek_ + 2U]) << 8)
                 | (static_cast<uint32_t>(packet_[seek_ + 3U]));
            seek_ += 4U;
        }
        return *this;
    }

    UdsPacket& operator >> (std::vector<uint8_t>& vec)
    {
        if (seek_ < packet_.size())
        {
            vec.assign(packet_.begin() + static_cast<ptrdiff_t>(seek_), packet_.end());
            seek_ = packet_.size();
        }
        return *this;
    }

    UdsPacket& operator >> (os_vec_type&& vec)
    {
        if ((seek_ + vec.second) <= packet_.size())
        {
            vec.first.resize(vec.second);
            for (size_t i = 0U; i < vec.second; ++i)
            {
                vec.first[i] = packet_[seek_];
                ++seek_;
            }
        }
        return *this;
    }

    const UdsPacket& operator >> (bool& state) const
    {
        if (seek_ < packet_.size())
        {
            state = packet_[seek_];
            ++seek_;
        }
        return *this;
    }

    const UdsPacket& operator >> (uint8_t& bit8) const
    {
        if (seek_ < packet_.size())
        {
            bit8 = packet_[seek_];
            ++seek_;
        }
        return *this;
    }

    const UdsPacket& operator >> (uint16_t& bit16) const
    {
        if ((seek_ + 2U) <= packet_.size())
        {
            bit16 = static_cast<uint16_t>((static_cast<uint32_t>(packet_[seek_]) << 8) | static_cast<uint32_t>(packet_[seek_ + 1U]));
            seek_ += 2U;
        }
        return *this;
    }

    const UdsPacket& operator >> (uint32_t& bit32) const
    {
        if ((seek_ + 4U) <= packet_.size())
        {
            bit32 = (static_cast<uint32_t>(packet_[seek_]) << 24)
                 | (static_cast<uint32_t>(packet_[seek_ + 1U]) << 16)
                 | (static_cast<uint32_t>(packet_[seek_ + 2U]) << 8)
                 | (static_cast<uint32_t>(packet_[seek_ + 3U]));
            seek_ += 4U;
        }
        return *this;
    }

    const UdsPacket& operator >> (std::vector<uint8_t>& vec) const
    {
        if (seek_ < packet_.size())
        {
            vec.assign(packet_.begin() + static_cast<ptrdiff_t>(seek_), packet_.end());
            seek_ = packet_.size();
        }
        return *this;
    }

    const UdsPacket& operator >> (os_vec_type&& vec) const
    {
        if ((seek_ + vec.second) <= packet_.size())
        {
            vec.first.resize(vec.second);
            for (size_t i = 0U; i < vec.second; ++i)
            {
                vec.first[i] = packet_[seek_];
                ++seek_;
            }
        }
        return *this;
    }

protected:
    UdsPacket(const UdsPacket&) = default;
    explicit UdsPacket(UdsPacket&&) = default;
    UdsPacket& operator=(const UdsPacket&) = default;
    UdsPacket& operator=(UdsPacket&&) = default;

private:
    std::vector<uint8_t> packet_;
    mutable size_t seek_;

};

} // namespace diag
} // namespace midware

#endif
