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

#include "dcm/connection/do_ip_connection.h"

#include <arpa/inet.h>
#include <netinet/in.h>

#include "dcm/connection/do_ip_communication.h"
#include "dcm/connection/do_ip_connection_manager.h"
#include "dcm/connection/do_ip_constants.h"

#include "common/multi_byte_type.h"

#include "asf/diag/uds_transport/protocol_types.h"
using asf::diag::uds_transport::ByteVector;
#include "uds_transport/protocol_types_priv.h"
using asf::diag::uds_transport::UdsTransportProtocol;
#include "asf/diag/uds_transport/uds_message.h"
using asf::diag::uds_transport::UdsMessage;
#include "uds_transport/uds_message_ext.h"
using asf::diag::uds_transport::UdsMessageExt;

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

DoIpConnection::DoIpConnection(const int connection_id,
    const struct in_addr local_addr,
    const struct in_addr remote_addr,
    DoIpConnectionManager& connManager,
    const TransferUpperHandler TransferUpperLeverHandler,
    const ConnectionStateHandler StateChangedHandler)
    : Connection(connection_id, UdsTransportProtocol::DoIp)
    , tcpConnectionState_(DoIpConnectionState::kListen)
    , tcp_connect_state_mutex_()
    , local_addr_(local_addr)
    , remote_addr_(remote_addr)
    , connManager_(connManager)
    , tcp_socket_(-1)
    , tcp_handler_thread_()
    , source_addr_(0)
    , source_addr_mutex_()
    , aliveCheckPromise_()
    , aliveCheckMutex_()
    , aliveCheckTimer_(nullptr)
    , generalinactivityTimer_(nullptr)
    , initialinactivityTimer_(nullptr)
    , TransferUpperLeverHandler_(TransferUpperLeverHandler)
    , ConnectionStateHandler_(StateChangedHandler)
{
    aliveCheckTimer_ = std::make_shared<common::WheelsTimer>(connManager_.GetAliveCheckChannel(), [this](){ this->AliveCheckTimeout(); });
    generalinactivityTimer_ = std::make_shared<common::WheelsTimer>(connManager_.GetGeneralInactivityChannel(), [this](){ this->GeneralInactivityTimeout(); });
    initialinactivityTimer_ = std::make_shared<common::WheelsTimer>(connManager_.GetInitialInactivityChannel(), [this](){ this->InitialInactivityTimeout(); });
}

void DoIpConnection::Stop()
{
    this->Cancel();
    if (tcp_handler_thread_.joinable()) {
        // in case port is shutdown elsewhere; ReceiveMessages would detect it and set tcp_socket_ to -1, we would then
        // need to join the thread, otherwise the thread destructor will raise an exception
        tcp_handler_thread_.join();
    }
    if (aliveCheckTimer_ != nullptr)
    {
        aliveCheckTimer_ = nullptr;
    }
    if (generalinactivityTimer_ != nullptr)
    {
        generalinactivityTimer_ = nullptr;
    }
    if (initialinactivityTimer_ != nullptr)
    {
        initialinactivityTimer_ = nullptr;
    }
}

void DoIpConnection::Cancel()
{
    if (aliveCheckTimer_->IsRunning())
    {
        aliveCheckTimer_->Stop();
        try {
            aliveCheckPromise_.set_value(false);
        } catch (...) { }
    }
    if (generalinactivityTimer_->IsRunning())
    {
        generalinactivityTimer_->Stop();
    }
    if (initialinactivityTimer_->IsRunning())
    {
        initialinactivityTimer_->Stop();
    }
    if (GetTcpSocket() != -1)
    {
        (void)shutdown(GetTcpSocket(), SHUT_RDWR);
        (void)close(GetTcpSocket());
        SetTcpSocket(-1);
    }
    if (GetSourceAddress() != 0)
    {
        SetSourceAddress(0);
    }
    if (GetConnectionState() != DoIpConnectionState::kListen)
    {
        SetConnectionState(DoIpConnectionState::kListen);
        ConnectionStateHandler_(GetConnectionID(), false);
    }
}

void DoIpConnection::AddTcpSocket(const int socket)
{
    // to check if socket already assigned which would mean that another connection is being setup before this one is
    // closed, Cancel would then close and shutdown the socket and join the thread

    // this connection replacement mechanism could make an ongoing conversation respond on the newly created
    // socket and will for sure make it inherit its securityAccess and SessionLevel, therefore:
    // TODO(NA) needs to be revisited if/when security becomes a concern
    this->Cancel();
    {
        // We need to take care of:
        // 1- Self-preemption: cannot happen because this function is only called from the TcpHandler

        // 3- Preemption of Cancel when shutdown is requested:
        //      cannot happen because handler is stopped first, which closes TcpHandler thread (see
        //      DoIpConnection::Stop)
        //      which is the only place where this function is called, then connection manager calls Cancel for all
        //      connections
        SetTcpSocket(socket);
        if (tcp_handler_thread_.joinable()) {
            // in case port is shutdown elsewhere; ReceiveMessages would detect it and set tcp_socket_ to -1, we would
            // then need to join the thread before reconstructing it.
            tcp_handler_thread_.join();
        }

        SetConnectionState(DoIpConnectionState::kInitialized);

        // DoIP-127, ISO13400-2:2012
        if (initialinactivityTimer_->IsRunning())
        {
            initialinactivityTimer_->Stop();
        }
        (void)initialinactivityTimer_->StartOnce(std::chrono::seconds(doip::kTcpInitialInactivityTimeout));
        tcp_handler_thread_ = std::thread(&DoIpConnection::ReceiveMessages, this);
        //tcp_handler_thread_.detach();
    }
}

void DoIpConnection::ReceiveMessages()
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (GetTcpSocket() < 0) {
        LOG_ERROR << __FUNCTION__ << "No Tcp Socket set! ";
        return;
    }
    LOG_ERROR << __FUNCTION__ << "Starting TcpHandler (Recv). ";
    bool is_socket_open{true};
    while (is_socket_open) {
        // Attempt to read a DoIpPacket. SocketRead* will fall through if is_socket_open is set to false at any point.
        DoIpPacket doip_request_message(DoIpPacket::kNetwork, connManager_.GetLogicalAddress());

        // Payload size is not known in advance, so read header-only.
        try {
            DoIpCommunication::SocketRead(GetTcpSocket(), doip_request_message, is_socket_open, false);
        }
        catch(std::runtime_error& e) {
            LOG_ERROR << __FUNCTION__ << "runtime_error: " << e.what();
            //return;
            continue;
        }
        //restart general inactivity Timer when socket read
        if ((is_socket_open) && (GetConnectionState() == DoIpConnectionState::kRegistered))
        {
            if (generalinactivityTimer_->IsRunning())
            {
                generalinactivityTimer_->Stop();
            }
            (void)generalinactivityTimer_->StartOnce(std::chrono::minutes(doip::kTcpGeneralInactivityTimeout));
        }

        // DoIP-041, ISO13400-2:2012
        if (is_socket_open && (DoIpCommunication::CheckGenericDoIPHeaderVersion(doip_request_message) == false)) {
            LOG_WARN << __FUNCTION__ << "DoIP Header Version error. ";
            DoIpPacket doip_response_message{doip_request_message.ConstructNack(DoIpNackCodes::kIncorrectPatternFormat)};
            DoIpCommunication::SocketWrite(GetTcpSocket(), doip_response_message, is_socket_open, nullptr);
            is_socket_open = false;
        }
        // DoIP-042, ISO13400-2:2012
        if (is_socket_open) {
            const DoIpPacket::PayloadType type = doip_request_message.payload_type_;
            if ((DoIpPayloadType::kGenericDoIpNack == type)
             || (DoIpPayloadType::kAliveCheckResponse == type)
             || (DoIpPayloadType::kRoutingActivationRequest == type)
             || (DoIpPayloadType::kDiagnosticMessage == type)) {
                LOG_INFO << __FUNCTION__ << "Payload Type: " << LOG_HEX(type);
            } else {
                LOG_WARN << __FUNCTION__ << "Payload Type error. ";
                DoIpPacket doip_response_message{doip_request_message.ConstructNack(DoIpNackCodes::kUnknownPayloadType)};
                DoIpCommunication::SocketWrite(GetTcpSocket(), doip_response_message, is_socket_open, nullptr);
                DoIpCommunication::SocketReadPayloadDiscard(GetTcpSocket(), doip_request_message.payload_length_, is_socket_open);
                continue;
            }
        }


        if ((is_socket_open) && (doip_request_message.payload_length_ != 0)) {
            // DoIP-043, ISO13400-2:2012
            if (doip_request_message.payload_length_ > connManager_.GetMaxDataSize()) {
                LOG_WARN << __FUNCTION__ << "DoIP Payload Length > MaxDataSize error. ";
                DoIpPacket doip_response_message{doip_request_message.ConstructNack(DoIpNackCodes::kMessageTooLarge)};
                DoIpCommunication::SocketWrite(GetTcpSocket(), doip_response_message, is_socket_open, nullptr);
                DoIpCommunication::SocketReadPayloadDiscard(GetTcpSocket(), doip_request_message.payload_length_, is_socket_open);
                continue;
            }
            // DoIP-044, ISO13400-2:2012
            try {
                doip_request_message.SetPayloadLength(doip_request_message.payload_length_, true);
            }
            catch(std::bad_alloc& e) {
                LOG_ERROR << __FUNCTION__ << "bad_alloc! ";
                DoIpPacket doip_response_message{doip_request_message.ConstructNack(DoIpNackCodes::kOutOfMemory)};
                DoIpCommunication::SocketWrite(GetTcpSocket(), doip_response_message, is_socket_open, nullptr);
                DoIpCommunication::SocketReadPayloadDiscard(GetTcpSocket(), doip_request_message.payload_length_, is_socket_open);
                continue;
            }
            DoIpCommunication::SocketReadPayload(GetTcpSocket(), doip_request_message, is_socket_open);
        }

        // DoIP-045, ISO13400-2:2012
        if (is_socket_open && (DoIpCommunication::CheckPayloadTypeSpecificLength(doip_request_message) == false)) {
            LOG_WARN << __FUNCTION__ << "DoIP Payload Specific Length error. ";
            DoIpPacket doip_response_message{doip_request_message.ConstructNack(DoIpNackCodes::kInvalidPayloadLength)};
            DoIpCommunication::SocketWrite(GetTcpSocket(), doip_response_message, is_socket_open, nullptr);
            is_socket_open = false;
        }
        
        if (is_socket_open) {
            switch (static_cast<int>(doip_request_message.payload_type_)) {
            case DoIpPayloadType::kDiagnosticMessage: {
                if (GetConnectionState() == DoIpConnectionState::kRegistered)
                {
                    uint16_t source_address{0};
                    SetByte(source_address, doip_request_message.payload_.at(0), 1);
                    SetByte(source_address, doip_request_message.payload_.at(1), 0);
                    if (GetSourceAddress() != source_address)
                    {
                        LOG_WARN << __FUNCTION__ << "DiagnosticMessage sa error. ";
                        DoIpPacket doip_response_message = doip_request_message.ConstructDiagNack(
                            DoIpDiagMessageNackCodes::kInvalidSourceAddress, connManager_.GetLogicalAddress(), source_address);
                        DoIpCommunication::SocketWrite(GetTcpSocket(), doip_response_message, is_socket_open, nullptr);
                        is_socket_open = false;
                    }
                    else
                    {
                        LOG_ERROR << __FUNCTION__ << "DiagnosticMessage address: " << LOG_HEX(source_address);
                        TransferUpperLeverHandler_(GetConnectionID(), doip_request_message.payload_);
                    }
                }
                break;
            }
            case DoIpPayloadType::kRoutingActivationRequest: {
                // DoIP-085, ISO13400-2:2012
                if (initialinactivityTimer_->IsRunning())
                {
                    initialinactivityTimer_->Stop();
                }
                uint16_t source_address{0};
                uint8_t activation_type{0};
                if (doip_request_message.payload_.size() > 2)
                {
                    SetByte(source_address, doip_request_message.payload_.at(0), 1);
                    SetByte(source_address, doip_request_message.payload_.at(1), 0);
                    SetByte(activation_type, doip_request_message.payload_.at(2), 0);
                }
                DoIpRoutingActivationResponseCodes ack_code = kRoutingActivationSuccessfullyActivated;
                ack_code = connManager_.ConnectionRoutingActivation(*this, source_address, activation_type);
                DoIpPacket doip_response_message = doip_request_message.ConstructRoutingActivationResponse(ack_code);
                DoIpCommunication::SocketWrite(GetTcpSocket(), doip_response_message, is_socket_open, nullptr);
                // DoIP-102, ISO13400-2:2012
                if (ack_code == DoIpRoutingActivationResponseCodes::kRoutingActivationSuccessfullyActivated)
                {
                    // DoIP-128, ISO13400-2:2012
                    if (generalinactivityTimer_->IsRunning())
                    {
                        generalinactivityTimer_->Stop();
                    }
                    (void)generalinactivityTimer_->StartOnce(std::chrono::minutes(doip::kTcpGeneralInactivityTimeout));
                }
                else
                {
                    is_socket_open = false;
                }
                if (is_socket_open)
                {
                    ConnectionStateHandler_(GetConnectionID(), true);
                }
                break;
            }
            case DoIpPayloadType::kAliveCheckResponse: {
                if (aliveCheckTimer_->IsRunning())
                {
                    aliveCheckTimer_->Stop();
                }
                else
                {
                    LOG_ERROR << __FUNCTION__ << "Alive Check not running. ";
                    break;
                }
                uint16_t source_address{0};
                if (doip_request_message.payload_.size() > 1)
                {
                    SetByte(source_address, doip_request_message.payload_.at(0), 1);
                    SetByte(source_address, doip_request_message.payload_.at(1), 0);
                }
                // add  check SA 
                if (GetSourceAddress() == source_address)
                {
                    SetAliveCheckResult(true);
                }
                else
                {
                    LOG_ERROR << __FUNCTION__ << "Alive check sa error! connection will be closed. ";
                    SetConnectionState(DoIpConnectionState::kFinalize);
                    is_socket_open = false;
                    SetAliveCheckResult(false);
                }
                break;
            }
            // DoIP-039, ISO13400-2:2012
            case DoIpPayloadType::kGenericDoIpNack: {
                break;
            }
            default: {
                // Create the NACK and a scatter array for sending
                LOG_WARN << __FUNCTION__ << "Unknown Payload Type. ";
                break;
                //DoIpPacket doip_response_message {
                //    doip_request_message.ConstructNack(DoIpNackCodes::kUnknownPayloadType)};
                //    DoIpCommunication::SocketWrite(GetTcpSocket(), doip_response_message, is_socket_open, nullptr);
            }
            }
        }
    }
    {
        if (generalinactivityTimer_->IsRunning())
        {
            generalinactivityTimer_->Stop();
        }
        if (initialinactivityTimer_->IsRunning())
        {
            initialinactivityTimer_->Stop();
        }
        (void)shutdown(GetTcpSocket(), SHUT_RDWR);
        (void)close(GetTcpSocket());
        SetTcpSocket(-1);
        if (GetSourceAddress() != 0)
        {
            SetSourceAddress(0);
        }
        if (GetConnectionState() != DoIpConnectionState::kListen)
        {
            SetConnectionState(DoIpConnectionState::kListen);
            ConnectionStateHandler_(GetConnectionID(), false);
        }
    }
}

bool DoIpConnection::Transmit(const UdsMessageExt& message)
{
    if (GetTcpSocket() < 0) {
        LOG_WARN << __FUNCTION__ << "Tcp socket invalid when call Transmit(const UdsMessageExt& message). ";
        return false;
    }

    // Convert UdsMessage to DoIp Packet
    const auto& payload = message.GetPayload();
    DoIpPacket doip_message{DoIpPacket::kHost, connManager_.GetLogicalAddress()};
    doip_message.SetProtocolVersion(kSupportedDoIpVersion);
    doip_message.payload_type_ = DoIpPayloadType::kDiagnosticMessage;
    doip_message.SetPayloadLength((static_cast<uint32_t>(2 * sizeof(UdsMessage::Address))) + static_cast<uint32_t>(payload.size()));

    if (doip_message.payload_.size() > 3)
    {
        doip_message.payload_[0] = GetByte(message.GetSa(), 1);
        doip_message.payload_[1] = GetByte(message.GetSa(), 0);
        doip_message.payload_[2] = GetByte(message.GetTa(), 1);
        doip_message.payload_[3] = GetByte(message.GetTa(), 0);
    }

    if (doip_message.payload_.size() > (2 * sizeof(UdsMessage::Address)))
    {
        (void)std::copy(payload.begin(), payload.end(), doip_message.payload_.begin() + (2 * sizeof(UdsMessage::Address)));
    }

    // Send DoIp Packet via the Tcp socket
    bool is_socket_open{true};
    if (GetConnectionState() == DoIpConnectionState::kRegistered)
    {
        if (generalinactivityTimer_->IsRunning())
        {
            generalinactivityTimer_->Stop();
        }
        (void)generalinactivityTimer_->StartOnce(std::chrono::minutes(doip::kTcpGeneralInactivityTimeout));
    }
    DoIpCommunication::SocketWrite(GetTcpSocket(), doip_message, is_socket_open, nullptr);
    return is_socket_open;
}

bool DoIpConnection::Transmit(DoIpPacket& doipMessage)
{
    if (GetTcpSocket() < 0)
    {
        LOG_ERROR << __FUNCTION__ << "Tcp socket invalid when call Transmit(DoIpPacket& doipMessage). ";
        return false;
    }
    // Send DoIp Packet via the Tcp socket
    bool is_socket_open{true};
    if (GetConnectionState() == DoIpConnectionState::kRegistered)
    {
        if (generalinactivityTimer_->IsRunning())
        {
            generalinactivityTimer_->Stop();
        }
        (void)generalinactivityTimer_->StartOnce(std::chrono::minutes(doip::kTcpGeneralInactivityTimeout));
    }
    DoIpCommunication::SocketWrite(GetTcpSocket(), doipMessage, is_socket_open, nullptr);
    return is_socket_open;
}

bool DoIpConnection::IsOpenSocketPresent()
{
    return (GetTcpSocket() >= 0);
}

DoIpConnectionState DoIpConnection::GetConnectionState()
{
    const std::unique_lock<std::mutex> locker_tcp_connect_sts(tcp_connect_state_mutex_);
    return tcpConnectionState_;
}

void DoIpConnection::SetConnectionState(DoIpConnectionState state)
{
    const std::unique_lock<std::mutex> locker_tcp_connect_sts(tcp_connect_state_mutex_);
    tcpConnectionState_ = state;
}

std::future<bool> DoIpConnection::PerformAliveCheck()
{
    if (GetTcpSocket() < 0) {
        LOG_ERROR << __FUNCTION__ << "Tcp socket invalid. ";
        auto error_promise = std::promise<bool>();
        error_promise.set_value(false);
        return error_promise.get_future();
    }

    if (GetConnectionState() != DoIpConnectionState::kRegistered) {
        LOG_ERROR << __FUNCTION__ << "Invalid connection state. ";
        auto error_promise = std::promise<bool>();
        error_promise.set_value(false);
        return error_promise.get_future();
    }

    if (aliveCheckTimer_->IsRunning()) {
        LOG_ERROR << __FUNCTION__ << "Alive Check already running. ";
        auto error_promise = std::promise<bool>();
        error_promise.set_value(false);
        return error_promise.get_future();
    }

    aliveCheckPromise_ = std::promise<bool>();
    auto result = aliveCheckPromise_.get_future();
    if (aliveCheckTimer_->IsRunning())
    {
        aliveCheckTimer_->Stop();
    }
    (void)aliveCheckTimer_->StartOnce(std::chrono::milliseconds(doip::kTcpAliveCheckTimeout));
    SendAliveCheckRequest();
    return result;
}

void DoIpConnection::SendAliveCheckRequest()
{
    LOG_INFO << __FUNCTION__ << "in! ";
    bool is_socket_open {true};
    DoIpPacket aliveCheckRequest(DoIpPacket::kHost, connManager_.GetLogicalAddress());
    aliveCheckRequest.SetProtocolVersion(kSupportedDoIpVersion);
    aliveCheckRequest.SetPayloadLength(0);
    aliveCheckRequest.payload_type_ = DoIpPayloadType::kAliveCheckRequest;
    DoIpCommunication::SocketWrite(GetTcpSocket(), aliveCheckRequest, is_socket_open, nullptr);
}

void DoIpConnection::SetAliveCheckResult(const bool result)
{
    const std::lock_guard<std::mutex> locker(aliveCheckMutex_);
    aliveCheckPromise_.set_value(result);
}

void DoIpConnection::AliveCheckTimeout()
{
    LOG_ERROR << __FUNCTION__ << "Alive check timeout! connection will be closed. ";
    SetConnectionState(DoIpConnectionState::kFinalize);

    // We first need to call Cancel before notifying the client, because otherwise
    // in the case that the DoIpConnection goes out of scope immediately after the future.get()
    // is called, joining the tcp thread in Cancel() called from the timer thread and  ~DoIpConnection() will compete
    // and in some cases a runtime_exception with "invalid argument" is thrown in the destructor
    Cancel();
    SetAliveCheckResult(false);
}

void DoIpConnection::GeneralInactivityTimeout()
{
    LOG_ERROR << __FUNCTION__ << "connection will be closed. ";
    SetConnectionState(DoIpConnectionState::kFinalize);
    Cancel();
}

void DoIpConnection::InitialInactivityTimeout()
{
    LOG_ERROR << __FUNCTION__ << "connection will be closed. ";
    SetConnectionState(DoIpConnectionState::kFinalize);
    Cancel();
}

void DoIpConnection::SetTcpSocket(int socket)
{
    tcp_socket_.store(socket, std::memory_order_release);
}

int DoIpConnection::GetTcpSocket()
{
    return tcp_socket_.load(std::memory_order_acquire);
}

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
