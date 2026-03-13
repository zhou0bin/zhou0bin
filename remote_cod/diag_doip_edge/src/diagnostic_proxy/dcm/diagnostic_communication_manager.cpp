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

#include "dcm/diagnostic_communication_manager.h"

#include <iostream>
#include <cstring>
#include <memory>
#include <thread>

#include "runtime_proxy.h"

#include "uds_transport/transport_protocol_mgr.h"

#include "neusar_net/init/inc/init.h"

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{


DCM::DCM(const std::string& config)
    : DiagnosticProxy(config)
    , DiagServerInterface()
    , transportprotocolMgrPtr_(nullptr)
    , conversation_manager_(std::make_shared<conversation::ConversationManager>(*this, config))
{
}

void DCM::Initialize()
{
    LOG_ERROR << __FUNCTION__ << "in! ";

#ifdef USE_FS_LIBNET
    libnet::Init();
#else
    libnet::init();
#endif
    // Setup Tester table
    tester::TesterTable& tester_table {conversation_manager_->GetTesterTable()};

    std::ifstream ifs(this->config_);
    if (ifs.good())
    {
        rapidjson::IStreamWrapper ist(ifs);
        rapidjson::Document doc;
        (void)doc.ParseStream(ist);

        const rapidjson::Value& testers = doc["TesterTable"];
        for (uint16_t i = 0; i < testers.Size(); i++)
        {
            if ((testers[i].HasMember("TesterAddress") && testers[i]["TesterAddress"].IsInt())
             && (testers[i].HasMember("TesterPriority") && testers[i]["TesterPriority"].IsInt())
             && (testers[i].HasMember("TesterType") && testers[i]["TesterType"].IsInt()))
            {
                const auto tester_address = static_cast<uint16_t>(testers[i]["TesterAddress"].GetInt());
                const auto tester_priority = static_cast<uint8_t>(testers[i]["TesterPriority"].GetInt());
                const auto tester_type = static_cast<uint8_t>(testers[i]["TesterType"].GetInt());
                if (tester_type == TesterType::kLocal)
                {
                    tester_table.AddLocalTesterTable(tester_address, tester_priority);
                }
                else
                {
                    tester_table.AddVirtualTesterTable(tester_address, tester_priority);
                }
            }
        }
    }

    // Initialize TransportProtocolMgr
    transportprotocolMgrPtr_ = std::make_shared<uds_transport::TransportProtocolMgr>(*this, this->config_);
    transportprotocolMgrPtr_->Initialize();

    LOG_ERROR << __FUNCTION__ << "out! ";
    //OfferServices();
}

void DCM::Run()
{
    LOG_ERROR << __FUNCTION__ << "in! ";
    conversation_manager_->Start();
    transportprotocolMgrPtr_->StartHandlers();
    LOG_ERROR << __FUNCTION__ << "out! ";
}

void DCM::Shutdown()
{
    LOG_ERROR << __FUNCTION__ << "in! ";
    transportprotocolMgrPtr_->StopHandlers();
    conversation_manager_->Stop();
    LOG_ERROR << __FUNCTION__ << "out! ";
}

void DCM::HandleMessage(uds_transport::UdsMessageExtPtr UdsMsgExtPtr)
{
    LOG_ERROR << __FUNCTION__ << "in! ";
    conversation::Conversation::Ptr conversation = conversation_manager_->FindOrCreateConversation(*UdsMsgExtPtr);
    if (conversation != nullptr)
    {
        conversation->HandleMessage(std::move(UdsMsgExtPtr));
    }
    LOG_ERROR << __FUNCTION__ << "out! ";
}

void DCM::TransmitConfirm(const bool result)
{
    LOG_INFO << __FUNCTION__ << result;
}

void DCM::TransmitMsg(uds_transport::UdsMessageExt UdsMsgExt, const GlobalChannelID ToChannelID)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    transportprotocolMgrPtr_->Transmit(std::make_unique<uds_transport::UdsMessageExt>(UdsMsgExt), ToChannelID);
}

void DCM::TransmitAckMsg(const uint16_t sa, const uint16_t ta, const GlobalChannelID ToChannelID)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    transportprotocolMgrPtr_->TransmitAck(sa, ta, ToChannelID);
}

void DCM::TransmitNackMsg(const uint16_t sa, const uint16_t ta, const uint8_t nack_code, const GlobalChannelID ToChannelID)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    transportprotocolMgrPtr_->TransmitNack(sa, ta, nack_code, ToChannelID);
}

bool DCM::RegChannelStateEvent(const GlobalChannelID ChannelID)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    return transportprotocolMgrPtr_->RegisterChannelStateEvent(ChannelID);
}

void DCM::HandleChannelDisconnected(GlobalChannelID ChannelID)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    conversation_manager_->HandleChannelDisconnected(ChannelID);
}

void DCM::HandleChannelReestablished(GlobalChannelID ChannelID)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    conversation_manager_->HandleChannelReestablished(ChannelID);
}

} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
