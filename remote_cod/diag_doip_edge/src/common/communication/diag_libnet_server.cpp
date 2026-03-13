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

#include "communication/diag_libnet_server.h"

#include "communication/diag_libnet_protocol.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace communication
{

Server::Server()
    : accept_group_(std::make_shared<wheels::IOThreadPoolExecutor>(1))
    , io_group_(std::make_shared<wheels::IOThreadPoolExecutor>(1))
{
#ifdef USE_FS_LIBNET
    (void)server_.ChildPipeline(std::make_shared<PipelineFactory>(this));
    (void)server_.Group(accept_group_, io_group_);
#else
    (void)server_.childPipeline(std::make_shared<PipelineFactory>(this));
    (void)server_.group(accept_group_, io_group_);
#endif
}

void Server::OfferService(const std::string& interface)
{
#ifdef USE_FS_LIBNET
    server_.Bind(libnet::SocketAddress(interface));
    wait_stop_thread_ = std::thread([this]() { server_.WaitForStop(); });
#else
    server_.bind(libnet::SocketAddress(interface));
    wait_stop_thread_ = std::thread([this]() { server_.waitForStop(); });
#endif
    (void)pthread_setname_np(wait_stop_thread_.native_handle(), "ASF_Diag_Libnet_SerS");
}

bool Server::SendMessage(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& data)
{
    bool result(true);
    asf::diag::communication::TlvMsg msg(static_cast<uint32_t>(data.size() + 64U));
    msg.header().version(0x01);
    msg.header().inv_version(0xFE);
    msg.header().payload_type(type);
#ifdef USE_FS_LIBNET
    msg.content().WriteArray(&data[0], data.size());
    try {
        const std::lock_guard<std::mutex> locker(clients_pipeline_lock_);
        const auto iter = std::find(clients_pipeline_.begin(), clients_pipeline_.end(), reinterpret_cast<libnet::PipelineBase *>(client_handle));
        if (iter != clients_pipeline_.end()) {
            //iter->write(std::move(msg)).get();
            const auto pipeline = dynamic_cast<Pipeline *>(*iter);
            if (static_cast<bool>(pipeline)) {
                pipeline->Write(std::move(msg)).get();
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR << __FUNCTION__ << "exception: " << e.what();
        result = false;
    }
#else
    msg.content().writeArray(&data[0U], static_cast<uint32_t>(data.size()));
    try {
        const std::lock_guard<std::mutex> locker(clients_pipeline_lock_);
        const auto iter = std::find(clients_pipeline_.begin(),
                    clients_pipeline_.end(), reinterpret_cast<libnet::PipelineBase *>(client_handle));
        if (iter != clients_pipeline_.end()) {
            //iter->write(std::move(msg)).get();
            const auto pipeline = dynamic_cast<Pipeline *>(*iter);
            if (static_cast<bool>(pipeline)) {
                pipeline->write(std::move(msg)).get();
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR << __FUNCTION__ << "exception: " << e.what();
        result = false;
    }
#endif
    return result;
}

void Server::Stop()
{
    handle_message_callback_ = nullptr;
    handle_avaliable_callback_ = nullptr;
    io_group_.reset();
    accept_group_.reset();
#ifdef USE_FS_LIBNET
    server_.Stop();
#else
    server_.stop();
#endif
    if (wait_stop_thread_.joinable())
    {
        wait_stop_thread_.join();
    }
}

void Server::RegisterHandleMessageCallback(const HandleMessgaeCallback fun)
{
    handle_message_callback_ = fun;
}

void Server::RegisterHandleAvaliableCallback(const HandleAvaliableCallback fun)
{
    handle_avaliable_callback_ = fun;
}

void Server::HandleMessage(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& data)
{
    try
    {
        handle_message_callback_(client_handle, type, data);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "try callback error! " << e.what();
    }
}

void Server::HandleAvaliable(const uint64_t client_handle, const bool avaliable)
{
    try
    {
        handle_avaliable_callback_(client_handle, avaliable);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "try callback error! " << e.what();
    }
}

Server::Handler::Handler(Server *const host)
    : host_(host)
{
}

#ifdef USE_FS_LIBNET
void Server::Handler::Read(Context *ctx, TlvMsg &&msg)
{
    const uint8_t version = msg.header().version();
    const uint8_t inv_version = msg.header().inv_version();
    if ((version + inv_version) != 0xFF)
    {
        return;
    }
    const uint16_t payload_type = msg.header().payload_type();
    const uint32_t payload_length = msg.header().payload_length();
    std::vector<uint8_t> payload(static_cast<std::vector<uint8_t>::size_type>(payload_length));
    msg.content().ReadArray(&payload[0U], payload_length);
    host_->HandleMessage(reinterpret_cast<uint64_t>(ctx->GetPipeline()), payload_type, payload);
}

void Server::Handler::ReadException(Context *ctx, libnet::ChannelException ex)
{
    {
        std::lock_guard<std::mutex> locker(host_->clients_pipeline_lock_);
        auto iter = std::find(host_->clients_pipeline_.begin(), host_->clients_pipeline_.end(), ctx->GetPipeline());
        if (iter != host_->clients_pipeline_.end())
        {
            (void)host_->clients_pipeline_.erase(iter);
        }
        // for (auto it = host_->clients_pipeline_.begin(); it != host_->clients_pipeline_.end(); ++it)
        // {
        //     if (*it == ctx->getPipeline())
        //     {
        //         host_->clients_pipeline_.erase(it);
        //         break;
        //     }
        // }
    }
    host_->HandleAvaliable(reinterpret_cast<uint64_t>(ctx->GetPipeline()), false);
    LOG_INFO << __FUNCTION__ << "exception: " << ex.what();
}

void Server::Handler::ReadEOF(Context *ctx)
{
    LOG_INFO << __FUNCTION__ << "Pipeline: " << ctx->GetPipeline();
}

void Server::Handler::TransportActive(Context *ctx)
{
    {
        std::lock_guard<std::mutex> locker(host_->clients_pipeline_lock_);
        host_->clients_pipeline_.push_back(ctx->GetPipeline());
    }
    host_->HandleAvaliable(reinterpret_cast<uint64_t>(ctx->GetPipeline()), true);
    LOG_INFO << __FUNCTION__ << "Pipeline: " << ctx->GetPipeline();
}
#else
void Server::Handler::read(Context *const ctx, TlvMsg &&msg)
{
    const uint8_t version = msg.header().version();
    const uint8_t inv_version = msg.header().inv_version();
    if ((version + inv_version) != 0xFF)
    {
        return;
    }
    const uint16_t payload_type = msg.header().payload_type();
    const uint32_t payload_length = msg.header().payload_length();
    std::vector<uint8_t> payload(static_cast<std::vector<uint8_t>::size_type>(payload_length));
    msg.content().readArray(&payload[0U], payload_length);
    host_->HandleMessage(reinterpret_cast<uint64_t>(ctx->getPipeline()), payload_type, payload);
}

void Server::Handler::readException(Context *const ctx, libnet::ChannelException ex)
{
    {
        const std::lock_guard<std::mutex> locker(host_->clients_pipeline_lock_);
        const auto iter = std::find(host_->clients_pipeline_.begin(), host_->clients_pipeline_.end(), ctx->getPipeline());
        if (iter != host_->clients_pipeline_.end())
        {
            (void)host_->clients_pipeline_.erase(iter);
        }
        // for (auto it = host_->clients_pipeline_.begin(); it != host_->clients_pipeline_.end(); ++it)
        // {
        //     if (*it == ctx->getPipeline())
        //     {
        //         host_->clients_pipeline_.erase(it);
        //         break;
        //     }
        // }
    }
    host_->HandleAvaliable(reinterpret_cast<uint64_t>(ctx->getPipeline()), false);
    LOG_INFO << __FUNCTION__ << "exception: " << ex.what();
}

void Server::Handler::readEOF(Context *const ctx)
{
    LOG_INFO << __FUNCTION__ << "Pipeline: " << ctx->getPipeline();
}

void Server::Handler::transportActive(Context *const ctx)
{
    {
        const std::lock_guard<std::mutex> locker(host_->clients_pipeline_lock_);
        host_->clients_pipeline_.push_back(ctx->getPipeline());
    }
    host_->HandleAvaliable(reinterpret_cast<uint64_t>(ctx->getPipeline()), true);
    LOG_INFO << __FUNCTION__ << "Pipeline: " << ctx->getPipeline();
}
#endif

Server::PipelineFactory::PipelineFactory(Server *const host)
    : host_(host)
{
}

#ifdef USE_FS_LIBNET
std::shared_ptr<Server::Pipeline> Server::PipelineFactory::NewPipeline(std::shared_ptr<libnet::Channel> channel)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    std::shared_ptr<Pipeline> pipeline = std::make_shared<Pipeline>();
    (void)pipeline->AddBack(libnet::ChannelHandler(std::move(channel)));
    (void)pipeline->AddBack(libnet::LengthFieldBasedFrameDecoder(1024 * 1024 * 100 + 64U, offsetof(TlvMsgHdr, payload_length_), 4U));
    (void)pipeline->AddBack(ProtocolDecoder());
    (void)pipeline->AddBack(ProtocolEncoder());
    (void)pipeline->AddBack(Handler(host_));

    return pipeline;
}
#else
std::shared_ptr<Server::Pipeline> Server::PipelineFactory::newPipeline(std::shared_ptr<libnet::Channel> channel)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    std::shared_ptr<Pipeline> pipeline = std::make_shared<Pipeline>();
    (void)pipeline->addBack(libnet::ChannelHandler(std::move(channel)));
    (void)pipeline->addBack(libnet::LengthFieldBasedFrameDecoder(1024 * 1024 * 100 + 64U, offsetof(TlvMsgHdr, payload_length_), 4U));
    (void)pipeline->addBack(ProtocolDecoder());
    (void)pipeline->addBack(ProtocolEncoder());
    (void)pipeline->addBack(Handler(host_));

    return pipeline;
}
#endif

} //namespace communication
} //namespace diag
} //asf
