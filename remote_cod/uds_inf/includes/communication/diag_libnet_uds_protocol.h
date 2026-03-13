#ifndef __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_UDS_PROTOCOL_H__
#define __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_UDS_PROTOCOL_H__

#include "neusar_net/buffer/inc/byte_buffer_utils.h"

#include "diag_libnet_uds_msg.h"

namespace asf
{
namespace diag
{
namespace communication
{

class UdsProtocolEncoder : public libnet::OutboundHandler<TlvMsg&&, std::unique_ptr<IoBuffer> >
{
public:
#ifdef USE_FS_LIBNET
    std::future<void> Write(Context* ctx, TlvMsg&& msg) override
    {
        libnet::IoByteBuffer content = std::move(msg.content());
        const uint32_t len = content.GetPktSize();

        std::unique_ptr<IoBuffer> iobuf((len == 0U) ? new IoBuffer() : content.GetBufferQueue().Head());
        content.GetBufferQueue().Stolen();

        libnet::ByteBufferUtils::SetInt(IoBuffer::Prepend(iobuf.get(), 4U), 0, static_cast<int>(len));
        libnet::ByteBufferUtils::SetShort(IoBuffer::Prepend(iobuf.get(), 2U), 0, static_cast<int>(msg.header().payload_type()));
        libnet::ByteBufferUtils::SetByte(IoBuffer::Prepend(iobuf.get(), 1U), 0, static_cast<int>(msg.header().inv_version()));
        libnet::ByteBufferUtils::SetByte(IoBuffer::Prepend(iobuf.get(), 1U), 0, static_cast<int>(msg.header().version()));

        return ctx->FireWrite(std::move(iobuf));
    }
#else
    virtual std::future<void> write(Context* const ctx, TlvMsg&& msg) override
    {
        libnet::IoByteBuffer content = std::move(msg.content());
        const uint32_t len = content.getPktSize();

        std::unique_ptr<IoBuffer> iobuf((len == 0U) ? new IoBuffer() : content.getBufferQueue().head());
        content.getBufferQueue().stolen();

        libnet::ByteBufferUtils::setInt(IoBuffer::prepend(iobuf.get(), 4U), 0, static_cast<int>(len));
        libnet::ByteBufferUtils::setShort(IoBuffer::prepend(iobuf.get(), 2U), 0, static_cast<int>(msg.header().payload_type()));
        libnet::ByteBufferUtils::setByte(IoBuffer::prepend(iobuf.get(), 1U), 0, static_cast<int>(msg.header().inv_version()));
        libnet::ByteBufferUtils::setByte(IoBuffer::prepend(iobuf.get(), 1U), 0, static_cast<int>(msg.header().version()));

        return ctx->fireWrite(std::move(iobuf));
    }
#endif
};

class UdsProtocolDecoder : public libnet::InboundHandler<libnet::BufferQueue&&, TlvMsg&&>
{
public:
#ifdef USE_FS_LIBNET
    virtual void Read(Context* ctx, libnet::BufferQueue&& in) override
    {
        TlvMsg msgOut(std::move(in));
        ctx->FireRead(std::move(msgOut));
    }
#else
    virtual void read(Context* const ctx, libnet::BufferQueue&& in) override
    {
        TlvMsg msgOut(std::move(in));
        ctx->fireRead(std::move(msgOut));
    }
#endif
};

} //namespace communication
} //namespace diag
} //asf

#endif // __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_UDS_PROTOCOL_H__
