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

/// @file
/// @brief Common place for DoIp constants and helper methods as defined in ISO 13400-2:2012

#ifndef ARA_DIAG_DCM_CONNECTION_DO_IP_COMMUNICATION_H_
#define ARA_DIAG_DCM_CONNECTION_DO_IP_COMMUNICATION_H_

#include <netinet/in.h>
#include <sys/socket.h>

#include "dcm/connection/do_ip_packet.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

class DoIpCommunication
{
public:
    static bool IsSocketClosed(const int current_errno);

    /**
     * \brief Blocking read the payload field of the doip_packet from the given socket.
     *
     * The contents of doip_packet.payload are undefined when is_socket_open == false.
     * The contents of doip_packet are undefined when is_socket_open == false.
     *
     * \param socket The socket to read from
     * \param doip_packet The DoIpPacket providing the buffer space and specifying the length of the payload to be read.
     * \param is_socket_open Flag (in/out) to indicate whether the socket is opened or closed.
     */
    static void SocketReadPayload(const int socket, DoIpPacket& doip_packet, bool& is_socket_open);

    /**
     * \brief Read and discard the payload length bytes from the given socket.
     *
     * \param socket The socket to read from
     * \param discard_length The length to be read.
     * \param is_socket_open Flag (in/out) to indicate whether the socket is opened or closed.
     */
    static void SocketReadPayloadDiscard(const int socket, DoIpPacket::PayloadLength discard_length, bool& is_socket_open);

    /**
     * \brief Blocking read a doip_packet from the given socket.
     *
     * The payload size of the doip_packet must be set to the correct size of the packet being received.
     *
     * The contents of doip_packet are undefined when is_socket_open == false.
     *
     * \param socket The socket to read from
     * \param doip_packet The DoIpPacket providing the buffer space and specifying the length of the payload to be read.
     * \param is_socket_open Flag (in/out) to indicate whether the socket is opened or closed.
     * \param read_payload Whether to read the full packet (true) or just the header (false). If set to true,
     * doip_packet
     *          must contain a buffer of sufficient size.
     */
    static void SocketRead(const int socket, DoIpPacket& doip_packet, bool& is_socket_open, const bool read_payload);

    /**
     * \brief Blocking write a DoIpPacket to the given socket.
     *
     * \param socket The socket to write to
     * \param doip_packet The DoIpPacket so write.
     * \param is_socket_open Flag (in/out) to indicate whether the socket is opened or closed.
     * \param destination_address The destination address.
     */
    static void SocketWrite(const int socket, DoIpPacket& doip_packet, bool& is_socket_open, struct sockaddr_in* const destination_address);

    /// @brief Check the doip header vesion as described in DoIP-041.
    ///
    /// @param doip_message The packet that shall be filled with the DoIP Header.
    static bool CheckGenericDoIPHeaderVersion(DoIpPacket& doip_message);

    /// @brief Check the doip payload type-specific length as described in DoIP-045.
    ///
    /// @param doip_message The packet that shall be filled with the DoIP Header.
    static bool CheckPayloadTypeSpecificLength(DoIpPacket& doip_message);

    /// @brief Check the doip payload type.
    ///
    /// @param doip_message The packet that shall be filled with the DoIP Header.
    static bool CheckPayloadType(DoIpPacket& doip_message);

};

}  // namespace connection
}  // namespace dcm
}  // namespace diag
}  // namespace asf

#endif  // ARA_DIAG_DCM_CONNECTION_DO_IP_COMMUNICATION_H_
