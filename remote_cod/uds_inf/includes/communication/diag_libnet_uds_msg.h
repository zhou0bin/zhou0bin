#ifndef __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_UDS_MSG_H__
#define __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_UDS_MSG_H__

#include "neusar_net/buffer/inc/io_byte_buffer.h"

namespace asf
{
namespace diag
{
namespace communication
{

#ifdef USE_FS_LIBNET
using IoBuffer = libnet::IoBuffer;
#else
using IoBuffer = libnet::IOBuffer;
#endif

struct TlvMsgHdr
{
    uint8_t version_;
    uint8_t inv_version_;
    uint16_t payload_type_;
    uint32_t payload_length_;

    uint8_t version() const
    {
        return version_;
    }

    void version(const uint8_t ver)
    {
        version_ = ver;
    }

    uint8_t inv_version() const
    {
        return inv_version_;
    }

    void inv_version(const uint8_t inv_ver)
    {
        inv_version_ = inv_ver;
    }

    uint16_t payload_type()const
    {
        return payload_type_;
    }

    void payload_type(const uint16_t type)
    {
        payload_type_ = type;
    }

    uint32_t payload_length() const
    {
        return payload_length_;
    }

    void payload_length(const uint32_t length)
    {
        payload_length_ = length;
    }
};

class TlvMsg
{
public:
    explicit TlvMsg(libnet::BufferQueue bq)
        : content_(std::move(bq))
    {
#ifdef USE_FS_LIBNET
        header_.version(content_.ReadByte());
        header_.inv_version(content_.ReadByte());
        header_.payload_type(static_cast<uint16_t>(content_.ReadShort()));
        header_.payload_length(static_cast<uint32_t>(content_.ReadInt()));
#else
        header_.version(content_.readByte());
        header_.inv_version(content_.readByte());
        header_.payload_type(static_cast<uint16_t>(content_.readShort()));
        header_.payload_length(static_cast<uint32_t>(content_.readInt()));
#endif
    }

    explicit TlvMsg(const uint32_t bufferSize)
        : content_(libnet::BufferQueue(new IoBuffer(bufferSize)))
    {
        header_.version(0x00U);
        header_.inv_version(0xFFU);
        header_.payload_type(0U);
        header_.payload_length(0U);
    }

    explicit TlvMsg()
        : content_(libnet::BufferQueue(new IoBuffer()))
    {
        header_.version(0x00U);
        header_.inv_version(0xFFU);
        header_.payload_type(0U);
        header_.payload_length(0U);
    }

    virtual ~TlvMsg() = default;

    TlvMsgHdr& header()
    {
        return header_;
    }

    libnet::IoByteBuffer& content()
    {
        return content_;
    }

private:
    TlvMsgHdr header_;
    libnet::IoByteBuffer content_;
};

} //namespace communication
} //namespace diag
} //asf

#endif // __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_MSG_H__
