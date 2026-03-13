#ifndef DP_UNIT_COMMON_H
#define DP_UNIT_COMMON_H

#include <iostream>
#include <iomanip>
#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gtest/stub.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

#include "common/multi_byte_type.h"

#include "dcm/conversation/conversation.h"

#include "connection/do_ip_packet.h"
using asf::diag::dcm::connection::DoIpPacket;

#include "uds_transport/uds_message_ext.h"
using asf::diag::uds_transport::UdsMessage;
using asf::diag::uds_transport::UdsMessageExt;
using asf::diag::uds_transport::UdsMessageExtPtr;
using asf::diag::uds_transport::ByteVector;
using asf::diag::uds_transport::GlobalChannelIdentifier;
using asf::diag::dcm::conversation::Conversation;

#include "uds_transport/protocol_types_priv.h"
using asf::diag::uds_transport::UdsTransportProtocol;

using TargetAddressType = asf::diag::uds_transport::UdsMessage::TargetAddressType;
using DoIpPayloadType = asf::diag::dcm::connection::DoIpPayloadType;

const GlobalChannelIdentifier Channel_ID{1, 1};
const Conversation::ConversationId Conversation_ID{1};

const uint64_t Client_ID{1};

const uint16_t tester_addr{0x0e80};
const uint16_t ccu{0x004f};
const uint16_t rt1{0xc001};
const uint16_t func{0x7df};

const uint16_t sa{0x0e01};
const uint16_t ta{0x0770};

const uint16_t adc{0x000e};

const std::string kPathToConfigFile = "/containers/NeuSARPlatform/work/common/opt/AsfDiagnosticProxy/etc/proxy_config_parallel.json";

#endif
