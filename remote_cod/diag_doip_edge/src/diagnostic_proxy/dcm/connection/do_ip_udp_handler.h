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

#ifndef SOURCE_DCM_CONNECTION_DO_IP_UDP_HANDLER_H_
#define SOURCE_DCM_CONNECTION_DO_IP_UDP_HANDLER_H_

#include <cstdint>
#include <mutex>
#include <thread>

#include "dcm/connection/do_ip_udp_info.h"
#include "dcm/connection/do_ip_packet.h"
#include "dcm/connection/entity_identification.h"

#include "diag_proxy_common.h"

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
class DoIpUdpHandler
{
public:
    /**
     * \brief Smart Pointer Type for the DoIpUdpHandler class.
     */
    typedef std::shared_ptr<DoIpUdpHandler> Ptr;

protected:

    /// @brief Connection manager
    DoIpConnectionManager& connManager_;

    DoIpUdpInfo doip_udp_info_;

    /**
     * \brief The local IP to listen on.
     */
    std::string local_ip_;

    /// @brief DoIp entity identification.
    std::shared_ptr<EntityIdentification> entityIdentificationPtr_;

    /// @brief The Vehicle Identification Number
    asf::diag::uds_transport::ByteVector vehicleIdentificationNumber_;

    /// @brief Mutex to guard against concurrent modifications of vehicleIdentificationNumber_.
    std::mutex vinMutex_;

    /// @brief DoIp group identification.
    GidStatus gidStatus_;

    /// @brief Mutex to guard against concurrent modifications of gid_status.
    std::mutex gidMutex_;

    /// @brief DoIp power mode.
    asf::diag::PowerModeType powerMode_;

    /// @brief Mutex to guard against concurrent modifications of powerMode_.
    std::mutex powerModeMutex_;

    std::string config_;

public:

    /// @brief Constructor for a new DoIp Connection.
    ///
    /// @param[in] connManager The connection manager.
    /// @param[in] config configuration object
    DoIpUdpHandler(const std::string& config, DoIpConnectionManager& connManager);
    explicit DoIpUdpHandler(DoIpUdpHandler&&) = delete;
    DoIpUdpHandler(DoIpUdpHandler&) = delete;
    DoIpUdpHandler& operator=(DoIpUdpHandler&) = delete;
    DoIpUdpHandler& operator=(DoIpUdpHandler&&) = delete;
    virtual ~DoIpUdpHandler() = default;

    virtual void Initialize();

    /**
     * \brief Handle a DoIp message pending in a Udp socket directed at this Connection.
     *
     * \param udp_socket The socket on which a DoIp message is pending.
     * \param remote_addr The sender address from which the DoIp message was received.
     * \param bytes_available The length of the DoIp message payload
     * \param is_socket_open Flag (in/out) to indicate whether the socket is opened or closed.
     *
     * Method is marked as virtual to allow override during testing.
     */
    virtual void HandleUdpMessage(const int udp_socket,
        struct sockaddr_in& remote_addr,
        const unsigned int bytes_available,
        bool& is_socket_open);

    /// @brief Send the vehicle announcement three times with 500ms delay between each message as defined
    ///        by DoIp 050 of ISO 13400-2:2012
    ///
    /// @param[in] udpDiscoverySocket The socket which is used to transmit the vehicle announcement.
    /// @param[in] limitedBroadcast The limited broadcast address to which the vehicle announcement shall be sent.
    ///
    /// @return Indication, whether the Vehicle Announcement was sent successfully
    virtual bool SendVehicleAnnouncement(const int udpDiscoverySocket, struct sockaddr_in& limitedBroadcast) noexcept(false);

    virtual DoIpUdpInfo& GetDoIpUdpInfo()
    {
        return doip_udp_info_;
    }

protected:

    /// @brief Getting the Entity Identification by calling the EntityIdentification class.
    ///
    /// @param[in] givenSocket
    /// @param[in, out] eid Buffer for the Entity Identification
    ///
    /// @return Indication, whether the operation was successful
    bool GetEntityIdentification(const int givenSocket, asf::diag::uds_transport::ByteVector& eid) noexcept;

    /// @brief Getter for the vehicle identifiction number.
    ///
    /// @return VIN byte array.
    virtual asf::diag::uds_transport::ByteVector GetVehicleIdentificationNumber();

    /// @brief Getter for the DoIp group identification struct.
    ///
    /// @return Returns DoIp group identification struct.
    virtual GidStatus GetGroupIdentification();

    /// @brief Getter for the DoIp power mode struct.
    ///
    /// @return Returns DoIp power mode struct.
    virtual asf::diag::PowerModeType GetDoipPowerMode();

    /// @brief Querying of the DoIP group identification.
    void QueryVehicleIdentification() noexcept;

    /// @brief Querying of the DoIP group identification.
    void QueryGroupIdentification() noexcept;

    /// @brief Querying of the diagnostic DoIP power mode.
    void QueryDiagnosticDoipPowerMode() noexcept;
    
    /// @brief Builds the vehicle announcement message by relying on methods for retrieving the VIN,
    ///        logical address, EID and GID.
    ///
    /// @param[in] givenSocket The socket to which a device was bound in order to find the mac for that device
    /// @param[in,out] vehicleAnnouncementBuffer The packet that shall be filled with the vehicle announcement message.
    virtual void BuildVehicleAnnouncementMessage(const int givenSocket, DoIpPacket& vehicleAnnouncementBuffer) noexcept(false);

    /// @brief Builds the entity status response message as described in DoIP-120.
    ///
    /// @param doip_response_message The packet that shall be filled with the entity status response.
    void BuildEntityStatusResponseMessage(DoIpPacket& doip_response_message);

    /// @brief Builds the power mode response message as described in DoIP-117.
    ///
    /// @param doip_response_message The packet that shall be filled with the power mode response.
    void BuildPowerModeResponseMessage(DoIpPacket& doip_response_message);

    bool ShouldIgnoreThisRequest(const DoIpPacket& message);
};

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_CONNECTION_DO_IP_UDP_HANDLER_H_
