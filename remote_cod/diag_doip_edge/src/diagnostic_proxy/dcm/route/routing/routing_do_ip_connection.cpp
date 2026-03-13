#include "dcm/route/routing/routing_do_ip_connection.h"

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "dcm/connection/do_ip_communication.h"
using asf::diag::dcm::connection::DoIpCommunication;
#include "dcm/connection/do_ip_packet.h"
using asf::diag::dcm::connection::DoIpPacket;
using asf::diag::dcm::connection::DoIpPayloadType;

#include "common/multi_byte_type.h"
#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace route
{

RoutingDoIpConnection::RoutingDoIpConnection(const std::string& connection_name,
                                             const std::string& remote_ip,
                                             const uint16_t remote_port,
                                             const uint16_t port,
                                             const uint16_t diagnostic_address,
                                             const uint16_t functional_address,
                                             const uint16_t logical_address,
                                             const bool routing_activation)
    : RoutingConnection(connection_name)
    , remote_ip_(inet_addr(remote_ip.c_str()))
    , remote_port_(remote_port)
    , port_(port)
    , diagnostic_address_(diagnostic_address)
    , functional_address_(functional_address)
    , logical_address_(logical_address)
    , routing_activation_(routing_activation)
    , socket_fd_(-1)
    , wait_timeout_(500)
    , connect_result_(false)
    , exit_requested_(false)
    , fds_mutex_()
    , mutex_()
    , condition_variable_()
{
}

void RoutingDoIpConnection::Offer()
{
    thread_ = std::thread([this]() { ReceiveMessage(); });
}

void RoutingDoIpConnection::StopOffer()
{
    exit_requested_.store(true, std::memory_order_release);
    if (socket_fd_ != -1)
    {
        (void)shutdown(socket_fd_, SHUT_RDWR);
        (void)close(socket_fd_);
    }
    condition_variable_.notify_all();
    if (thread_.joinable())
    {
        thread_.join();
    }
}

void RoutingDoIpConnection::SetConnectResult(const bool result)
{
    connect_result_.store(result, std::memory_order_release);
}

bool RoutingDoIpConnection::GetConnectResult()
{
    return connect_result_.load(std::memory_order_acquire);
}

bool RoutingDoIpConnection::ConnectSocket()
{
    const std::unique_lock<std::mutex> locker_fd(fds_mutex_);
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (port_ != 0)
    {
        struct sockaddr_in addr_client = {};
        addr_client.sin_family = AF_INET;           // ipv4
        addr_client.sin_port = htons(port_);        // port
        addr_client.sin_addr.s_addr = INADDR_ANY;   // ip

        struct linger so_linger;
        so_linger.l_onoff = 1;
        so_linger.l_linger = 0;
        if (setsockopt(socket_fd_, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) == -1)
        {
            LOG_ERROR << __FUNCTION__ << "setsockopt SO_LINGER failed! " << "[" << connection_name_ << "]";
        }
        if (bind(socket_fd_, reinterpret_cast<struct sockaddr*>(&addr_client), sizeof(struct sockaddr)) < 0)
        {
            LOG_ERROR << __FUNCTION__ << "bind Error! errno: " << strerror(errno) << "[" << connection_name_ << "]";
            return false;
        }
        LOG_INFO << __FUNCTION__ << "bind port: " << port_ << "[" << connection_name_ << "]";
    }

#ifdef SOCKET_KEEPALIVE
    int keepalive = 1;
    int keepidle = 30;
    int keepinterval = 2;
    int keepcount = 3;
    if (setsockopt(socket_fd_, SOL_SOCKET, SO_KEEPALIVE, static_cast<void *>(&keepalive) , sizeof(keepalive )) == -1)
    {
        LOG_ERROR << __FUNCTION__ << "SO_KEEPALIVE " << keepalive << "failed! " << "[" << connection_name_ << "]";
    }
    if (setsockopt(socket_fd_, SOL_TCP, TCP_KEEPIDLE, static_cast<void *>(&keepidle) , sizeof(keepidle )) == -1)
    {
        LOG_ERROR << __FUNCTION__ << "TCP_KEEPIDLE " << keepidle << "failed! " << "[" << connection_name_ << "]";
    }
    if (setsockopt(socket_fd_, SOL_TCP, TCP_KEEPINTVL, static_cast<void *>(&keepinterval) , sizeof(keepinterval )) == -1)
    {
        LOG_ERROR << __FUNCTION__ << "TCP_KEEPINTVL " << keepinterval << "failed! " << "[" << connection_name_ << "]";
    }
    if (setsockopt(socket_fd_, SOL_TCP, TCP_KEEPCNT, static_cast<void *>(&keepcount) , sizeof(keepcount )) == -1)
    {
        LOG_ERROR << __FUNCTION__ << "TCP_KEEPCNT " << keepcount << "failed! " << "[" << connection_name_ << "]";
    }
#endif

    struct sockaddr_in addr_server = {};
    addr_server.sin_family = AF_INET;           // ipv4
    addr_server.sin_port = htons(remote_port_); // port
    addr_server.sin_addr.s_addr = remote_ip_;   // ip

    struct timeval timeout;
    timeout.tv_sec = wait_timeout_ / 1000;
    timeout.tv_usec = (wait_timeout_ % 1000) * 1000;

    socklen_t len = sizeof(timeout);
    if (setsockopt(socket_fd_, SOL_SOCKET, SO_SNDTIMEO, &timeout, len))
    {
        LOG_ERROR << __FUNCTION__ << "setsockopt SO_SNDTIMEO failed! " << "[" << connection_name_ << "]";
    }

    LOG_ERROR << __FUNCTION__ << "connect server " << "[" << connection_name_ << "]";
    if (connect(socket_fd_, reinterpret_cast<struct sockaddr*>(&addr_server), sizeof(struct sockaddr)) < 0)
    {
        if (errno == EINPROGRESS)
        {
            LOG_ERROR << __FUNCTION__ << "connect timeout! " << "[" << connection_name_ << "]";
        }
        LOG_ERROR << __FUNCTION__ << "connect Error: " << strerror(errno) << "[" << connection_name_ << "]";
        return false;
    }
    LOG_ERROR << __FUNCTION__ << "connect server success! fd: " << socket_fd_ << "[" << connection_name_ << "]";
    return true;
}

void RoutingDoIpConnection::CloseSocket()
{
    const std::unique_lock<std::mutex> locker_fd(fds_mutex_);
    if (socket_fd_ != -1)
    {
        (void)shutdown(socket_fd_, SHUT_RDWR);
        (void)close(socket_fd_);
        socket_fd_ = -1;
    }
}

void RoutingDoIpConnection::ClearSocket()
{
    SetConnectResult(false);
    CloseSocket();
}

bool RoutingDoIpConnection::ConnectServer()
{
    if (exit_requested_.load(std::memory_order_acquire))
    {
        return false;
    }
    if (!GetConnectResult())
    {
        if (!ConnectSocket())
        {
            CloseSocket();
            return false;
        }
        else
        {
            SetConnectResult(true);
        }
        SendRoutingActivation();
    }
    condition_variable_.notify_all();
    return true;
}

void RoutingDoIpConnection::ReceiveMessage()
{
    std::unique_lock<std::mutex> locker(mutex_);
    while (!exit_requested_.load(std::memory_order_acquire))
    {
        if (!GetConnectResult())
        {
            condition_variable_.wait(locker);
            continue;
        }
        if (GetConnectResult())
        {
            locker.unlock();
            ReceiveDiagnosticMessage();
            locker.lock();
        }
    }
}

void RoutingDoIpConnection::ReceiveDiagnosticMessage()
{
    LOG_ERROR << __FUNCTION__ << "fd: " << socket_fd_ << "[" << connection_name_ << "]";
    DoIpPacket doip_packet(DoIpPacket::kHost, logical_address_);
    bool is_socket_open {true};
    try {
        DoIpCommunication::SocketRead(socket_fd_, doip_packet, is_socket_open, false);
    }
    catch(std::runtime_error& e) {
        LOG_ERROR << __FUNCTION__ << "runtime_error: " << e.what() << "[" << connection_name_ << "]";
        ClearSocket();
        return;
    }
    if (is_socket_open)
    {
        if (DoIpCommunication::CheckGenericDoIPHeaderVersion(doip_packet) == false)
        {
            LOG_ERROR << __FUNCTION__ << "recv doip header version error! " << "[" << connection_name_ << "]";
            ClearSocket();
            return;
        }
        if (DoIpCommunication::CheckPayloadType(doip_packet) == false)
        {
            LOG_ERROR << __FUNCTION__ << "recv doip payload type error! " << "[" << connection_name_ << "]";
            ClearSocket();
            return;
        }
        try {
            doip_packet.SetPayloadLength(doip_packet.payload_length_, true);
        }
        catch(std::bad_alloc& e) {
            LOG_ERROR << __FUNCTION__ << "bad_alloc! payload length: " << doip_packet.payload_length_ << "[" << connection_name_ << "]";
            DoIpCommunication::SocketReadPayloadDiscard(socket_fd_, doip_packet.payload_length_, is_socket_open);
            return;
        }
        DoIpCommunication::SocketReadPayload(socket_fd_, doip_packet, is_socket_open);
    }
    if (!is_socket_open)
    {
        LOG_ERROR << __FUNCTION__ << "socket closed! fd: " << socket_fd_ << "[" << connection_name_ << "]";
        ClearSocket();
        return;
    }
    if (doip_packet.payload_length_ < connection::kDoIp_DiagnosticMessage_length_min)
    {
        LOG_ERROR << __FUNCTION__ << "payload length error! length: " << doip_packet.payload_length_ << "[" << connection_name_ << "]";
        return;
    }
    switch (doip_packet.payload_type_)
    {
    case DoIpPayloadType::kDiagnosticMessage:
    case DoIpPayloadType::kDiagnosticAck:
    case DoIpPayloadType::kDiagnosticNack:
    {
        LOG_ERROR << __FUNCTION__ << "doip payload type: " << LOG_HEX(doip_packet.payload_type_) << "[" << connection_name_ << "]";
        HandleDiagnosticMessage(doip_packet.payload_type_, doip_packet.payload_);
        break;
    }
    case DoIpPayloadType::kCustomMessage:
    case DoIpPayloadType::kCustomNack:
    case DoIpPayloadType::kCustomCanTableSync:
    {
        HandleCustomMessage(doip_packet.payload_type_, doip_packet.payload_);
        break;
    }
    case DoIpPayloadType::kGenericDoIpNack:
    {
        LOG_ERROR << __FUNCTION__ << "recv generic doip nack: " << doip_packet.payload_.at(0) << "[" << connection_name_ << "]";
        break;
    }
    case DoIpPayloadType::kRoutingActivationResponse:
    {
        LOG_ERROR << __FUNCTION__ << "recv routing activation response. " << "[" << connection_name_ << "]";
        break;
    }
    default:
    {
        LOG_ERROR << __FUNCTION__ << "recv unknow type. type: " << LOG_HEX(doip_packet.payload_type_) << "[" << connection_name_ << "]";
        break;
    }
    }
}

void RoutingDoIpConnection::HandleDiagnosticMessage(const uint16_t payload_type, const std::vector<uint8_t>& payload)
{
    std::vector<uint8_t> msg{};
    uint16_t source_address{};
    SetByte(source_address, payload.at(0), 1);
    SetByte(source_address, payload.at(1), 0);
    uint16_t target_address{};
    SetByte(target_address, payload.at(2), 1);
    SetByte(target_address, payload.at(3), 0);
    (void)msg.insert(msg.cend(), payload.begin() + 4, payload.end());
    if (target_address != logical_address_)
    {
        LOG_ERROR << __FUNCTION__ << "target address error! target address: " << LOG_HEX(target_address) << "[" << connection_name_ << "]";
        return;
    }
    if (payload_type == DoIpPayloadType::kDiagnosticMessage)
    {
        LOG_ERROR << __FUNCTION__ << "source address: " << LOG_HEX(source_address) << "target address: " << LOG_HEX(target_address) << "payload size: " << msg.size() << "payload: " << LOG_RAW_BUFFER(msg);
    }
    HandleMessage(payload_type, source_address, target_address, msg);
}

void RoutingDoIpConnection::HandleCustomMessage(const uint16_t payload_type, const std::vector<uint8_t>& payload)
{
    std::vector<uint8_t> msg{};
    uint16_t source_address{};
    SetByte(source_address, payload.at(0), 1);
    SetByte(source_address, payload.at(1), 0);
    uint16_t target_address{};
    SetByte(target_address, payload.at(2), 1);
    SetByte(target_address, payload.at(3), 0);
    (void)msg.insert(msg.cend(), payload.begin() + 4, payload.end());
    HandleMessage(payload_type, source_address, target_address, msg);
}

void RoutingDoIpConnection::HandleMessage(const uint16_t payload_type, const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)
{
    if (callback_ != nullptr)
    {
        callback_(payload_type, source_address, target_address, payload);
    }
}

bool RoutingDoIpConnection::SendDoipMessage(const uint16_t payload_type, const uint16_t sa, const uint16_t ta, const std::vector<uint8_t>& payload)
{
    DoIpPacket doip_packet{DoIpPacket::kHost, sa};
    doip_packet.SetProtocolVersion(connection::kSupportedDoIpVersion);
    doip_packet.payload_type_ = payload_type;
    doip_packet.SetPayloadLength((static_cast<uint32_t>(2 * sizeof(uint16_t))) + static_cast<uint32_t>(payload.size()));
    doip_packet.payload_[0] = GetByte(sa, 1);
    doip_packet.payload_[1] = GetByte(sa, 0);
    doip_packet.payload_[2] = GetByte(ta, 1);
    doip_packet.payload_[3] = GetByte(ta, 0);

    (void)std::copy(payload.begin(), payload.end(), doip_packet.payload_.begin() + (2 * sizeof(uint16_t)));

    bool is_socket_open {true};
    DoIpCommunication::SocketWrite(socket_fd_, doip_packet, is_socket_open, nullptr);
    if (!is_socket_open)
    {
        LOG_ERROR << __FUNCTION__ << "socket closed! fd: " << socket_fd_ << "[" << connection_name_ << "]";
        ClearSocket();
        return false;
    }
    return true;
}

void RoutingDoIpConnection::SendRoutingActivation()
{
    if (routing_activation_)
    {
        if (!SendDoipMessage(DoIpPayloadType::kRoutingActivationRequest, logical_address_, 0x0000, {0x00, 0x00, 0x00}))
        {
            LOG_ERROR << __FUNCTION__ << "failed! " << "[" << connection_name_ << "]";
        }
    }
}

bool RoutingDoIpConnection::SendDiagnosticMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)
{
    LOG_ERROR << __FUNCTION__ << "source address: " << LOG_HEX(source_address) << "target address: " << LOG_HEX(target_address) << "payload size: " << payload.size() << "payload: " << LOG_RAW_BUFFER(payload);
    if (!SendDoipMessage(DoIpPayloadType::kDiagnosticMessage, logical_address_, target_address, payload))
    {
        LOG_ERROR << __FUNCTION__ << "failed! " << "[" << connection_name_ << "]";
        return false;
    }
    LOG_ERROR << __FUNCTION__ << "success! " << "[" << connection_name_ << "]";
    return true;
}

bool RoutingDoIpConnection::SendCustomMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)
{
    if (!SendDoipMessage(DoIpPayloadType::kCustomCanTableSync, source_address, target_address, payload))
    {
        LOG_ERROR << __FUNCTION__ << "failed! " << "[" << connection_name_ << "]";
        return false;
    }
    return true;
}

}
}
}
}
