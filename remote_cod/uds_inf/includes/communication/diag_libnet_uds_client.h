#ifndef __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_UDS_CLIENT_H__
#define __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_UDS_CLIENT_H__

#include "neusar_net/bootstrap/inc/client_bootstrap.h"
#include "neusar_net/channel/inc/channel_handler.h"
#include "neusar_net/codec/inc/length_field_based_frame_decoder.h"

#include "diag_libnet_uds_msg.h"

namespace asf
{
namespace diag
{
namespace communication
{

class UdsClient
{
public:
    using Ptr = std::shared_ptr<UdsClient>;
    using HandleMessgaeCallback = std::function<void(const uint16_t type, const std::vector<uint8_t>& data)>;
    using HandleAvaliableCallback = std::function<void(const bool avaliable)>;

    explicit UdsClient();
    virtual ~UdsClient() = default;
    UdsClient(const UdsClient&) = delete;
    UdsClient& operator=(const UdsClient&) = delete;

    bool FindService(const std::string& interface, const HandleAvaliableCallback fun);
    bool SendMessage(const uint16_t type, const std::vector<uint8_t>& data);
    void Stop();

    void RegisterHandleMessageCallback(const HandleMessgaeCallback fun);

protected:
    void RegisterHandleAvaliableCallback(const HandleAvaliableCallback fun);

    void HandleMessage(const uint16_t type, const std::vector<uint8_t>& data);
    void HandleAvaliable(const bool avaliable);

private:
    using Pipeline = libnet::Pipeline<libnet::BufferQueue&, TlvMsg&&>;
    class Handler : public libnet::HandlerAdapter<TlvMsg&&>
    {
    public:
        Handler(UdsClient * const host);
#ifdef USE_FS_LIBNET
        void Read(Context* ctx, TlvMsg&& msg) override;
        void ReadException(Context* ctx, libnet::ChannelException ex) override;
        void ReadEOF(Context* ctx);
        void TransportActive(Context* ctx);
#else
        virtual void read(Context* const ctx, TlvMsg&& msg) override;
        virtual void readException(Context* const ctx, libnet::ChannelException ex) override;
        virtual void readEOF(Context* const ctx);
        virtual void transportActive(Context* const ctx);
#endif
    private:
        UdsClient *host_;
    };

    class PipelineFactory : public libnet::PipelineFactory<Pipeline>
    {
    public:
        PipelineFactory(UdsClient * const host);
#ifdef USE_FS_LIBNET
        virtual std::shared_ptr<Pipeline> NewPipeline(std::shared_ptr<libnet::Channel> channel) override;
#else
        virtual std::shared_ptr<Pipeline> newPipeline(std::shared_ptr<libnet::Channel> channel) override;
#endif
    private:
        UdsClient *host_;
    };

    libnet::ClientBootstrap<Pipeline> client_;
    HandleMessgaeCallback handle_message_callback_;
    HandleAvaliableCallback handle_avaliable_callback_;

};

} //namespace communication
} //namespace diag
} //asf

#endif // __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_UDS_CLIENT_H__
