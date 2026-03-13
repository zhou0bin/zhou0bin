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

#ifndef SOURCE_DCM_DIAGNOSTIC_SERVER_INTERFACE_H_
#define SOURCE_DCM_DIAGNOSTIC_SERVER_INTERFACE_H_

#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>
/*
namespace asf{
namespace diag{
namespace dcm{
    class DiagServerInterface;
}
}
}
*/
//#include "asf/diag/uds_transport/protocol_mgr.h"
#include "asf/diag/uds_transport/protocol_types.h"
#include "asf/diag/uds_transport/uds_message.h"
#include "uds_transport/uds_message_ext.h"


using asf::diag::uds_transport::UdsMessagePtr;
using asf::diag::uds_transport::UdsMessage;
using asf::diag::uds_transport::UdsMessageExt;


namespace asf
{
namespace diag
{
namespace dcm
{

class DiagServerInterface
{
public:
    using logicalAddr = asf::diag::uds_transport::UdsMessage::Address;
    using GlobalChannelID = uds_transport::GlobalChannelIdentifier;

////////////method////////////
public:
    DiagServerInterface() {};
    virtual ~DiagServerInterface() = default;

////////////Upstream Interface////////////

    virtual bool RequestCanBeHandled(    
        const logicalAddr /*source_addr*/,
        const logicalAddr /*target_addr*/,
        const uds_transport::UdsMessage::TargetAddressType /*type*/,
        const uds_transport::GlobalChannelIdentifier /*global_channel_id*/,
        const std::size_t /*size*/,
        const uds_transport::Priority /*priority*/,
        const uds_transport::ProtocolKind /*protocol_kind*/)
    {
        return true;
    }

    virtual void HandleMessage(const uds_transport::UdsMessageExtPtr UdsMsgExtPtr) = 0;
    // virtual void TransmitConfirm(const uds_transport::UdsMessageConstPtr message, const bool result) = 0;
    virtual void TransmitConfirm(const bool result) = 0;

    virtual void HandleChannelDisconnected(GlobalChannelID ChannelID) = 0;
    virtual void HandleChannelReestablished(GlobalChannelID ChannelID) = 0;

////////////Downstream Interface////////////

    virtual void TransmitMsg(uds_transport::UdsMessageExt UdsMsgExt, GlobalChannelID ToChannelID) = 0;
    virtual void TransmitAckMsg(const uint16_t sa, const uint16_t ta, GlobalChannelID ToChannelID) = 0;
    virtual void TransmitNackMsg(const uint16_t sa, const uint16_t ta, const uint8_t nack_code, GlobalChannelID ToChannelID) = 0;

    virtual bool RegChannelStateEvent(GlobalChannelID ChannelID) = 0;

////////////Private interface////////////
private:
    explicit DiagServerInterface(DiagServerInterface&&) = delete;
    DiagServerInterface(DiagServerInterface&) = delete;
    DiagServerInterface& operator=(DiagServerInterface&) = delete;
    DiagServerInterface& operator=(DiagServerInterface&&) = delete;

};

} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_DIAGNOSTIC_SERVER_INTERFACE_H_
