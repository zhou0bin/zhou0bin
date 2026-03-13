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

#include "dcm/connection/uds_on_neusar_net.h"

#include "asf/diag/uds_transport/protocol_handler.h"
using asf::diag::uds_transport::UdsTransportProtocolHandler;
#include "uds_transport/protocol_types_priv.h"
using asf::diag::uds_transport::UdsTransportProtocol;
using asf::diag::uds_transport::ByteVector;

#include "common/multi_byte_type.h"

#include "log.h"

using asf::diag::msg_on_neusar_net::DiagMsgType;

using InitializationResult = asf::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult;
using TransmissionResult = asf::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult;
using TargetAddressType = asf::diag::uds_transport::UdsMessage::TargetAddressType;

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

UdsOnNeusarNet::UdsOnNeusarNet(const std::string& config, uds_transport::UdsTransportProtocolMgr& transport_protocol_mgr)
    : UdsTransportProtocolHandler(UdsTransportProtocol::UDSonNeusarNet, transport_protocol_mgr)
    , diagnostic_address_(0x0000)
    , functional_address_(0x0000)
    , uds_on_neusar_net_server_(std::make_shared<communication::Server>())
    , uds_on_neusar_net_path_com_("/tmp/ASF_UDS_ON_NEUSAR_NET_Interface")
    , client_map_()
    , client_map_mutex_()
    , config_(config)
{
}

InitializationResult UdsOnNeusarNet::Initialize()
{
    std::ifstream ifs(config_);
    if (ifs.good())
    {
        rapidjson::IStreamWrapper ist(ifs);
        rapidjson::Document doc;
        (void)doc.ParseStream(ist);

        const rapidjson::Value& ecu_instance = doc["EcuInstance"];
        if (ecu_instance.HasMember("diagnosticAddress") && ecu_instance["diagnosticAddress"].IsInt())
        {
            diagnostic_address_ = static_cast<uint16_t>(ecu_instance["diagnosticAddress"].GetInt());
        }
        if (ecu_instance.HasMember("functionalAddress") && ecu_instance["functionalAddress"].IsInt())
        {
            functional_address_ = static_cast<uint16_t>(ecu_instance["functionalAddress"].GetInt());
        }
    }
    LOG_INFO << __FUNCTION__ << "ok! ";
    return InitializationResult::kInitializeOk;
}

void UdsOnNeusarNet::Start()
{
    if (unlink(uds_on_neusar_net_path_com_.c_str()) == -1)
    {
        LOG_WARN << __FUNCTION__ << "unlink Error! errno: " << strerror(errno);
    }
    LOG_ERROR << __FUNCTION__ << "service path: " << uds_on_neusar_net_path_com_;
    uds_on_neusar_net_server_->RegisterHandleAvaliableCallback([this](const uint64_t client, const bool avaliable)
                                                               { ClientAvaliable(client, avaliable); });
    uds_on_neusar_net_server_->RegisterHandleMessageCallback([this](const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t> &payload)
                                                             { ReceiveProxyMessage(client_handle, type, payload); });
    uds_on_neusar_net_server_->OfferService(uds_on_neusar_net_path_com_);
}

void UdsOnNeusarNet::Stop()
{
    try {
        uds_on_neusar_net_server_->Stop();
    } catch(...) { }
    transportprotocol_manager_.HandlerStopped(UdsTransportProtocol::UDSonNeusarNet);
}

bool UdsOnNeusarNet::NotifyReestablishment(const ChannelID channelId)
{
    LOG_DEBUG << __FUNCTION__ << "channelId: " << channelId;
    return true;
}

void UdsOnNeusarNet::ClientAvaliable(const uint64_t client, const bool avaliable)
{
    const std::lock_guard<std::mutex> locker_client{client_map_mutex_};
    if (avaliable == false)
    {
        for(auto it = client_map_.begin(); it != client_map_.end(); )
        {
            if (it->second == client)
            {
                it = client_map_.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

uint64_t UdsOnNeusarNet::GetClientBySpecifier(const uint32_t address)
{
    uint64_t client{0};
    const std::lock_guard<std::mutex> locker_client{client_map_mutex_};
    if (client_map_.find(address) != client_map_.end())
    {
        client = client_map_[address];
    }
    return client;
}

void UdsOnNeusarNet::ReceiveProxyMessage(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& payload)
{
    LOG_INFO << __FUNCTION__ << "client_handle: " << client_handle;
    if (CommMsgType::kOfferMessage == type)
    {
        if (payload.size() < 2)
        {
            LOG_ERROR << __FUNCTION__ << "payload size error! size: " << payload.size();
            return;
        }
        uint16_t address{ 0 };
        SetByte(address, payload.at(0), 1);
        SetByte(address, payload.at(1), 0);
        LOG_ERROR << __FUNCTION__ << "Offer address: " << LOG_HEX(address) << " client_handle: " << client_handle;
        const std::lock_guard<std::mutex> locker_offermsg{client_map_mutex_};
        if (client_map_[address] != client_handle)
        {
            client_map_[address] = client_handle;
        }
    }
    else if (CommMsgType::kStopOfferMessage == type)
    {
        if (payload.size() < 2)
        {
            LOG_ERROR << __FUNCTION__ << "payload size error! size: " << payload.size();
            return;
        }
        uint16_t address{ 0 };
        SetByte(address, payload.at(0), 1);
        SetByte(address, payload.at(1), 0);
        LOG_ERROR << __FUNCTION__ << "StopOffer address: " << LOG_HEX(address) << " client_handle: " << client_handle;
        const std::lock_guard<std::mutex> locker_stopoffermsg{client_map_mutex_};
        if (client_map_.find(address) != client_map_.end())
        {
            (void)client_map_.erase(address);
        }
    }
    else
    {
        switch (type)
        {
            case DiagMsgType::kDiagnosticMessage:
            {
                if (payload.size() < 2)
                {
                    LOG_ERROR << __FUNCTION__ << "payload size error! size: " << payload.size();
                    return;
                }
                uint16_t address{ 0 };
                SetByte(address, payload.at(0), 1);
                SetByte(address, payload.at(1), 0);
                {
                    const std::lock_guard<std::mutex> locker_msg{client_map_mutex_};
                    if (client_map_.find(address) == client_map_.end())
                    {
                        client_map_[address] = client_handle;
                    }
                }
                LOG_ERROR << __FUNCTION__ << "DiagnosticMessage address: " << LOG_HEX(address) << " client_handle: " << client_handle;
                HandleDiagnosticMessage(std::move(payload));
                break;
            }
            default:
            {
                LOG_ERROR << __FUNCTION__ << "unknow type! ";
                break;
            }
        }
    }
}

void UdsOnNeusarNet::HandleDiagnosticMessage(const std::vector<uint8_t>& message)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    uint16_t source_address{0};
    uint16_t target_address{0};
    if (message.size() > 3)
    {
        SetByte(source_address, message.at(0), 1);
        SetByte(source_address, message.at(1), 0);
        SetByte(target_address, message.at(2), 1);
        SetByte(target_address, message.at(3), 0);
    }

    const ChannelID channelId{source_address};
    const std::size_t size = message.size();
    if (size < 5)
    {
        LOG_ERROR << __FUNCTION__ << "message size < 5! size: " << size;
        TransmitNack(diagnostic_address_, source_address, uds_on_neusar_net::UdsNack::kUdsTransportProtocolError, channelId);
        return;
    }

    TargetAddressType addressingType {TargetAddressType::kPhysical};
    if (target_address == functional_address_)
    {
        addressingType = TargetAddressType::kFunctional;
    }

    const uds_transport::UdsTransportProtocolMgr::GlobalChannelIdentifier globalChannelID = std::make_tuple(GetHandlerID(), channelId);
    LOG_DEBUG << __FUNCTION__ << "ChannelID: " << std::get<0>(globalChannelID) << "," << std::get<1>(globalChannelID);

    // Indicat Message
    auto IndicateResult = transportprotocol_manager_.IndicateMessage(source_address, target_address, addressingType, globalChannelID,
                                                                     size, 0 /*priority*/, "" /*protocolKind*/, {} /*payloadInfo*/);

    if (IndicateResult.first != uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk)
    {
        LOG_ERROR << __FUNCTION__ << "IndicateMessage failed! errno: " << static_cast<unsigned>(IndicateResult.first);
        TransmitNack(diagnostic_address_, source_address, uds_on_neusar_net::UdsNack::kUdsOutOfMemoryToStore, channelId);
        return;
    }

    uds_transport::UdsMessageExtPtr UdsMsgPtr = std::move(IndicateResult.second);

    // LOG_INFO << __FUNCTION__ << "IndicateMessage OK! ";
    // TransmitAck(diagnostic_address_, source_address, channelId);

    // Handle Message
    uds_transport::ByteVector& UdsMsgPayload = UdsMsgPtr->GetPayload();
    (void)UdsMsgPayload.assign(message.begin() + 4, message.end());
    transportprotocol_manager_.HandleMessage(std::move(UdsMsgPtr));
}

void UdsOnNeusarNet::Transmit(UdsMessageConstPtr const message, const ChannelID channelId)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto client_handle = GetClientBySpecifier(channelId);
    if (!static_cast<bool>(client_handle))
    {
        LOG_ERROR << __FUNCTION__ << "GetClientBySpecifier failed! channelId: " << channelId;
        return;
    }
    const uint16_t source_address = message->GetSa();
    const uint16_t target_address = message->GetTa();
    const std::vector<uint8_t> payload =  message->GetPayload();
    std::vector<uint8_t> buffer {
        GetByte(source_address, 1),
        GetByte(source_address, 0),
        GetByte(target_address, 1),
        GetByte(target_address, 0)};
    (void)buffer.insert(buffer.cend(), payload.begin(), payload.end());
    const bool send_result = uds_on_neusar_net_server_->SendMessage(client_handle, DiagMsgType::kDiagnosticMessage, buffer);
    if (!send_result)
    {
        LOG_ERROR << __FUNCTION__ << "size: " << payload.size() << "payload: " << LOG_RAW_BUFFER(payload);
    }
    LOG_ERROR << __FUNCTION__ << "Transmit Message Result: " << send_result << "address: " << LOG_HEX(static_cast<uint16_t>(channelId)) << "client_handle: " << client_handle;
    const TransmissionResult rst = ((send_result == true) ? TransmissionResult::kTransmitOk : TransmissionResult::kTransmitFailed);
    transportprotocol_manager_.TransmitConfirmation(rst);
}

void UdsOnNeusarNet::TransmitAck(const uint16_t sa, const uint16_t ta, const ChannelID channelId)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto client_handle = GetClientBySpecifier(channelId);
    if (!static_cast<bool>(client_handle))
    {
        LOG_ERROR << __FUNCTION__ << "GetClientBySpecifier failed! channelId: " << channelId;
        return;
    }
    const std::vector<uint8_t> buffer {
        GetByte(sa, 1),
        GetByte(sa, 0),
        GetByte(ta, 1),
        GetByte(ta, 0),
        0x00};
    const bool send_result = uds_on_neusar_net_server_->SendMessage(client_handle, DiagMsgType::kDiagnosticAck, buffer);
    LOG_DEBUG << __FUNCTION__ << "Transmit Message Result: " << send_result << "address: " << LOG_HEX(static_cast<uint16_t>(channelId)) << "client_handle: " << client_handle;
}

void UdsOnNeusarNet::TransmitNack(const uint16_t sa, const uint16_t ta, const uint8_t nack_code, const ChannelID channelId)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto client_handle = GetClientBySpecifier(channelId);
    if (!static_cast<bool>(client_handle))
    {
        LOG_ERROR << __FUNCTION__ << "GetClientBySpecifier failed! channelId: " << channelId;
        return;
    }
    const std::vector<uint8_t> buffer {
        GetByte(sa, 1),
        GetByte(sa, 0),
        GetByte(ta, 1),
        GetByte(ta, 0),
        nack_code};
    const bool send_result = uds_on_neusar_net_server_->SendMessage(client_handle, DiagMsgType::kDiagnosticNack, buffer);
    LOG_DEBUG << __FUNCTION__ << "Transmit Message Result: " << send_result << "address: " << LOG_HEX(static_cast<uint16_t>(channelId)) << "client_handle: " << client_handle;
}

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
