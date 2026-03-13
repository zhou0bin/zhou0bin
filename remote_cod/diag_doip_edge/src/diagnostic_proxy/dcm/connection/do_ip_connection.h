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

#ifndef SOURCE_DCM_CONNECTION_DO_IP_CONNECTION_H_
#define SOURCE_DCM_CONNECTION_DO_IP_CONNECTION_H_

#include "dcm/connection/connection.h"
#include "dcm/connection/do_ip_connection_state.h"
#include "dcm/connection/do_ip_packet.h"

#include "common/wheels_timer.h"

#include "diag_proxy_common.h"

// #include "ara/core/future.h"
// #include "ara/core/promise.h"
#include <future>

#include <netinet/in.h>

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

/// @brief forward declaration
class DoIpConnectionManager;

/**
 * \brief Implementation of DoIp as a Uds Transport Protocol
 *
 */
class DoIpConnection : public Connection
{
    using TransferUpperHandler = std::function<void (const ID id, const uds_transport::ByteVector& payload)>;
    using ConnectionStateHandler = std::function<void (const ID id, const bool state)>;

public:
    /**
     * \brief Smart Pointer Type for the DoIpConnection class.
     */
    typedef std::shared_ptr<DoIpConnection> Ptr;

    /// @brief The state of this connection as defined in ISO 13400-2 Figure 12
    DoIpConnectionState tcpConnectionState_;

protected:

    std::mutex tcp_connect_state_mutex_;

    /**
     * \brief IP of the local endpoint
     */
    const struct in_addr local_addr_;

    /**
     * \brief IP of the remote endpoint
     */
    const struct in_addr remote_addr_;

    /// @brief Connection manager
    DoIpConnectionManager& connManager_;

    /**
     * \brief The Tcp socket used for communication.
     *
     * This member is only valid after AddTcpSocket() has been called.
     */
    std::atomic<int> tcp_socket_;

    /**
     * \brief The thread spawned by accept() to handle incoming data on tcp_socket_.
     */
    std::thread tcp_handler_thread_;

    //The external test equipment Logical Address
    uint16_t source_addr_;

    std::mutex source_addr_mutex_;

    /// @brief This promise is used to create a future that is returned when calling PerformAliveCheck().
    /// The future object is notified as soon as a alive response msg is received or a timeout occurs.
    std::promise<bool> aliveCheckPromise_;

    std::mutex aliveCheckMutex_;

    /// @brief This timer is used to check whether an alive check response msg is received in time
    /// after sending an alive check request
    std::shared_ptr<common::WheelsTimer> aliveCheckTimer_;

    /// @brief This timer is used to check the network connection is broken or 
    /// the external test equipment does not send any data but does not close the TCP_DATA connection
    std::shared_ptr<common::WheelsTimer> generalinactivityTimer_;

    /// @brief This timer is used to check whether valid routing activation request msg is received in time
    std::shared_ptr<common::WheelsTimer> initialinactivityTimer_;

    TransferUpperHandler TransferUpperLeverHandler_;

    ConnectionStateHandler ConnectionStateHandler_;

public:

    /// @brief Constructor for a new DoIp Connection.
    ///
    /// @param[in] connectionID The connection ID of this doip connection.
    /// @param[in] remote_addr The remote address of this connection.
    /// @param[in] local_addr The local address of this connection.
    /// @param[in] connManager The connection manager.
    /// @param[in] config configuration object
    DoIpConnection(const int connection_id,
        const struct in_addr local_addr,
        const struct in_addr remote_addr,
        DoIpConnectionManager& connManager,
        const TransferUpperHandler TransferUpperLeverHandler = 
            [](const ID /*id*/, const uds_transport::ByteVector& /*payload*/) {},
        const ConnectionStateHandler StateChangedHandler = 
            [](const ID /*id*/, const bool /*state*/) {});
    explicit DoIpConnection(DoIpConnection&&) = delete;
    DoIpConnection(DoIpConnection&) = delete;
    DoIpConnection& operator=(DoIpConnection&) = delete;
    DoIpConnection& operator=(DoIpConnection&&) = delete;
    virtual ~DoIpConnection() = default;

    void Stop();

    /**
     * \brief Stop Processing DoIp messages incoming via Tcp.
     *
     * This method currently does nothing.
     */
    void Cancel();

    /**
     * \brief Add a Tcp socket to this connection.
     *
     * This method will throw std::runtime_error in case a Tcp socket has already been set.
     * Method is marked as virtual to allow override during testing.
     */
    void AddTcpSocket(const int socket);

    /**
     * \brief Start Processing DoIp messages incoming via Tcp.
     *
     * This method spawns a new thread.
     */
    void ReceiveMessages();

    /// @brief Transmit a Uds message via the underlying Uds Transport Protocol connection.
    ///
    /// @param[in] message The message to be transmitted.
    ///
    /// @return Returns true if a message was successfully sent, otherwise false.
    virtual bool Transmit(const asf::diag::uds_transport::UdsMessageExt& message) override;

    bool Transmit(DoIpPacket& doipMessage);

    /// @brief Getter for the IP address of the local endpoint of the Tcp connection.
    ///
    /// @return Address struct containg the local address.
    const struct in_addr& GetLocalAddr() const
    {
        return local_addr_;
    }

    /// @brief Setter for the IP address of the remote endpoint of the Tcp connection.
    ///
    /// @return Adsress struct containing the remote address.
    const struct in_addr& GetRemoteAddr() const
    {
        return remote_addr_;
    }

    /// @brief Getter for the external test equipment logical address.
    /// @return Returns source address.
    uint16_t GetSourceAddress() const
    {
        return source_addr_;
    }

    void SetSourceAddress(const uint16_t address)
    {
        source_addr_ = address;
    }

    /// @brief Check if a TCP socket is already opened.
    ///
    /// @return Returns true, if there is any opened, otherwise false.
    bool IsOpenSocketPresent();

    /// @brief Returns the current state of this connection objects TCP connection, as defined in ISO 13400-2 Figure 12
    ///
    /// @return The current connection state
    DoIpConnectionState GetConnectionState();

    void SetConnectionState(DoIpConnectionState state);

    /// @brief Sends an alive check request message to the remote address. This method may only be called on connections
    /// that are already in the registered state.
    ///
    /// @throw std::runtime_exception If either the socket of this connection is invalid or the connection is not in a
    /// registered state.
    /// @throw std::future_error If this method is called again before the last alive check is finished
    ///
    /// @return Returns whether the alive check request was successfully sent
    std::future<bool> PerformAliveCheck();

    void SetTransferUpperHandler(const TransferUpperHandler TransferUpperLeverHandler)
    {
        TransferUpperLeverHandler_ = TransferUpperLeverHandler;
    }

    void SetConnectionStateHandler(const ConnectionStateHandler StateChangedHandler)
    {
        ConnectionStateHandler_ = StateChangedHandler;
    }

protected:
    void SendAliveCheckRequest();

    void SetAliveCheckResult(const bool result);

    /// @brief Callback for the alive_check_timer that is fired when a timeout occurs
    virtual void AliveCheckTimeout();

    /// @brief Callback for the general inactivity timer that is fired when a timeout occurs
    virtual void GeneralInactivityTimeout();

    /// @brief Callback for the initial inactivity timer that is fired when a timeout occurs
    virtual void InitialInactivityTimeout();

    void SetTcpSocket(int socket);

    int GetTcpSocket();

};

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_CONNECTION_DO_IP_CONNECTION_H_
