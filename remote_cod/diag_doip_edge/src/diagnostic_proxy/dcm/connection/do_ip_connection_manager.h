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

#ifndef SOURCE_DCM_CONNECTION_DO_IP_CONNECTION_MANAGER_H_
#define SOURCE_DCM_CONNECTION_DO_IP_CONNECTION_MANAGER_H_

#include "dcm/connection/do_ip_connection.h"

#include "asf/diag/uds_transport/protocol_types.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

/**
 * \brief This is just a small wrapper around the creation/management of a DoIpConnection used by
 * DoIpHandler. This minimal functionality could also be integrated within DoIpHandler but would make
 * testing via mocks a lot harder! I.e. Testing of DoIpHandler which internally creates its DoIpConnections via "new" is
 * hard to achieve, since we want to work with DoIpConnection-mocks...
 */
class DoIpConnectionManager
{
public:
    using ConnectionIDType = uds_transport::ChannelID;
public:
    ///
    /// @brief Constructor for a new DoIpConnectionManager.
    ///
    /// @param[in] config The config path.
    ///
    DoIpConnectionManager(const std::string& config);
    explicit DoIpConnectionManager(DoIpConnectionManager&&) = delete;
    DoIpConnectionManager(DoIpConnectionManager&) = delete;
    DoIpConnectionManager& operator=(DoIpConnectionManager&) = delete;
    DoIpConnectionManager& operator=(DoIpConnectionManager&&) = delete;
    virtual ~DoIpConnectionManager() = default;

    virtual void Initialize();

    ///
    /// @brief Close all connections and the corresponding sockets.
    ///
    virtual void Stop();

    ///
    /// @brief Returns a DoIpConnection object for the given tuple of remote IP/local IP.
    /// If no existing Connection object is found, creates a new connection object.
    /// Method is marked as virtual to allow override during testing.
    ///
    /// @param[in] remote_addr The remote connection address
    /// @param[in] local_addr The local connection address
    /// @return The connection object, if successfully created/found
    ///
    virtual std::shared_ptr<DoIpConnection> FindOrCreateConnection(struct in_addr local_addr, struct in_addr remote_addr);

    virtual std::shared_ptr<DoIpConnection> FindConnectionByID(const ConnectionIDType connection_id);

    /// @brief Looks up for open TCP sockets.
    ///
    /// @return Returns the number of all TCP sockets currently opened.
    virtual uint8_t GetCurrentlyOpenSocketsCount();

    /// @brief Looks up for register TCP sockets.
    /// @return Returns the number of all TCP sockets currently status registered.
    virtual uint8_t GetCurrentlyRegisterSocketsCount();

    /// @brief Looks up for register TCP sockets.
    /// @return Return a DoIpConnection object currently status registered for the given source_address.
    virtual std::shared_ptr<DoIpConnection> FindRegisterConnectionBySA(const uint16_t source_address);
   
    /// @brief Perform alive check for all register TCP sockets.
    /// @return Return ture for least one socket was closed. 
    /// @return Return false for no socket was closed.
    virtual bool PerformAliveCheckForAllRegisterSockets();

    virtual DoIpRoutingActivationResponseCodes ConnectionRoutingActivation(DoIpConnection& doip_connection, const uint16_t source_address, const uint8_t activation_type);

    inline uint16_t GetLogicalAddress() const
    {
        return diagnostic_address_;
    }

    inline uint16_t GetFunctionalAddress() const
    {
        return functional_address_;
    }

    inline uint64_t GetEid() const
    {
        return eid_;
    }

    inline bool GetEidUseMac() const
    {
        return eid_use_mac_;
    }

    inline bool GetEntityStatusMaxByteFieldUse() const
    {
        return entity_status_max_byte_field_use_;
    }

    inline uint64_t GetGid() const
    {
        return gid_;
    }

    inline uint8_t GetGidInvalidPattern() const
    {
        return gid_invalid_pattern_;
    }

    inline uint8_t GetMaxConcurrentTcpSockets() const
    {
        return max_concurrent_tcp_sockets_;
    }

    inline uint32_t GetMaxDataSize() const
    {
        return max_data_size_;
    }

    inline float GetMaxInitialVehicleAnnouncementTime() const
    {
        return max_initial_vehicle_announcement_time_;
    }

    inline uint8_t GetNodeType() const
    {
        return node_type_;
    }

    inline bool GetVehicleIdentificationSyncStatus() const
    {
        return vehicle_identification_sync_status_;
    }

    inline uint8_t GetVinInvalidPattern() const
    {
        return vin_invalid_pattern_;
    }

    inline std::shared_ptr<wheels::TimerChannel> GetAliveCheckChannel()
    {
        return alive_check_channel_;
    }

    inline std::shared_ptr<wheels::TimerChannel> GetGeneralInactivityChannel()
    {
        return general_inactivity_channel_;
    }

    inline std::shared_ptr<wheels::TimerChannel> GetInitialInactivityChannel()
    {
        return initial_inactivity_channel_;
    }

protected:
    /**
     * \brief Container for all connection objects created and owned by this ConnectionManager
     */
    std::vector<DoIpConnection::Ptr> connections_;

    /**
     * \brief Counter used to hand out unique connection IDs to newly created Connection objects.
     */
    Connection::ID next_connection_id_;

    ///
    /// @brief Mutex to guard against concurrent modifications of connections_.
    ///
    std::mutex connections_mutex_;

    std::mutex mutex_;

    ///
    /// @brief The configuration object.
    ///
    uint16_t diagnostic_address_;
    uint16_t functional_address_;

    uint64_t eid_;
    bool eid_use_mac_;
    bool entity_status_max_byte_field_use_;
    uint64_t gid_;
    uint8_t gid_invalid_pattern_;
    uint8_t max_concurrent_tcp_sockets_;
    uint32_t max_data_size_;
    float max_initial_vehicle_announcement_time_;
    uint8_t node_type_;
    bool vehicle_identification_sync_status_;
    uint8_t vin_invalid_pattern_;

    wheels::IOThreadPoolExecutor executor_;
    std::shared_ptr<wheels::TimerChannel> alive_check_channel_;
    std::shared_ptr<wheels::TimerChannel> general_inactivity_channel_;
    std::shared_ptr<wheels::TimerChannel> initial_inactivity_channel_;

private:
    const std::string& config_;

};

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
#endif  // SOURCE_DCM_CONNECTION_DO_IP_CONNECTION_MANAGER_H_
