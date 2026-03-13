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

#ifndef SOURCE_DCM_CONNECTION_DO_IP_HANDLER_H_
#define SOURCE_DCM_CONNECTION_DO_IP_HANDLER_H_

#include <atomic>
#include <cstddef>
#include <cstring>
#include <memory>
#include <thread>
#include <cstdint>

#include "dcm/connection/do_ip_connection_manager.h"
#include "dcm/connection/do_ip_udp_handler.h"

#include "asf/diag/uds_transport/protocol_handler.h"
#include "asf/diag/uds_transport/protocol_mgr.h"
#include "asf/diag/uds_transport/protocol_types.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

/**
 * \brief The maximum number of pending Tcp connection requests, before
 * additional requests are rejected.
 */
static const unsigned int kDoIp_Tcp_maxQueueLength(3U);

/**
 * Uds Protocol Handler for ISO13400 DoIp
 *
 */
class DoIpHandler : public asf::diag::uds_transport::UdsTransportProtocolHandler
{
public:
    using UdsMessageConstPtr = uds_transport::UdsMessageConstPtr;
    using ChannelID = uds_transport::ChannelID;
protected:

    /**
     * \brief Container to hold all threads spawned by the DCM.
     */
    std::vector<std::thread> threads_;

    /**
     * \brief factory to manage doip connection objects owned by the DoIpHandler
     */
    std::unique_ptr<DoIpConnectionManager> doip_connection_mgr_;

    ///
    /// @brief DoIpUdpHandler
    ///
    std::unique_ptr<DoIpUdpHandler> udp_handler_;

    /**
     * \brief Flag to indicate that this UdsTransportProtocolHandler was asked to terminate.
     */
    std::atomic_bool exit_requested_;

    /**
     * \brief The local IP to listen on.
     */
    std::string local_ip_;

    /**
     * \brief TCP port
     */
    unsigned tcp_port_;

    /**
     * \brief UDP port
     */
    unsigned udp_port_;

    /**
     * \brief The Tcp listen socket.
     */
    int tcp_socket_;

    /**
     * \brief The Udp socket fpr DoIp Control Mesages.
     */
    int udp_socket_;

    /**
     * \brief Multicast IP.
     */
    std::string multicast_ip_;

    std::vector<ChannelID> channel_state_notify_tbl_;

private:
    std::string config_;

public:
    ///
    /// @brief Constructor for a new DoIpHandler.
    ///
    /// @param[in] config The config object.
    ///
    explicit DoIpHandler(const std::string& config, uds_transport::UdsTransportProtocolMgr& transport_protocol_mgr);
    explicit DoIpHandler(DoIpHandler&&) = delete;
    DoIpHandler(DoIpHandler&) = delete;
    DoIpHandler& operator=(DoIpHandler&) = delete;
    DoIpHandler& operator=(DoIpHandler&&) = delete;
    virtual ~DoIpHandler() override = default;

    virtual InitializationResult Initialize() override;

    /**
     * \brief Configure the Tcp and Udp sockets.
     *
     * This method will launch additional threads for listening for Tcp connection
     * and Udp datagrams, respectively.
     */
    virtual void Start() override;

    /**
     * \brief Shut down all communication.
     *
     * This method will signal a termination request to all subordinate classes,
     * close all sockets and wake all threads
     * waiting on the to-be-closed sockets.
     */
    virtual void Stop() override;

    virtual bool NotifyReestablishment(const ChannelID channelId) override;

    virtual void Transmit(UdsMessageConstPtr const message, const ChannelID channelId) override;

    virtual void TransmitAck(const uint16_t sa, const uint16_t ta, const ChannelID channelId) override;

    virtual void TransmitNack(const uint16_t sa, const uint16_t ta, const uint8_t nack_code, const ChannelID channelId) override;

protected:
    /**
     * \brief Entry-Point for the Thread waiting for Tcp connections.
     *
     * This method may create a new Connection object when no Connection object
     * for the origin of the Tcp connection can
     * be found.
     */
    void TcpHandler();

    /**
     * \brief Entry-Point for the Thread waiting for Udp Datagrams.
     *
     * This method may create a new Connection object when no Connection object
     * for the sender of a datagram can be found.
     */
    void UdpHandler();

    ///
    /// @brief Sends the Vehicle Announcement Message as specified as defined
    ///        by DoIp 050 of ISO 13400-2:2012
    ///
    void SendVehicleAnnouncementWhenSocketIsBound();

    void RecvUDSMessageCallback(const ChannelID channelId, const uds_transport::ByteVector& payload);

    void ConnectionStateChanged(const ChannelID channelId, const bool state);

    //Find local_ip in the local network interface
    bool FindInterfaceIP(const struct in_addr local_ip, std::string& ifa_name);

};

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_CONNECTION_DO_IP_HANDLER_H_
