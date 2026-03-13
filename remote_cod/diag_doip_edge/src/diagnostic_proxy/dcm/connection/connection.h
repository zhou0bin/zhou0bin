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

#ifndef SOURCE_DCM_CONNECTION_CONNECTION_H_
#define SOURCE_DCM_CONNECTION_CONNECTION_H_

#include <cstring>
#include <memory>
#include <tuple>

// #include "ara/core/map.h"
#include <map>

#include "asf/diag/uds_transport/protocol_types.h"
#include "uds_transport/uds_message_ext.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

/*
 * \brief Defines enumerator index of the IdentifyingTuple of a connection
 */
enum ConnectionTupleIndex : uint8_t
{
    kConnectionProtocolIdIdx,
    kConnectionConnectionIdIdx
};

/**
 * \brief Base class for all Uds Transport Protocol connections.
 */
class Connection : public std::enable_shared_from_this<Connection>
{
public:
    /**
     * \brief Type for Connection IDs
     */
    typedef int ID;

    /**
     * \brief Type for the meta information of a connection.
     */
    typedef std::map<std::string, std::string> MetaInfoMap;

    /**
     * \brief Smart Pointer Type for the Connection class.
     */
    typedef std::shared_ptr<Connection> Ptr;

    /**
     * \brief Type of tuple to pack protocollID and connectionID together.
     */
    typedef std::tuple<uds_transport::UdsTransportProtocolHandlerID, ID> IdentifyingTuple;

protected:
    /**
     * \brief A process-wide unique ID for this Connection.
     */
    ID connection_id_;

    /**
     * \brief The ID of the Uds Transport Protocol implemented by this Connection class.
     */
    uds_transport::UdsTransportProtocolHandlerID protocol_id_;

    /**
     * The Meta-Information of this Connection.
     */
    MetaInfoMap meta_info_map_;

public:
    /**
     * \brief Constructor for a new Connection.
     *
     * \param connection_id The ID of the connection.
     * \param protocol_id The ID of the Uds Transport Protocol implemented by this Connection class.
     * \param connection_manager The connection manager.
     */
    Connection(const ID connection_id,
        const uds_transport::UdsTransportProtocolHandlerID protocol_id);
    explicit Connection(Connection&&) = delete;
    Connection(Connection&) = delete;
    Connection& operator=(Connection&) = delete;
    Connection& operator=(Connection&&) = delete;
    virtual ~Connection() = default;

    /**
     * \brief Transmit a Uds message via the underlying Uds Transport Protocol connection.
     *
     * \param message The message to be transmitted.
     * \return Returns true if a message was successfully send otherwise false.
     */
    virtual bool Transmit(const asf::diag::uds_transport::UdsMessageExt& message) = 0;

    /**
     * \brief Get a DCM-wide unique identifier for the connection
     */
    IdentifyingTuple GetIdentifyingTuple() const
    {
        return std::make_tuple(protocol_id_, connection_id_);
    }

    ID GetConnectionID() const
    {
        return connection_id_;
    }
    uds_transport::UdsTransportProtocolHandlerID GetProtocolHandlerID() const
    {
        return protocol_id_;
    }
};

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_CONNECTION_CONNECTION_H_
