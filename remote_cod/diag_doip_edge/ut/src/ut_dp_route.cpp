#include "ut_common.h"

#include "dcm/diagnostic_communication_manager.h"
using namespace asf::diag::dcm::route;
using namespace asf::diag::dcm::conversation;

namespace asf
{
namespace diag
{
namespace dcm
{

extern DCM dcm;

static Stub stub;

class Test_route : public testing::Test
{
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    virtual void SetUp() {}

    virtual void TearDown() {}
};

void Test_route::SetUpTestCase()
{
    std::cout<<"==========Test SetUpTestCase->Test_route=========="<<std::endl;
}

void Test_route::TearDownTestCase()
{
    std::cout<<"==========Test TearDownTestCase->Test_route=========="<<std::endl;
}

extern bool Stub_ConnectServer();

extern bool Stub_SendDiagnosticMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload);

extern bool Stub_SendCustomMessage(const uint16_t source_address, const uint16_t target_address, const std::vector<uint8_t>& payload);

TEST(Test_route, GatewayRoute)
{
    ConversationManager& conversation_manager = *(dcm.conversation_manager_);
    auto routing_table = conversation_manager.GetRouting().GetRoutingTable();

    GatewayRoute* gateway_route = reinterpret_cast<GatewayRoute*>((routing_table.GetRoute(RouteType::kGateway)).get());

    GatewayProcessor::Ptr gateway_processor = gateway_route->GetGatewayProcessor(1);

    gateway_route->GatewayRoutingUpdated(ta, 1);

    ByteVector payload = {0x10, 0x01};
    UdsMessageExtPtr message = std::make_unique<UdsMessageExt>(tester_addr, ta, payload);
    message->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message->SetRecvChannelID({UdsTransportProtocol::DoIp, 1});
    EXPECT_NO_THROW(gateway_route->CheckAndHandleMessage(Conversation_ID, 1, *message));

    stub.set(ADDR(RoutingSocket, ConnectServer), Stub_ConnectServer);
    stub.set(ADDR(RoutingSocket, SendDiagnosticMessage), Stub_SendDiagnosticMessage);
    stub.set(ADDR(RoutingSocket, SendCustomMessage), Stub_SendCustomMessage);

    EXPECT_NO_THROW(gateway_route->CheckAndHandleMessage(Conversation_ID, 1, *message));

    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kDiagnosticAck, {0x07, 0x70, 0x0e, 0x80, 0x00, 0x10}));
    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kDiagnosticMessage, {0x07, 0x70, 0x0e, 0x80, 0x7f, 0x10, 0x78}));
    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kDiagnosticMessage, {0x07, 0x70, 0x0e, 0x80, 0x7f, 0x10, 0x22}));
    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kDiagnosticMessage, {0x07, 0x70, 0x0e, 0x80, 0x7f, 0x50, 0x01, 0x13, 0x88, 0x13, 0x88}));
    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kDiagnosticNack, {0x07, 0x70, 0x0e, 0x80, 0x00, 0x10}));
    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kRoutingActivationResponse, {0x07, 0xdf, 0x0e, 0x80, 0x00}));

    ByteVector payload_3e80 = {0x3e, 0x80};
    UdsMessageExtPtr message_3e80 = std::make_unique<UdsMessageExt>(tester_addr, func, payload_3e80);
    message_3e80->SetTaType(UdsMessage::TargetAddressType::kFunctional);
    message_3e80->SetRecvChannelID({UdsTransportProtocol::UDSonNeusarNet, 2});
    EXPECT_NO_THROW(gateway_route->CheckAndHandleMessage(2, 1, *message_3e80));

    std::this_thread::sleep_for(std::chrono::seconds(3));

    ByteVector payload_3e00 = {0x3e, 0x00};
    UdsMessageExtPtr message_virtual = std::make_unique<UdsMessageExt>(tester_addr, func, payload_3e00);
    message_virtual->SetTaType(UdsMessage::TargetAddressType::kFunctional);
    message_virtual->SetRecvChannelID({UdsTransportProtocol::UDSonNeusarNet, 2});
    EXPECT_NO_THROW(gateway_route->CheckAndHandleMessage(2, 1, *message_virtual));
    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kDiagnosticAck, {0x07, 0xdf, 0x0e, 0x80, 0x00, 0x3e}));
    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kDiagnosticNack, {0x07, 0xdf, 0x0e, 0x80, 0x00, 0x3e}));

    EXPECT_NO_THROW(gateway_route->CheckAndHandleMessage(2, 1, *message_virtual));

    UdsMessageExtPtr message_physical = std::make_unique<UdsMessageExt>(tester_addr, ta, payload_3e00);
    message_physical->SetTaType(UdsMessage::TargetAddressType::kPhysical);
    message_physical->SetRecvChannelID({UdsTransportProtocol::DoIp, 1});
    message_physical->SetTesterType(TesterType::kLocal);
    message_physical->SetTesterPriority(1);
    EXPECT_NO_THROW(gateway_route->CheckAndHandleMessage(Conversation_ID, 1, *message_physical));

    EXPECT_NO_THROW(gateway_route->CheckAndHandleMessage(Conversation_ID, 1, *message_physical));

    std::this_thread::sleep_for(std::chrono::seconds(3));

    UdsMessageExtPtr message_physical_func = std::make_unique<UdsMessageExt>(tester_addr, func, payload_3e00);
    message_physical_func->SetTaType(UdsMessage::TargetAddressType::kFunctional);
    message_physical_func->SetRecvChannelID({UdsTransportProtocol::DoIp, 1});
    message_physical_func->SetTesterType(TesterType::kLocal);
    message_physical_func->SetTesterPriority(1);
    EXPECT_NO_THROW(gateway_route->CheckAndHandleMessage(Conversation_ID, 1, *message_physical_func));

    std::this_thread::sleep_for(std::chrono::seconds(3));

    ByteVector custom_message = {0x01};
    gateway_processor->GatewaySendCustomMessage(custom_message);

    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kCustomCanTableSync, {0x07, 0xdf, 0x0e, 0x80, 0x01, 0x00, 0x00}));
    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kCustomCanTableSync, {0x07, 0xdf, 0x0e, 0x80, 0x01, 0x00, 0x01, 0x07, 0x70}));
    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kCustomCanTableSync, {0x07, 0xdf, 0x0e, 0x80, 0x01, 0x00, 0x02, 0x07, 0x71}));
    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kCustomMessage, {0x07, 0xdf, 0x0e, 0x80, 0x00}));
    EXPECT_NO_THROW(gateway_processor->HandleMessage(DoIpPayloadType::kCustomMessage, {0x07, 0xdf, 0x0e, 0x80, 0x01}));

    stub.reset(ADDR(RoutingSocket, SendCustomMessage));
    stub.reset(ADDR(RoutingSocket, SendDiagnosticMessage));
    stub.reset(ADDR(RoutingSocket, ConnectServer));
}

TEST(Test_route, NodeRoute)
{
    ConversationManager& conversation_manager = *(dcm.conversation_manager_);
    auto routing_table = conversation_manager.GetRouting().GetRoutingTable();

    NodeRoute* node_route = reinterpret_cast<NodeRoute*>((routing_table.GetRoute(RouteType::kNode)).get());

    NodeProcessor::Ptr node_processor = node_route->GetNodeProcessor(1);

    stub.set(ADDR(RoutingSocket, ConnectServer), Stub_ConnectServer);
    stub.set(ADDR(RoutingSocket, SendDiagnosticMessage), Stub_SendDiagnosticMessage);
    stub.set(ADDR(RoutingSocket, SendCustomMessage), Stub_SendCustomMessage);

    node_route->CheckAndHandleMessage(Conversation_ID, 1, {tester_addr, adc, {0x10, 0x01}});
    EXPECT_NO_THROW(node_processor->HandleMessage(DoIpPayloadType::kDiagnosticAck, {0x00, 0x0e, 0x0e, 0x80, 0x00, 0x10}));
    EXPECT_NO_THROW(node_processor->HandleMessage(DoIpPayloadType::kDiagnosticMessage, {0x00, 0x2d, 0x0e, 0x80, 0x7f, 0x10, 0x22}));
    EXPECT_NO_THROW(node_processor->HandleMessage(DoIpPayloadType::kDiagnosticMessage, {0x00, 0x0e, 0x0e, 0x80, 0x7f, 0x10, 0x78}));
    EXPECT_NO_THROW(node_processor->HandleMessage(DoIpPayloadType::kDiagnosticMessage, {0x00, 0x0e, 0x0e, 0x80, 0x7f, 0x10, 0x22}));
    EXPECT_NO_THROW(node_processor->HandleMessage(DoIpPayloadType::kDiagnosticMessage, {0x00, 0x0e, 0x0e, 0x80, 0x7f, 0x50, 0x01, 0x13, 0x88, 0x13, 0x88}));
    EXPECT_NO_THROW(node_processor->HandleMessage(DoIpPayloadType::kDiagnosticNack, {0x00, 0x0e, 0x0e, 0x80, 0x00, 0x10}));
    EXPECT_NO_THROW(node_processor->HandleMessage(DoIpPayloadType::kRoutingActivationResponse, {0x00, 0x0e, 0x0e, 0x80, 0x00}));

    node_processor->HandleCacheMessageCtrl(Conversation_ID, {tester_addr, adc, {0x10, 0x01}});
    node_processor->HandleCacheMessageCtrl(Conversation_ID, {tester_addr, adc, {0x10, 0x01}});
    std::this_thread::sleep_for(std::chrono::seconds(1));
    node_processor->NodeRecevingFinish(adc);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    stub.reset(ADDR(RoutingSocket, SendCustomMessage));
    stub.reset(ADDR(RoutingSocket, SendDiagnosticMessage));
    stub.reset(ADDR(RoutingSocket, ConnectServer));
}

TEST(Test_route, PartitionRoute)
{
    ConversationManager& conversation_manager = *(dcm.conversation_manager_);
    auto routing_table = conversation_manager.GetRouting().GetRoutingTable();

    PartitionRoute* partition_route = reinterpret_cast<PartitionRoute*>((routing_table.GetRoute(RouteType::kPartition)).get());

    partition_route->SetRoutingState(false);
    EXPECT_NO_THROW(partition_route->CheckAndHandleMessage(Conversation_ID, PartitionEnum::kPartitionDcmC, {tester_addr, ccu, {0x10, 0x01}}));

    partition_route->SetRoutingState(true);
    EXPECT_NO_THROW(partition_route->CheckAndHandleMessage(Conversation_ID, PartitionEnum::kPartitionDcmC, {tester_addr, ccu, {0x10, 0x01}}));

    PartitionProcessor::Ptr partition_processor = partition_route->GetPartitionProcessor(PartitionEnum::kPartitionDcmC);

    stub.set(ADDR(RoutingSocket, ConnectServer), Stub_ConnectServer);
    stub.set(ADDR(RoutingSocket, SendDiagnosticMessage), Stub_SendDiagnosticMessage);
    stub.set(ADDR(RoutingSocket, SendCustomMessage), Stub_SendCustomMessage);

    partition_route->CheckAndHandleMessage(Conversation_ID, PartitionEnum::kPartitionDcmC, {tester_addr, ccu, {0x10, 0x01}});
    EXPECT_NO_THROW(partition_processor->HandleMessage(DoIpPayloadType::kDiagnosticAck, {0xc0, 0x01, 0x0e, 0x80, 0x00, 0x10}));
    EXPECT_NO_THROW(partition_processor->HandleMessage(DoIpPayloadType::kDiagnosticMessage, {0xc0, 0x01, 0x0e, 0x80, 0x7f, 0x10, 0x78}));
    EXPECT_NO_THROW(partition_processor->HandleMessage(DoIpPayloadType::kDiagnosticMessage, {0xc0, 0x01, 0x0e, 0x80, 0x7f, 0x10, 0x22}));
    EXPECT_NO_THROW(partition_processor->HandleMessage(DoIpPayloadType::kDiagnosticMessage, {0xc0, 0x01, 0x0e, 0x80, 0x7f, 0x50, 0x01, 0x13, 0x88, 0x13, 0x88}));
    EXPECT_NO_THROW(partition_processor->HandleMessage(DoIpPayloadType::kDiagnosticNack, {0xc0, 0x01, 0x0e, 0x80, 0x00, 0x10}));
    EXPECT_NO_THROW(partition_processor->HandleMessage(DoIpPayloadType::kRoutingActivationResponse, {0xc0, 0x01, 0x0e, 0x80, 0x00}));

    partition_processor->HandleCacheMessageCtrl(Conversation_ID, {tester_addr, rt1, {0x10, 0x01}});
    partition_processor->HandleCacheMessageCtrl(Conversation_ID, {tester_addr, rt1, {0x10, 0x01}});
    std::this_thread::sleep_for(std::chrono::seconds(1));
    partition_processor->PartitionRecevingFinish(rt1);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ASSERT_EQ(partition_route->GetPartitionTa(PartitionEnum::kPartitionDcmC, UdsMessage::TargetAddressType::kPhysical), 0xc001);
    ASSERT_EQ(partition_route->GetPartitionTa(PartitionEnum::kPartitionDcmC, UdsMessage::TargetAddressType::kFunctional), 0xcf01);

    stub.reset(ADDR(RoutingSocket, SendCustomMessage));
    stub.reset(ADDR(RoutingSocket, SendDiagnosticMessage));
    stub.reset(ADDR(RoutingSocket, ConnectServer));
}

TEST(Test_route, RoutingTable)
{
    ConversationManager& conversation_manager = *(dcm.conversation_manager_);
    auto routing_table = conversation_manager.GetRouting().GetRoutingTable();

    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x10, 0x01}}), PartitionEnum::kPartitionDcmC);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x22, 0xF1, 0x87}}), PartitionEnum::kPartitionDcmC);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x22, 0xBE, 0x30}}), PartitionEnum::kPartitionDcmA);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x2e, 0xF1, 0x87}}), PartitionEnum::kPartitionDcmC);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x2e, 0xBE, 0x30}}), PartitionEnum::kPartitionDcmA);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x31, 0x01, 0x21, 0x00}}), PartitionEnum::kPartitionDcmC);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x31, 0x01, 0xBE, 0xE9}}), PartitionEnum::kPartitionDcmA);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x34, 0x01}}), PartitionEnum::kPartitionDcmC);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x35, 0x01}}), PartitionEnum::kPartitionDcmC);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x36, 0x01, 0x00}}), PartitionEnum::kPartitionDcmC);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x37}}), PartitionEnum::kPartitionDcmC);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x38, 0x01}}), PartitionEnum::kPartitionDcmA);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x36, 0x01, 0x00}}), PartitionEnum::kPartitionDcmA);
    ASSERT_EQ(routing_table.GetProcessorByServiceRoutingTable({tester_addr, ccu, {0x37}}), PartitionEnum::kPartitionDcmA);

    EXPECT_NO_THROW(routing_table.GetRoute(0));
    EXPECT_NO_THROW(routing_table.AddGatewayAddress(0, 1));
    EXPECT_NO_THROW(routing_table.AddGatewayAddress(ta, 1));
    EXPECT_NO_THROW(routing_table.IsGatewayAddress(sa));
    EXPECT_NO_THROW(routing_table.IsGatewayAddress(ta));

    EXPECT_NO_THROW(routing_table.AddNodeAddress(0, 1));
    EXPECT_NO_THROW(routing_table.AddNodeAddress(adc, 1));
    EXPECT_NO_THROW(routing_table.GetNodeProcessorId(ta));

    EXPECT_NO_THROW(routing_table.AddPartition(1, 0));
    EXPECT_NO_THROW(routing_table.AddPartition(1, rt1));
    EXPECT_NO_THROW(routing_table.GetPartitionAddress(ta));

    EXPECT_NO_THROW(routing_table.AddServiceRoutingTableBySid(0, 1));
    EXPECT_NO_THROW(routing_table.AddServiceRoutingTableBySid(0x29, 1));
    EXPECT_NO_THROW(routing_table.AddServiceRoutingTableByDid(0, 1));
    EXPECT_NO_THROW(routing_table.AddServiceRoutingTableByDid(0xBE30, 1));
    EXPECT_NO_THROW(routing_table.AddServiceRoutingTableByWriteableDid(0, 1));
    EXPECT_NO_THROW(routing_table.AddServiceRoutingTableByWriteableDid(0xBE30, 1));
    EXPECT_NO_THROW(routing_table.AddServiceRoutingTableByRid(0, 1));
    EXPECT_NO_THROW(routing_table.AddServiceRoutingTableByRid(0xBE30, 1));

}

TEST(Test_route, TesterTable)
{
    ConversationManager& conversation_manager = *(dcm.conversation_manager_);
    EXPECT_NO_THROW(conversation_manager.GetTesterTable().AddLocalTesterTable(0, 0));
    EXPECT_NO_THROW(conversation_manager.GetTesterTable().AddLocalTesterTable(tester_addr, 1));
    EXPECT_NO_THROW(conversation_manager.GetTesterTable().AddVirtualTesterTable(tester_addr, 2));
    EXPECT_NO_THROW(conversation_manager.GetTesterTable().GetLocalTesterPriority(sa));
    EXPECT_NO_THROW(conversation_manager.GetTesterTable().VirtualTesterAddressValid(0));
    EXPECT_NO_THROW(conversation_manager.GetTesterTable().GetVirtualTesterPriority(0));
}

}
}
}
