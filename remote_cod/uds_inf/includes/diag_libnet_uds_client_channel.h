#ifndef __ASF_DIAG_UDS_INF_COMMUNICATION_DIAG_LIBNET_UDS_CLIENT_CHANNEL_H__
#define __ASF_DIAG_UDS_INF_COMMUNICATION_DIAG_LIBNET_UDS_CLIENT_CHANNEL_H__

#include <cstdint>
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "multi_byte_type.h"

#include "diag_libnet_common.h"

#include "communication/diag_libnet_uds_client.h"

#include "neusar_net/init/inc/init.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace communication
{

class UdsClientChannel
{
public:
    UdsClientChannel(const std::string& channel_path)
        : proxy_(std::make_shared<UdsClient>())
        , channel_path_(channel_path)
        , avaliable_(false)
    {
#ifdef USE_FS_LIBNET
        libnet::Init();
#else
        libnet::init();
#endif
    }

    ~UdsClientChannel()
    {
        try { proxy_->Stop(); } catch(...) { }
    }

    void SetMessageNotifier(const UdsClient::HandleMessgaeCallback fun)
    {
        proxy_->RegisterHandleMessageCallback(fun);
    }

    void SetAvaliableNotifier(const uint16_t diagnostic_address, const UdsClient::HandleAvaliableCallback fun)
    {
        const std::unique_lock<std::mutex> locker_af(avaliable_fun_mutex_);
        avaliable_fun_map_[diagnostic_address] = fun;
    }

    void Offer(const uint16_t diagnostic_address)
    {
        LOG_ERROR << __FUNCTION__ << "channel path: " << channel_path_.c_str();
        if (!avaliable_.load())
        {
            const std::unique_lock<std::mutex> locker_fs(find_server_mutex_);
            if (!avaliable_.load())
            {
                if (!proxy_->FindService(channel_path_, [this](const bool channel_avaliable){this->ChannelAvaliable(channel_avaliable);}))
                {
                    LOG_ERROR << __FUNCTION__ << "channel unavaliable! channel: " << channel_path_.c_str();
                }
            }
        }
        else
        {
            SendOffer(diagnostic_address);
        }
    }

    void SendOffer(const uint16_t diagnostic_address)
    {
        LOG_ERROR << __FUNCTION__ << "channel path: " << channel_path_.c_str();
        if (!avaliable_.load())
        {
            LOG_ERROR << __FUNCTION__ << "channel unavaliable! channel: " << channel_path_.c_str();
            return;
        }
        std::vector<uint8_t> uds{};
        uds.push_back(midware::diag::GetByte(diagnostic_address, 1U));
        uds.push_back(midware::diag::GetByte(diagnostic_address, 0U));
        if (!proxy_->SendMessage(CommMsgType::kOfferMessage, uds))
        {
            LOG_ERROR << __FUNCTION__ << "failed! diagnostic_address: " << LOG_HEX(diagnostic_address);
            return;
        }
        LOG_ERROR << __FUNCTION__ << "success! diagnostic_address: " << LOG_HEX(diagnostic_address);
    }

    void StopOffer(const uint16_t diagnostic_address)
    {
        LOG_ERROR << __FUNCTION__ << "channel path: " << channel_path_.c_str();
        if (!avaliable_.load())
        {
            LOG_ERROR << __FUNCTION__ << "channel unavaliable! channel: " << channel_path_.c_str();
            return;
        }
        std::vector<uint8_t> uds{};
        uds.push_back(midware::diag::GetByte(diagnostic_address, 1U));
        uds.push_back(midware::diag::GetByte(diagnostic_address, 0U));
        if (!proxy_->SendMessage(CommMsgType::kStopOfferMessage, uds))
        {
            LOG_ERROR << __FUNCTION__ << "failed! diagnostic_address: " << LOG_HEX(diagnostic_address);
            return;
        }
        LOG_ERROR << __FUNCTION__ << "success! diagnostic_address: " << LOG_HEX(diagnostic_address);
    }

    bool SendData(const uint16_t type, const std::vector<uint8_t>& data)
    {
        if (!avaliable_.load())
        {
            const std::unique_lock<std::mutex> locker_fs(find_server_mutex_);
            if (!avaliable_.load())
            {
                if (!proxy_->FindService(channel_path_, [this](const bool channel_avaliable){this->ChannelAvaliable(channel_avaliable);}))
                {
                    LOG_ERROR << __FUNCTION__ << "channel unavaliable! channel: " << channel_path_.c_str();
                    return false;
                }
            }
        }
        return proxy_->SendMessage(type, data);
    }

private:
    void ChannelAvaliable(const bool avaliable)
    {
        LOG_ERROR << __FUNCTION__  << channel_path_.c_str() << "avaliable: " << avaliable;
        avaliable_.store(avaliable, std::memory_order_relaxed);
        if (avaliable_.load())
        {
            const std::unique_lock<std::mutex> locker_af(avaliable_fun_mutex_);
            for (auto& fun : avaliable_fun_map_)
            {
                if (fun.second != nullptr)
                {
                    fun.second(avaliable);
                }
            }
        }
    }

private:
    std::shared_ptr<UdsClient> proxy_;

    std::string channel_path_;

    std::atomic_bool avaliable_;

    std::map<uint16_t, std::function<void(const bool )> > avaliable_fun_map_;

    std::mutex avaliable_fun_mutex_;

    std::mutex find_server_mutex_;

    std::thread thread_;

};

} //namespace communication
} //namespace diag
} //namespace asf

#endif // __ASF_DIAG_UDS_INF_COMMUNICATION_DIAG_LIBNET_UDS_CLIENT_CHANNEL_H__
