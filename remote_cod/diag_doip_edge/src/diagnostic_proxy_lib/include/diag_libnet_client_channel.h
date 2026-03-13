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

#ifndef __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_CLIENT_CHANNEL_H__
#define __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_CLIENT_CHANNEL_H__

#include <unistd.h>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "diag_libnet_common.h"

#include "communication/diag_libnet_client.h"

#include "neusar_net/init/inc/init.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace communication
{

template<typename ServiceType>
class ClientChannel {
public:
    static ClientChannel& GetInstance()
    {
        static ClientChannel proxy_channel;
        return proxy_channel;
    }
    ClientChannel()
        : proxy_(std::make_shared<Client>())
        , avaliable_(false)
        , has_connect_thread_(false)
        , stop_status_(false)
    {
#ifdef USE_FS_LIBNET
        libnet::Init();
#else
        libnet::init();
#endif
    }

    ~ClientChannel()
    {
        stop_status_ = true;
        proxy_->Stop();
        if (thread_.joinable())
        {
            thread_.join();
        }
    }

    void ChannelAvaliable(const bool avaliable)
    {
        LOG_ERROR << __FUNCTION__ << "avaliable: " << avaliable;
        SetAvaliable(avaliable);
        if (IsAvaliable())
        {
            for (auto& fun : avaliable_fun_map_)
            {
                if ((fun.second) != nullptr)
                {
                    fun.second(avaliable);
                }
            }
        }
        else
        {
            if (!has_connect_thread_)
            {
                has_connect_thread_ = true;
                LOG_ERROR << __FUNCTION__ << "Reconnect start! ";
                if (!stop_status_)
                {
                    if (thread_.joinable())
                    {
                        thread_.join();
                    }
                    thread_ = std::thread([this]()
                    {
                        while(!stop_status_)
                        {
                            if (!IsAvaliable())
                            {
                                const std::unique_lock<std::mutex> locker_fs(find_server_mutex_);
                                if (!IsAvaliable())
                                {
                                    const bool find_service_status = proxy_->FindService(channel_path_,
                                                                                         [this](const bool channel_avaliable)
                                                                                         { this->ChannelAvaliable(channel_avaliable); });
                                    if (find_service_status == true)
                                    {
                                        LOG_ERROR << __FUNCTION__ << "Reconnect success! ";
                                        break;
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }
                            (void)usleep(500*1000);
                        }
                        has_connect_thread_ = false;
                    });
                	(void)pthread_setname_np(thread_.native_handle(), "Asf_diag_libnet__reconnect");
                }
            }
        }
    }

    void SetAvaliable(const bool avaliable)
    {
        avaliable_ = avaliable;
    }

    bool IsAvaliable()
    {
        return avaliable_;
    }

    void SetAvaliableNotifier(const std::string& specifier_set, const Client::HandleAvaliableCallback fun)
    {
        avaliable_fun_map_[specifier_set] = fun;
    }

    void InsertService(const std::string& specifier_insert, ServiceType* const service)
    {
        const std::lock_guard<std::mutex> locker_is(channel_map_mutex_);
        channel_map_[specifier_insert] = service;
    }

    void EraseService(const std::string& specifier_erase)
    {
        const std::lock_guard<std::mutex> locker_es(channel_map_mutex_);
        const auto it = channel_map_.find(specifier_erase);
        if (it != channel_map_.end())
        {
            (void)channel_map_.erase(channel_map_.find(specifier_erase));
        }
    }

    ServiceType* GetService(const std::string& specifier)
    {
        ServiceType* service = nullptr;
        {
            const std::unique_lock<std::mutex> locker_gs(channel_map_mutex_);
            const auto it = channel_map_.find(specifier);
            if (it != channel_map_.end())
            {
                service = channel_map_.find(specifier)->second;
            }
        }
        return service;
    }

    communication::Client::Ptr& GetProxy()
    {
        return proxy_;
    }

    int Offer(const std::string& channel_path_offer, const std::string& specifier_offer, const Client::HandleMessgaeCallback func)
    {
        LOG_ERROR << __FUNCTION__ << "channel path: " << channel_path_offer << "specifier: " << specifier_offer;
        channel_path_ = channel_path_offer;
        proxy_->RegisterHandleMessageCallback(func);
        if (!IsAvaliable())
        {
            std::unique_lock<std::mutex> locker_fs(find_server_mutex_);
            if (!IsAvaliable())
            {
                const bool find_service_status = proxy_->FindService(channel_path_offer,
                                                                     [this](const bool avaliable)
                                                                     { this->ChannelAvaliable(avaliable); });
                locker_fs.unlock();
                if (!find_service_status)
                {
                    LOG_ERROR << __FUNCTION__ << "FindService Failed! channel path: " << channel_path_offer;
                    return -1;
                }
            }
        }
        LOG_ERROR << __FUNCTION__ << "success! specifier: " << specifier_offer;
        return 0;
    }

    int SendOffer(const std::string& specifier_sendoffer)
    {
        LOG_ERROR << __FUNCTION__ << "specifier: " << specifier_sendoffer;
        std::vector<uint8_t> buffer{};
        (void)buffer.assign(specifier_sendoffer.begin(), specifier_sendoffer.end());
        const bool send_status = proxy_->SendMessage(CommMsgType::kOfferMessage, buffer);
        if (!send_status)
        {
            LOG_ERROR << __FUNCTION__ << "failed! specifier: " << specifier_sendoffer;
            return -1;
        }
        LOG_ERROR << __FUNCTION__ << "success! specifier: " << specifier_sendoffer;
        return 0;
    }

    void StopOffer(const std::string& specifier_stopoffer)
    {
        LOG_ERROR << __FUNCTION__ << "specifier: " << specifier_stopoffer;
        std::vector<uint8_t> buffer{};
        (void)buffer.assign(specifier_stopoffer.begin(), specifier_stopoffer.end());
        if (!IsAvaliable())
        {
            LOG_ERROR << __FUNCTION__ << "Service is Unavaliable! ";
            return;
        }
        const bool send_status = proxy_->SendMessage(CommMsgType::kStopOfferMessage, buffer);
        if (!send_status)
        {
            LOG_ERROR << __FUNCTION__ << "failed! specifier: " << specifier_stopoffer;
            return;
        }
        LOG_ERROR << __FUNCTION__ << "success! specifier: " << specifier_stopoffer;
    }

    int SendMessage(const std::string& channel_path_send, const std::string& specifier_send, const uint16_t type, const std::vector<uint8_t>& data)
    {
        LOG_ERROR << __FUNCTION__ << "channel path: " << channel_path_send << "specifier: " << specifier_send;
        if (!IsAvaliable())
        {
            std::unique_lock<std::mutex> locker_fs(find_server_mutex_);
            if (!IsAvaliable())
            {
                if (!has_connect_thread_)
                {
                    const bool find_service_status = proxy_->FindService(channel_path_send,
                                                                         [this](const bool avaliable)
                                                                         { this->ChannelAvaliable(avaliable); });
                    locker_fs.unlock();
                    if (!find_service_status)
                    {
                        LOG_ERROR << __FUNCTION__ << "FindService Failed! channel path: " << channel_path_send;
                        return -1;
                    }
                }
                else
                {
                    locker_fs.unlock();
                    LOG_ERROR << __FUNCTION__ << "In connect Server! channel path: " << channel_path_send;
                    return -1;
                }
            }
        }
        const bool send_status = proxy_->SendMessage(type, data);
        if (!send_status)
        {
            LOG_ERROR << __FUNCTION__ << "failed! specifier: " << specifier_send;
            return -1;
        }
        LOG_ERROR << __FUNCTION__ << "success! specifier: " << specifier_send;
        return 0;
    }

private:
    std::shared_ptr<Client> proxy_;

    std::map<std::string, ServiceType*> channel_map_;

    std::map<std::string, std::function<void(bool avaliable)> > avaliable_fun_map_;

    std::string channel_path_;

    std::atomic_bool avaliable_;

    std::atomic_bool has_connect_thread_;

    std::atomic_bool stop_status_;

    std::mutex channel_map_mutex_;

    std::mutex find_server_mutex_;

    std::thread thread_;

};

} //namespace communication
} //namespace diag
} //namespace asf

#endif // __ASF_DIAG_COMMUNICATION_DIAG_LIBNET_CLIENT_CHANNEL_H__
