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

#ifndef SOURCE_DCM_CONNECTION_DO_IP_PACKET_H_
#define SOURCE_DCM_CONNECTION_DO_IP_PACKET_H_

#include <sys/uio.h>
#include <cstddef>
#include <cstdint>

// #include "ara/core/array.h"
#include <array>

#include "dcm/connection/payload.h"
#include "uds_transport/uds_message_ext.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

enum DoIpProtocolVersions : uint8_t
{
    kDoIpIsoDis13400_2_2010 = 0x01,
    kDoIpIsoDis13400_2_2012 = 0x02
};

/**
 * \brief The supported DoIp Protocol version.
 */
static const uint8_t kSupportedDoIpVersion(DoIpProtocolVersions::kDoIpIsoDis13400_2_2012);

/* DoIp Header format information */

/**
 * \brief Offset (in bytes) of the protocolVersion field in the DoIp header
 */
static const uint8_t kDoIp_ProtocolVersion_offset{0};

/**
 * \brief Length (in bytes) of the protocolVersion field in the DoIp header
 */
static const uint8_t kDoIp_ProtocolVersion_length{1};

/**
 * \brief Offset (in bytes) of the invProtocolVersion field in the DoIp header
 */
static const uint8_t kDoIp_InvProtocolVersion_offset{1};

/**
 * \brief Length (in bytes) of the invProtocolVersion field in the DoIp header
 */
static const uint8_t kDoIp_InvProtocolVersion_length{1};

/**
 * \brief Offset (in bytes) of the payloadType field in the DoIp header
 */
static const uint8_t kDoIp_PayloadType_offset{2};

/**
 * \brief Length (in bytes) of the payloadType field in the DoIp header
 */
static const uint8_t kDoIp_PayloadType_length{2};

/**
 * \brief Offset (in bytes) of the payloadLength field in the DoIp header
 */
static const uint8_t kDoIp_PayloadLength_offset{4};

/**
 * \brief Length (in bytes) of the payloadLength field in the DoIp header
 */
static const uint8_t kDoIp_PayloadLength_length{4};

/**
 * \brief Total length (in bytes) of a DoIp header
 */
static const uint8_t kDoIp_HeaderTotal_length{8};

/**
 * \brief Number of fields in the DoIp header
 */
static const uint8_t kDoIp_HeaderTotal_fields{5};

/**
 * \brief Definition of maximum protocol id
 */
static const uint8_t kDoIp_ProtocolVersion_max{0xFFU};

/**
 * \brief Definition of minimum Routing Activation Request length 
 */
static const uint8_t kDoIp_RoutingActivationRequest_length_min{7};

/**
 * \brief Definition of maximum Routing Activation Request length 
 */
static const uint8_t kDoIp_RoutingActivationRequest_length_max{11};

/**
 * \brief Definition of Alive Check Response length 
 */
static const uint8_t kDoIp_AliveCheckResponse_length{2};

/**
 * \brief Definition of Vehicle Identification Request length 
 */
static const uint8_t kDoIp_VehicleIdentificationRequest_length{0};

/**
 * \brief Definition of Vehicle Identification Request With EID length 
 */
static const uint8_t kDoIp_VehicleIdentificationRequestWithEid_length{6};

/**
 * \brief Definition of Vehicle Identification Request With VIN length 
 */
static const uint8_t kDoIp_VehicleIdentificationRequestWithVin_length{17};

/**
 * \brief Definition of Entity Status Request length 
 */
static const uint8_t kDoIp_EntityStatusRequest_length{0};

/**
 * \brief Definition of Power Mode Information Request length 
 */
static const uint8_t kDoIp_PowerModeInformationRequest_length{0};

/**
 * \brief Definition of minimum Diagnostic Message length 
 */
static const uint8_t kDoIp_DiagnosticMessage_length_min{5};

/**
 * \brief Definition of Generic DoIp Nack length 
 */
static const uint8_t kDoIp_GenericDoIpNack_length{1};

/**
 * \brief Definition of Vehicle Announcement length 
 */
static const uint8_t kDoIp_VehicleAnnouncement_length_min{32};
static const uint8_t kDoIp_VehicleAnnouncement_length_max{33};


/// @brief Identifies whether the contacted DoIP instance is either a DoIP node or a DoIP gateway.
// enum class DoIpNodeType : char
// {
//     kGateway = 0x00,
//     kNode = 0x01
// };

/**
 * \brief Enum for DoIp NACK codes
 */
enum DoIpNackCodes : char
{
    kIncorrectPatternFormat = 0x00,
    kUnknownPayloadType = 0x01,
    kMessageTooLarge = 0x02,
    kOutOfMemory = 0x03,
    kInvalidPayloadLength = 0x04
};

enum DoIpDiagMessageNackCodes : char
{
    kInvalidSourceAddress = 0x02,
    kUnknownTargetAddress = 0x03,
    kDiagMessageTooLage = 0x04,
    kOutOfMemoryToStore = 0x05,
    kTargetUnreachable = 0x06,
    kUnknownNetwork = 0x07,
    kTransportProtocolError = 0x08
};

/**
 * \brief Enum for Generic DoIp header structure - Table 11
 */
enum GenericDoIpHeader : char
{
    kProtocolVersionIdx,
    kInvProtocolVersionIdx,
    kPayloadTypeIdx,
    kPayloadLengthIdx,
    kPayloadIdx
};

/**
 * \brief Enumerator for DoIp ACK codes.
 */
enum DoIpAckCodes : char
{
    Ack = 0x00
};

/**
 * \brief Enumerator for DoIp payload types.
 */
enum DoIpPayloadType : uint16_t
{
    kGenericDoIpNack = 0,
    kVehicleIdentificationRequest = 0x0001,
    kVehicleIdentificationRequestWithEid = 0x0002,
    kVehicleIdentificationRequestWithVin = 0x0003,
    kVehicleAnnouncement = 0x0004,
    kRoutingActivationRequest = 0x0005,
    kRoutingActivationResponse = 0x0006,
    kAliveCheckRequest = 0x0007,
    kAliveCheckResponse = 0x0008,

    kDoIpEntityStatusRequest = 0x4001,
    kDoIpEntityStatusResponse = 0x4002,
    kDiagnosticPowerModeInformationRequest = 0x4003,
    kDiagnosticPowerModeInformationResponse = 0x4004,

    kDiagnosticMessage = 0x8001,
    kDiagnosticAck = 0x8002,
    kDiagnosticNack = 0x8003,
    kCustomMessage = 0x8004,
    kCustomNack = 0x8005,
    kCustomCanTableSync = 0x8006
};

/**
 * \brief Enumerator for DoIp routing activation request activation types.
 */
enum DoIpRoutingActivationRequestType : uint8_t
{
    /* ISO 13400-2:2012 Table-23*/
    kRoutingActivationDefault = 0x00,
    kRoutingActivationWWHOBD = 0x01,
    kRoutingActivationCentralSecurity = 0xE0
};

/**
 * \brief Enumerator for DoIp routing activation responses codes.
 */
enum DoIpRoutingActivationResponseCodes : uint8_t
{
    /* ISO 13400-2:2012 Table-25*/
    kRoutingActivationDeniedUnknownSa = 0x00,
    kRoutingActivationDeniedAllSocketsRegistered = 0x01,
    kRoutingActivationDeniedSaDifferent = 0x02,
    kRoutingActivationDeniedSaAlreadyRegsiteredAndActive = 0x03,
    kRoutingActivationDeniedMissingAuthentication = 0x04,
    kRoutingActivationDeniedRejectedConfirmation = 0x05,
    kRoutingActivationDeniedUnsupportedRoutingActivationType = 0x06,
    kRoutingActivationSuccessfullyActivated = 0x10,
    kRoutingActivationWillActivatedConfirmationRequired = 0x11
};

/**
 * \brief Represents a single message sent via DoIp.
 *
 * This class also provides methods to convert the DoIp header between Host byte
 * order and Network byte order.
 */
class DoIpPacket : public PayloadOwner<uint32_t>
{
public:
    /**
     * \brief Type for ScatterArray.
     */
    using ScatterArray = std::array<struct iovec, kDoIp_HeaderTotal_fields>;

    /**
     * \brief ByteOrder enumerator.
     */
    enum ByteOrder
    {
        kHost,
        kNetwork
    };
    /**
     * \brief The byte order of the doip packet.
     */
    ByteOrder byte_order_;

    /**
     * \brief Type of the protocolVersion field.
     */
    typedef uint8_t ProtocolVersion;

    /**
     * \brief Type of the source and target address.
     */
    typedef uint16_t AddressType;

    /**
     * \brief Type of the payloadType field.
     */
    typedef uint16_t PayloadType;

    using PayloadOwner::PayloadLength;

protected:
    ///
    /// @brief Logical address of the DoIp entity.
    ///
    uint16_t logicalAddress_;

    /**
     * \brief The protocolVersion DoIp header field.
     */
    ProtocolVersion protocol_version_;

    /**
     * \brief The inverse ProtocolVersion DoIp header field.
     */
    ProtocolVersion inv_protocol_version_;

public:
    /**
     * \brief The payloadType DoIp header field.
     */
    PayloadType payload_type_;

    /**
     * \brief The payloadLength DoIp header field.
     */
    PayloadLength payload_length_;

    /**
     * \brief Constructor for a DoIpPacket without payload and all header fields
     * initialized to 0.
     */
    DoIpPacket(const ByteOrder byte_order, const uint16_t givenLogicalAddress);
    explicit DoIpPacket(DoIpPacket&&) = default;
    DoIpPacket(const DoIpPacket&) = default;
    DoIpPacket& operator=(const DoIpPacket&) = default;
    DoIpPacket& operator=(DoIpPacket&&) = default;
    ~DoIpPacket() override = default;

    /**
     * \brief Setter for the payloadLength field.
     *
     * This setter also allocates a new payload buffer of the correct size.
     * Existing content of the payload field is not preserved.
     *
     * \param payload_length The new length payload_ should be set to
     * \param force Whether to ignore the current value of payload_length_ and to
     * recreate the buffer anyway.
     */
    virtual void SetPayloadLength(const PayloadLength payload_length, const bool force = false) override;

    /**
     * \brief Setter for the protocol_version_ .
     *
     * This setter also updates the inverse protocol version.
     *
     * \param prot_version The new protocol version protocol_version_ should be
     * set to
     */
    inline void SetProtocolVersion(const ProtocolVersion prot_version)
    {
        protocol_version_ = prot_version;
        inv_protocol_version_ = static_cast<ProtocolVersion>(~protocol_version_);
    }

    /**
     * \brief Getter for the protocol_version_ .
     */
    inline ProtocolVersion GetProtocolVersion(void) const
    {
        return (protocol_version_);
    }

    /**
     * \brief Getter for the inverse protocol_version_ .
     */
    inline ProtocolVersion GetInverseProtocolVersion(void) const
    {
        return (inv_protocol_version_);
    }

    /**
     * \brief Setter for the payload_type_ .
     *
     * \param type The new type payload_type_ should be set to
     */
    void SetPayloadType(const PayloadType type);

    /**
     * \brief Constructs an array of iovecs suitable for a scattered send using sendmsg()
     *
     * Note that this array contains pointers inside this DoIpPacket object. The
     * contents of the returned array will
     * become invalid when the associated DoIpPacket is modified, moved or
     * destroyed. The only valid modification of
     * the associated DoIpPacket is to call hton() and ntoh() while a scatter
     * array exists.
     */
    ScatterArray GetScatterArray();

    /**
     * \brief Helper Method to construct a new DoIpPacket object that contains a
     * DoIp routing activation response for this DoIpPacket.
     */
    DoIpPacket ConstructRoutingActivationResponse(const char ack_code);

    /**
     * \brief Helper Method to construct a new DoIpPacket object that contains a
     * DoIp negative acknowledgment for this DoIpPacket.
     */
    DoIpPacket ConstructNack(const char nack_code) const;
    DoIpPacket ConstructDiagNack(const char nack_code, const uint16_t Sa, const uint16_t Ta) const;

    /**
     * \brief Helper Method to construct a new DoIpPacket object that contains a
     * DoIp diagnostic positive acknowledgment for this DoIpPacket.
     *
     * \param uds_message message that contains the source & target address used
     * in the ACK message.
     */
    DoIpPacket ConstructDiagnosticAck(const asf::diag::uds_transport::UdsMessageExt& uds_message) const;
    DoIpPacket ConstructDiagnosticAck(const uint16_t Sa, const uint16_t Ta) const;

    /**
     * \brief Converts the header fields of this packet to network byte order.
     *
     * The conversion happens in-place.
     */
    void Hton();

    /**
     * \brief Converts the header fields of this packet to host byte order.
     *
     * The conversion happens in-place.
     */
    void Ntoh();

    /**
     * \brief Check match of DoIp protocol version and supported version.
     */
    inline bool CheckSupportOfProtocolVersion() const
    {
        return (protocol_version_ == kSupportedDoIpVersion);
    }
};

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_CONNECTION_DO_IP_PACKET_H_
