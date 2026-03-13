#ifndef __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_SERVER_H__
#define __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_SERVER_H__

#include "neusar_net/bootstrap/inc/server_bootstrap.h"
#include "neusar_net/channel/inc/channel_handler.h"
#include "neusar_net/pipeline/inc/handler.h"
#include "neusar_net/codec/inc/length_field_based_frame_decoder.h"

#include "diag_libnet_msg.h"

#include <vector>
#include <mutex>
#include <thread>

namespace asf
{
namespace diag
{
namespace communication
{

class Server
{
public:
    using Ptr = std::shared_ptr<Server>;
    using HandleMessgaeCallback = std::function<void(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& data)>;
    using HandleAvaliableCallback = std::function<void(const uint64_t client_handle, const bool avaliable)>;

    explicit Server();
    virtual ~Server() = default;
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    void OfferService(const std::string& interface);
    bool SendMessage(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& data);
    void Stop();

    void RegisterHandleMessageCallback(const HandleMessgaeCallback fun);

    void RegisterHandleAvaliableCallback(const HandleAvaliableCallback fun);

protected:
    void HandleMessage(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& data);
    void HandleAvaliable(const uint64_t client_handle, const bool avaliable);

private:
    using Pipeline = libnet::Pipeline<libnet::BufferQueue&, TlvMsg&&>;
    class Handler : public libnet::HandlerAdapter<TlvMsg&&>
    {
    public:
        Handler(Server * const host);
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
        Server *host_;
    };

    class PipelineFactory : public libnet::PipelineFactory<Pipeline>
    {
    public:
        PipelineFactory(Server * const host);
#ifdef USE_FS_LIBNET
        virtual std::shared_ptr<Pipeline> NewPipeline(std::shared_ptr<libnet::Channel> channel) override;
#else
        virtual std::shared_ptr<Pipeline> newPipeline(std::shared_ptr<libnet::Channel> channel) override;
#endif
    private:
        Server *host_;
    };

    libnet::ServerBootstrap<Pipeline> server_;
    std::shared_ptr<wheels::IOThreadPoolExecutor> accept_group_;
    std::shared_ptr<wheels::IOThreadPoolExecutor> io_group_;
    std::thread wait_stop_thread_;
    std::vector<libnet::PipelineBase *> clients_pipeline_;
    std::mutex clients_pipeline_lock_;
    HandleMessgaeCallback handle_message_callback_;
    HandleAvaliableCallback handle_avaliable_callback_;

};

} //namespace communication
} //namespace diag
} //asf

#endif // __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_SERVER_H__
