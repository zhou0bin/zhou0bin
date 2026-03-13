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

#include "communication/diag_libnet_client.h"

#include "communication/diag_libnet_protocol.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace communication
{

Client::Client()
{
#ifdef USE_FS_LIBNET
    (void)client_.Group(std::make_shared<wheels::IOThreadPoolExecutor>(1));
    (void)client_.SetPipelineFactory(std::make_shared<PipelineFactory>(this));
#else
    (void)client_.group(std::make_shared<wheels::IOThreadPoolExecutor>(1));
    (void)client_.pipelineFactory(std::make_shared<PipelineFactory>(this));
#endif
}

void Client::Stop()
{
    handle_message_callback_ = nullptr;
    handle_avaliable_callback_ = nullptr;
#ifdef USE_FS_LIBNET
    const auto pipeline = client_.GetPipeline();
    if (static_cast<bool>(pipeline))
    {
        (void)pipeline->Close();
    }
#else
    const auto pipeline = client_.getPipeline();
    if (static_cast<bool>(pipeline))
    {
        (void)pipeline->close();
    }
#endif
}

bool Client::FindService(const std::string& interface, const HandleAvaliableCallback fun)
{
    bool result(false);
    try {
        RegisterHandleAvaliableCallback(fun);
#ifdef USE_FS_LIBNET
        Pipeline * const pipeline = client_.Connect(libnet::SocketAddress(interface)).get();
#else
        Pipeline * const pipeline = client_.connect(libnet::SocketAddress(interface)).get();
#endif
        if (static_cast<bool>(pipeline))
        {
            result = true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << __FUNCTION__ << "exception: " << e.what();
        HandleAvaliable(false);
    }
    return result;
}

bool Client::SendMessage(const uint16_t type, const std::vector<uint8_t>& data)
{
    bool result(true);
    asf::diag::communication::TlvMsg msg(static_cast<uint32_t>(data.size() + 64U));
    msg.header().version(0x01U);
    msg.header().inv_version(0xFEU);
    msg.header().payload_type(type);
#ifdef USE_FS_LIBNET
    try {
        msg.content().WriteArray(&data[0], data.size());
        if (client_.GetPipeline() != nullptr)
        {
            client_.GetPipeline()->Write(std::move(msg)).get();
        }
    } catch (const std::exception &e) {
        LOG_ERROR << __FUNCTION__ << "exception: " << e.what();
        result = false;
    }
#else
    try {
        msg.content().writeArray(&data[0U], static_cast<uint32_t>(data.size()));
        if (client_.getPipeline() != nullptr)
        {
            client_.getPipeline()->write(std::move(msg)).get();
        }
    } catch (const std::exception &e) {
        LOG_ERROR << __FUNCTION__ << "exception: " << e.what();
        result = false;
    }
#endif
    return result;
}

void Client::RegisterHandleMessageCallback(const HandleMessgaeCallback fun)
{
    handle_message_callback_ = fun;
}

void Client::RegisterHandleAvaliableCallback(const HandleAvaliableCallback fun)
{
    handle_avaliable_callback_ = fun;
}

void Client::HandleMessage(const uint16_t type, const std::vector<uint8_t>& data)
{
    try
    {
        handle_message_callback_(type, data);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "try callback error! " << e.what();
    }
}

void Client::HandleAvaliable(const bool avaliable)
{
    try
    {
        handle_avaliable_callback_(avaliable);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR << __FUNCTION__ << "try callback error! " << e.what();
    }
}

Client::Handler::Handler(Client * const host)
    : host_(host)
{
}

#ifdef USE_FS_LIBNET
void Client::Handler::Read(Context *ctx, TlvMsg &&msg)
{
    if (ctx->GetPipeline() == host_->client_.GetPipeline())
    {
        const uint16_t payload_type = msg.header().payload_type();
        const uint32_t payload_length = msg.header().payload_length();
        std::vector<uint8_t> payload(static_cast<std::vector<uint8_t>::size_type>(payload_length));
        msg.content().ReadArray(&payload[0U], payload_length);
        host_->HandleMessage(payload_type, payload);
    }
    else
    {
        LOG_WARN << __FUNCTION__ << "Pipeline mismatch! ";
    }
}

void Client::Handler::ReadException(Context* ctx, libnet::ChannelException ex)
{
    if (ctx->GetPipeline() == host_->client_.GetPipeline())
    {
        host_->HandleAvaliable(false);
    }
    LOG_INFO << __FUNCTION__ << "exception: " << ex.what();
}

void Client::Handler::ReadEOF(Context* ctx)
{
    LOG_INFO << __FUNCTION__ << "Pipeline: " << ctx->GetPipeline();
}

void Client::Handler::TransportActive(Context* ctx)
{
    if (ctx->GetPipeline() == host_->client_.GetPipeline())
    {
        host_->HandleAvaliable(true);
    }
    LOG_INFO << __FUNCTION__ << "Pipeline: " << ctx->GetPipeline();
}
#else
void Client::Handler::read(Context* const ctx, TlvMsg&& msg)
{
    if (ctx->getPipeline() == host_->client_.getPipeline())
    {
        const uint16_t payload_type = msg.header().payload_type();
        const uint32_t payload_length = msg.header().payload_length();
        std::vector<uint8_t> payload(static_cast<std::vector<uint8_t>::size_type>(payload_length));
        msg.content().readArray(&payload[0U], payload_length);
        host_->HandleMessage(payload_type, payload);
    }
    else
    {
        LOG_WARN << __FUNCTION__ << "Pipeline mismatch! ";
    }
}

void Client::Handler::readException(Context *const ctx, libnet::ChannelException ex)
{
    if (ctx->getPipeline() == host_->client_.getPipeline())
    {
        host_->HandleAvaliable(false);
    }
    LOG_INFO << __FUNCTION__ << "exception: " << ex.what();
}

void Client::Handler::readEOF(Context *const ctx)
{
    LOG_INFO << __FUNCTION__ << "Pipeline: " << ctx->getPipeline();
}

void Client::Handler::transportActive(Context *const ctx)
{
    if (ctx->getPipeline() == host_->client_.getPipeline())
    {
        host_->HandleAvaliable(true);
    }
    LOG_INFO << __FUNCTION__ << "Pipeline: " << ctx->getPipeline();
}
#endif

Client::PipelineFactory::PipelineFactory(Client *const host)
    : host_(host)
{
}

#ifdef USE_FS_LIBNET
std::shared_ptr<Client::Pipeline> Client::PipelineFactory::NewPipeline(std::shared_ptr<libnet::Channel> channel)
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
std::shared_ptr<Client::Pipeline> Client::PipelineFactory::newPipeline(std::shared_ptr<libnet::Channel> channel)
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
