#include "ut_common.h"

#include "dcm/diagnostic_communication_manager.h"
#include "dcm/conversation/conversation_manager.h"
using namespace asf::diag::dcm::conversation;
using namespace asf::diag::dcm::route;
using namespace asf::diag::dcm::arbitration;
using namespace asf::diag::dcm::authentication;
using namespace asf::diag::uds_transport;

namespace asf
{
namespace diag
{
namespace dcm
{

DCM dcm(kPathToConfigFile);

static Stub stub;
void Stub_AddHandler(uds_transport::UdsTransportProtocolHandler* UdsTPHandlerPtr)
{
    std::cout<<"==========test Stub_AddHandler=========="<<std::endl;
}
void Stub_InitializeHandlers()
{
    std::cout<<"==========test Stub_InitializeHandlers=========="<<std::endl;
}
void Stub_StartHandlers()
{
    std::cout<<"==========test Stub_StartHandlers=========="<<std::endl;
}
void Stub_StopHandlers()
{
    std::cout<<"==========test Stub_StopHandlers=========="<<std::endl;
}
void Stub_Transmit(UdsMessageExtPtr UdsMsgExtPtr, GlobalChannelIdentifier ToChannelID, const uint16_t PayloadType)
{
    std::cout<<"==========test Stub_Transmit=========="<<std::endl;
}
void Stub_TransmitAck(const uint16_t sa, const uint16_t ta, const GlobalChannelIdentifier GlobalChannelID)
{
    std::cout<<"==========test Stub_TransmitAck=========="<<std::endl;
}
void Stub_TransmitNack(const uint16_t sa, const uint16_t ta, const uint8_t nack_code, const GlobalChannelIdentifier GlobalChannelID)
{
    std::cout<<"==========test Stub_TransmitNack=========="<<std::endl;
}

bool Stub_is_auth_service(const uint8_t sid)
{
    return true;
}

bool Stub_ConnectServer()
{   
    return true;
}

bool Stub_SendDiagnosticMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)
{   
    return true;
}

bool Stub_SendCustomMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload)
{   
    return true;
}

TEST(Test_dcm, Initialize)
{
    stub.set(ADDR(uds_transport::UdsTransportProtocolMgr, AddHandler), Stub_AddHandler);
    stub.set(ADDR(uds_transport::UdsTransportProtocolMgr, InitializeHandlers), Stub_InitializeHandlers);
    EXPECT_NO_THROW(dcm.Initialize());
}

TEST(Test_dcm, run)
{
    stub.set(ADDR(uds_transport::UdsTransportProtocolMgr, StartHandlers), Stub_StartHandlers);
    EXPECT_NO_THROW(dcm.Run());
}

TEST(Test_dcm, Shutdown)
{
    stub.set(ADDR(uds_transport::UdsTransportProtocolMgr, StopHandlers), Stub_StopHandlers);
    EXPECT_NO_THROW(dcm.Shutdown());
}

TEST(Test_dcm, HandleMessage)
{
    stub.set(ADDR(RoutingSocket, ConnectServer), Stub_ConnectServer);
    stub.set(ADDR(RoutingSocket, SendDiagnosticMessage), Stub_SendDiagnosticMessage);
    stub.set(ADDR(RoutingSocket, SendCustomMessage), Stub_SendCustomMessage);

    ConversationManager& conversation_manager = *(dcm.conversation_manager_);

    ByteVector payload = {0x10, 0x01};
    UdsMessageExtPtr message = std::make_unique<UdsMessageExt>(tester_addr, ccu, payload);
    message->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message->SetRecvChannelID({UdsTransportProtocol::DoIp, 1});
    EXPECT_NO_THROW(dcm.HandleMessage(std::move(message)));

    conversation_manager.GetAuthentication().StartAuthenticate(Conversation_ID);
    conversation_manager.GetAuthentication().HandleAuthenticationResult(0x01);

    UdsMessageExtPtr message_auth_failure = std::make_unique<UdsMessageExt>(tester_addr, ccu, payload);
    message_auth_failure->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message_auth_failure->SetRecvChannelID({UdsTransportProtocol::UDSonNeusarNet, 2});
    EXPECT_NO_THROW(dcm.HandleMessage(std::move(message_auth_failure)));

    conversation_manager.GetAuthentication().StartAuthenticate(Conversation_ID);
    conversation_manager.GetAuthentication().HandleAuthenticationResult(0x12);

    UdsMessageExtPtr message_auth_success = std::make_unique<UdsMessageExt>(tester_addr, ccu, payload);
    message_auth_success->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message_auth_success->SetRecvChannelID({UdsTransportProtocol::UDSonNeusarNet, 2});
    EXPECT_NO_THROW(dcm.HandleMessage(std::move(message_auth_success)));

    UdsMessageExtPtr message_partition = std::make_unique<UdsMessageExt>(tester_addr, ccu, payload);
    message_partition->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message_partition->SetRecvChannelID({UdsTransportProtocol::UDSonNeusarNet, 2});
    EXPECT_NO_THROW(dcm.HandleMessage(std::move(message_partition)));

    UdsMessageExtPtr message_note = std::make_unique<UdsMessageExt>(sa, adc, payload);
    message_note->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message_note->SetRecvChannelID({UdsTransportProtocol::UDSonNeusarNet, 2});
    EXPECT_NO_THROW(dcm.HandleMessage(std::move(message_note)));

    UdsMessageExtPtr message_note_pending = std::make_unique<UdsMessageExt>(tester_addr, adc, payload);
    message_note_pending->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message_note_pending->SetRecvChannelID({UdsTransportProtocol::UDSonNeusarNet, 2});
    EXPECT_NO_THROW(dcm.HandleMessage(std::move(message_note_pending)));

    UdsMessageExtPtr message_note_nrc94 = std::make_unique<UdsMessageExt>(sa, adc, payload);
    message_note_nrc94->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message_note_nrc94->SetRecvChannelID({UdsTransportProtocol::UDSonNeusarNet, 2});
    EXPECT_NO_THROW(dcm.HandleMessage(std::move(message_note_nrc94)));

    UdsMessageExtPtr message_gateway = std::make_unique<UdsMessageExt>(tester_addr, ta, payload);
    message_gateway->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message_gateway->SetRecvChannelID({UdsTransportProtocol::UDSonNeusarNet, 2});
    EXPECT_NO_THROW(dcm.HandleMessage(std::move(message_gateway)));

    UdsMessageExtPtr message_gateway_nrc94 = std::make_unique<UdsMessageExt>(sa, ta, payload);
    message_gateway_nrc94->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message_gateway_nrc94->SetRecvChannelID({UdsTransportProtocol::UDSonNeusarNet, 2});
    EXPECT_NO_THROW(dcm.HandleMessage(std::move(message_gateway_nrc94)));

    ByteVector payload_func = {0x3e, 0x80};
    UdsMessageExtPtr message_func = std::make_unique<UdsMessageExt>(tester_addr, func, payload_func);
    message_func->SetTaType(UdsMessage::TargetAddressType::kFunctional);
    message_func->SetRecvChannelID({UdsTransportProtocol::UDSonNeusarNet, 2});
    EXPECT_NO_THROW(dcm.HandleMessage(std::move(message_func)));

    UdsMessageExtPtr message_tester_undefined = std::make_unique<UdsMessageExt>(sa, ccu, payload);
    message_tester_undefined->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message_tester_undefined->SetRecvChannelID({UdsTransportProtocol::DoIp, 2});
    EXPECT_NO_THROW(dcm.HandleMessage(std::move(message_tester_undefined)));

    stub.set(ADDR(Authentication, IsAuthenticationService), Stub_is_auth_service);
    ByteVector payload_29 = {0x29, 0x01};
    UdsMessageExtPtr message_29 = std::make_unique<UdsMessageExt>(tester_addr, ccu, payload_29);
    message_29->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message_29->SetRecvChannelID({UdsTransportProtocol::DoIp, 1});
    EXPECT_NO_THROW(dcm.HandleMessage(std::move(message_29)));

    stub.reset(ADDR(RoutingSocket, SendCustomMessage));
    stub.reset(ADDR(RoutingSocket, SendDiagnosticMessage));
    stub.reset(ADDR(RoutingSocket, ConnectServer));

    std::this_thread::sleep_for(std::chrono::seconds(8));
}

TEST(Test_dcm, TransmitConfirm)
{
    EXPECT_NO_THROW(dcm.TransmitConfirm(true));
}

TEST(Test_dcm, HandleChannelDisconnected)
{
    EXPECT_NO_THROW(dcm.HandleChannelDisconnected(Channel_ID));
}

TEST(Test_dcm, HandleChannelReestablished)
{
    EXPECT_NO_THROW(dcm.HandleChannelReestablished(Channel_ID));
}

TEST(Test_dcm, DiagServerInterface)
{
    EXPECT_NO_THROW(dcm.DiagServerInterface::RequestCanBeHandled(sa, ta, TargetAddressType::kPhysical, Channel_ID, 2, 0, ""));
}

TEST(Test_dcm, Arbitration)
{
    ConversationManager& conversation_manager = *(dcm.conversation_manager_);

    ASSERT_EQ(conversation_manager.GetArbitration().Arbitrate(ta, tester_addr, 5), ArbitrateResult::kArbitratePass);
    conversation_manager.GetArbitration().ArbitrateStart(ta);
    ASSERT_EQ(conversation_manager.GetArbitration().Arbitrate(ta, tester_addr, 4), ArbitrateResult::kArbitratePending);
    ASSERT_EQ(conversation_manager.GetArbitration().Arbitrate(ta, tester_addr, 4), ArbitrateResult::kArbitrateReject);
    conversation_manager.GetArbitration().ArbitrateFinish(ta);
    ASSERT_EQ(conversation_manager.GetArbitration().Arbitrate(ta, tester_addr, 3), ArbitrateResult::kArbitratePass);
    ASSERT_EQ(conversation_manager.GetArbitration().Arbitrate(ta, tester_addr, 3), ArbitrateResult::kArbitratePass);
    ASSERT_EQ(conversation_manager.GetArbitration().Arbitrate(ta, sa, 3), ArbitrateResult::kArbitrateReject);
    ASSERT_EQ(conversation_manager.GetArbitration().Arbitrate(ta, tester_addr, 10), ArbitrateResult::kArbitrateReject);
    conversation_manager.GetArbitration().ArbitrateRelease(ta);
    ASSERT_EQ(conversation_manager.GetArbitration().Arbitrate(ta, tester_addr, 5), ArbitrateResult::kArbitratePass);
}

TEST(Test_dcm, Authentication)
{
    ConversationManager& conversation_manager = *(dcm.conversation_manager_);

    conversation_manager.GetAuthentication().StartAuthenticate(Conversation_ID);
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().Authenticate(Conversation_ID));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().Authenticate(0x00));

    conversation_manager.GetAuthentication().HandleAuthenticationResult(0xFF);
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().StartAuthenticate(Conversation_ID));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().Authenticate());
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().IsAuthenticationExemption(ccu, ccu));

    conversation_manager.GetAuthentication().HandleAuthenticationResult(0x00);
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().StartAuthenticationTimer(Conversation_ID));

    conversation_manager.GetAuthentication().HandleAuthenticationResult(0x12); 
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().StartAuthenticationTimer(0x00));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().StartAuthenticationTimer(Conversation_ID));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().StartAuthenticationTimer(Conversation_ID));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().StopAuthenticationTimer(0x00));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().StopAuthenticationTimer(Conversation_ID));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().HandleAuthenticationTimer());

    conversation_manager.GetAuthentication().ReceiveProxyMessage(Client_ID, CommMsgType::kOfferMessage, {0x01});
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().ReceiveProxyMessage(Client_ID, msg_on_neusar_net::AuthMsgType::kUpdateAuthResult, {0xFF}));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().ReceiveProxyMessage(Client_ID, msg_on_neusar_net::AuthMsgType::kUpdateAuthResult, {0x12}));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().ReceiveProxyMessage(Client_ID, msg_on_neusar_net::AuthMsgType::kUpdateAuthResult, {0x00}));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().ReceiveProxyMessage(Client_ID, msg_on_neusar_net::AuthMsgType::kUpdateAuthResult, {0x10}));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().ReceiveProxyMessage(Client_ID, msg_on_neusar_net::AuthMsgType::kUpdateAuthResult, {0xF0}));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().GetAuthenticationResult());

    conversation_manager.GetAuthentication().ReceiveProxyMessage(Client_ID, CommMsgType::kStopOfferMessage , {0x00});
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().ReceiveProxyMessage(Client_ID, 0x00, {0x00}));
    EXPECT_NO_THROW(conversation_manager.GetAuthentication().ReceiveProxyMessage(Client_ID, 0x00, {}));

    EXPECT_NO_THROW(conversation_manager.GetAuthentication().ClientAvaliable(Client_ID, false));
}

}
}
}
