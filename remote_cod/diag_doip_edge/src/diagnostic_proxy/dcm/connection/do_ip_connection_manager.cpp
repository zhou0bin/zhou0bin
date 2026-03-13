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

#include "dcm/connection/do_ip_connection_manager.h"
#include "dcm/connection/do_ip_constants.h"
#include "dcm/connection/entity_identification.h"

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <exception>
#include <iostream>
#include <cstring>

#include "log.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

DoIpConnectionManager::DoIpConnectionManager(const std::string& config)
    : connections_()
    , next_connection_id_(0)
    , connections_mutex_()
    , mutex_()
    , diagnostic_address_(0x0000)
    , functional_address_(0x0000)
    , eid_(0)
    , eid_use_mac_(false)
    , entity_status_max_byte_field_use_(true)
    , gid_(0)
    , gid_invalid_pattern_(0)
    , max_concurrent_tcp_sockets_(3)
    , max_data_size_(65535)
    , max_initial_vehicle_announcement_time_(0.0)
    , node_type_(1)
    , vehicle_identification_sync_status_(false)
    , vin_invalid_pattern_(0)
    , executor_(3U)
    , alive_check_channel_(nullptr)
    , general_inactivity_channel_(nullptr)
    , initial_inactivity_channel_(nullptr)
    , config_(config)
{
#ifdef USE_FS_LIBNET
    alive_check_channel_ = std::make_shared<wheels::TimerChannel>(executor_.GetEventBase());
#else
    alive_check_channel_ = std::make_shared<wheels::TimerChannel>(executor_.getEventBase());
#endif
#ifdef USE_FS_LIBNET
    general_inactivity_channel_ = std::make_shared<wheels::TimerChannel>(executor_.GetEventBase());
#else
    general_inactivity_channel_ = std::make_shared<wheels::TimerChannel>(executor_.getEventBase());
#endif
#ifdef USE_FS_LIBNET
    initial_inactivity_channel_ = std::make_shared<wheels::TimerChannel>(executor_.GetEventBase());
#else
    initial_inactivity_channel_ = std::make_shared<wheels::TimerChannel>(executor_.getEventBase());
#endif

}

void DoIpConnectionManager::Initialize()
{
    std::ifstream ifs(config_);
    if (ifs.good())
    {
        rapidjson::IStreamWrapper ist(ifs);
        rapidjson::Document doc;
        (void)doc.ParseStream(ist);

        const rapidjson::Value& ecu_instance = doc["EcuInstance"];
        if (ecu_instance.HasMember("diagnosticAddress") && ecu_instance["diagnosticAddress"].IsInt())
        {
            diagnostic_address_ = static_cast<uint16_t>(ecu_instance["diagnosticAddress"].GetInt());
        }
        if (ecu_instance.HasMember("functionalAddress") && ecu_instance["functionalAddress"].IsInt())
        {
            functional_address_ = static_cast<uint16_t>(ecu_instance["functionalAddress"].GetInt());
        }

        const rapidjson::Value& doip = doc["DoIp"];
        if (doip.HasMember("Eid") && doip["Eid"].IsInt64())
        {
            eid_ = static_cast<uint64_t>(doip["Eid"].GetInt64());
        }
        if (doip.HasMember("EidUseMac") && doip["EidUseMac"].IsBool())
        {
            eid_use_mac_ = doip["EidUseMac"].GetBool();
        }
        if (doip.HasMember("EntityStatusMaxByteFieldUse") && doip["EntityStatusMaxByteFieldUse"].IsBool())
        {
            entity_status_max_byte_field_use_ = doip["EntityStatusMaxByteFieldUse"].GetBool();
        }
        if (doip.HasMember("Gid") && doip["Gid"].IsInt64())
        {
            gid_ = static_cast<uint64_t>(doip["Gid"].GetInt64());
        }
        if (doip.HasMember("GidInvalidPattern") && doip["GidInvalidPattern"].IsInt())
        {
            gid_invalid_pattern_ = static_cast<uint8_t>(doip["GidInvalidPattern"].GetInt());
        }
        if (doip.HasMember("MaxConcurrentTcpSockets") && doip["MaxConcurrentTcpSockets"].IsInt())
        {
            max_concurrent_tcp_sockets_ = static_cast<uint8_t>(doip["MaxConcurrentTcpSockets"].GetInt());
        }
        if (doip.HasMember("MaxDataSize") && doip["MaxDataSize"].IsInt())
        {
            max_data_size_ = static_cast<uint32_t>(doip["MaxDataSize"].GetInt());
        }
        if (doip.HasMember("MaxInitialVehicleAnnouncementTime") && doip["MaxInitialVehicleAnnouncementTime"].IsFloat())
        {
            max_initial_vehicle_announcement_time_ = doip["MaxInitialVehicleAnnouncementTime"].GetFloat();
        }
        if (doip.HasMember("NodeType") && doip["NodeType"].IsInt())
        {
            node_type_ = static_cast<uint8_t>(doip["NodeType"].GetInt());
        }
        if (doip.HasMember("VehicleIdentificationSyncStatus") && doip["VehicleIdentificationSyncStatus"].IsBool())
        {
            vehicle_identification_sync_status_ = doip["VehicleIdentificationSyncStatus"].GetBool();
        }
        if (doip.HasMember("VinInvalidPattern") && doip["VinInvalidPattern"].IsInt())
        {
            vin_invalid_pattern_ = static_cast<uint8_t>(doip["VinInvalidPattern"].GetInt());
        }
    }
}

void DoIpConnectionManager::Stop()
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const std::lock_guard<std::mutex> locker_c(connections_mutex_);
    for (auto connection : connections_) {
        connection->Stop();
    }
    connections_.clear();
    if (alive_check_channel_ != nullptr)
    {
        alive_check_channel_ = nullptr;
    }
    if (general_inactivity_channel_ != nullptr)
    {
        general_inactivity_channel_ = nullptr;
    }
    if (initial_inactivity_channel_ != nullptr)
    {
        initial_inactivity_channel_ = nullptr;
    }
}

DoIpConnection::Ptr DoIpConnectionManager::FindOrCreateConnection(struct in_addr local_addr, struct in_addr remote_addr)
{
    const std::lock_guard<std::mutex> locker_c(connections_mutex_);
    uint8_t currentlyOpenSockets = 0U;

    // Look for an existing connection
    for (const Connection::Ptr connection : connections_) {
        DoIpConnection::Ptr doip_connection{std::dynamic_pointer_cast<DoIpConnection>(connection)};
        if (doip_connection) {
            if (doip_connection->IsOpenSocketPresent()) {
                ++currentlyOpenSockets;
            }
            if ((doip_connection->GetLocalAddr().s_addr == local_addr.s_addr)
             && (doip_connection->GetRemoteAddr().s_addr == remote_addr.s_addr)) {
                return doip_connection;
            }
        }
    }

    // None found, create a new one.
    if (currentlyOpenSockets < (max_concurrent_tcp_sockets_ + 1)) {
        DoIpConnection::Ptr doip_connection{std::make_shared<DoIpConnection>(
            next_connection_id_++, local_addr, remote_addr, *this)};
        (void)connections_.emplace_back(doip_connection);
        return doip_connection;
    }
    return nullptr;
}

DoIpConnection::Ptr DoIpConnectionManager::FindConnectionByID(const ConnectionIDType connection_id)
{
    //ConnectionIDType connection_id_tmp;
    const std::lock_guard<std::mutex> locker_c(connections_mutex_);
    for (const Connection::Ptr connection : connections_) {
        DoIpConnection::Ptr doip_connection{std::dynamic_pointer_cast<DoIpConnection>(connection)};
        if (doip_connection) {
            if (doip_connection->GetConnectionID() == static_cast<int>(connection_id)) {
                return doip_connection;
            }
        }
    }
    return nullptr;
}

uint8_t DoIpConnectionManager::GetCurrentlyOpenSocketsCount()
{
    const std::lock_guard<std::mutex> locker_c(connections_mutex_);
    uint8_t currentlyOpenSockets = 0U;

    for (DoIpConnection::Ptr doip_connection : connections_) {
        if (doip_connection->IsOpenSocketPresent()) {
            ++currentlyOpenSockets;
        }
    }
    return currentlyOpenSockets;
}

uint8_t DoIpConnectionManager::GetCurrentlyRegisterSocketsCount()
{
    const std::lock_guard<std::mutex> locker_c(connections_mutex_);
    uint8_t currentlyRegisterSockets = 0U;

    for (DoIpConnection::Ptr doip_connection : connections_) {
        if (doip_connection->GetConnectionState()== DoIpConnectionState::kRegistered) {
            ++currentlyRegisterSockets;
        }
    }
    return currentlyRegisterSockets;
}

DoIpConnection::Ptr DoIpConnectionManager::FindRegisterConnectionBySA(const uint16_t source_address)
{
    const std::lock_guard<std::mutex> locker_c(connections_mutex_);
    // Look for an existing connection
    for (const Connection::Ptr connection : connections_) {
        DoIpConnection::Ptr doip_connection{std::dynamic_pointer_cast<DoIpConnection>(connection)};
        if (doip_connection) {
            if ((doip_connection->GetConnectionState() == DoIpConnectionState::kRegistered)
             && (doip_connection->GetSourceAddress() == source_address)) {
                return doip_connection;
            }
        }
    }
    return nullptr;
}

bool DoIpConnectionManager::PerformAliveCheckForAllRegisterSockets()
{
    const std::lock_guard<std::mutex> locker_c(connections_mutex_);
    // Look for an existing connection
    for (const Connection::Ptr connection : connections_) {
        DoIpConnection::Ptr doip_connection{std::dynamic_pointer_cast<DoIpConnection>(connection)};
        if (doip_connection) {
            if ((doip_connection->GetConnectionState() == DoIpConnectionState::kRegistered)) {
                auto future_tmp = doip_connection->PerformAliveCheck();
                if (future_tmp.get() == false) {
                    return true;
                }
            }
        }
    }
    return false;
}

DoIpRoutingActivationResponseCodes DoIpConnectionManager::ConnectionRoutingActivation(DoIpConnection& doip_connection, const uint16_t source_address, const uint8_t activation_type)
{
    const std::lock_guard<std::mutex> locker(mutex_);
    DoIpRoutingActivationResponseCodes ack_code = kRoutingActivationSuccessfullyActivated;
    // DoIP-059, ISO13400-2:2012
    if ((source_address < doip::External_Test_Equipment_Addr_Min) || (source_address > doip::External_Test_Equipment_Addr_Max))
    {
        LOG_WARN << __FUNCTION__ << "RoutingActivationRequest SA<0E00||SA>0FFF error. ";
        ack_code = DoIpRoutingActivationResponseCodes::kRoutingActivationDeniedUnknownSa;
    }
    // DoIP-151, ISO13400-2:2012
    else if ((activation_type != DoIpRoutingActivationRequestType::kRoutingActivationDefault)
          && (activation_type != DoIpRoutingActivationRequestType::kRoutingActivationWWHOBD)
          && (activation_type != DoIpRoutingActivationRequestType::kRoutingActivationCentralSecurity))
    {
        LOG_WARN << __FUNCTION__ << "RoutingActivationRequest Type error. ";
        ack_code = DoIpRoutingActivationResponseCodes::kRoutingActivationDeniedUnsupportedRoutingActivationType;
    }
    // Figure 13, ISO13400-2:2012
    else
    {
        if (GetCurrentlyRegisterSocketsCount() > 0)
        {
            if (doip_connection.GetConnectionState() != DoIpConnectionState::kRegistered)
            {
                auto doip_connection_tmp = FindRegisterConnectionBySA(source_address);
                // DoIP-091, ISO13400-2:2012
                if (doip_connection_tmp)
                {
                    auto future_tmp = doip_connection_tmp->PerformAliveCheck();
                    // DoIP-093, ISO13400-2:2012
                    if (future_tmp.get() == true)
                    {
                        ack_code = DoIpRoutingActivationResponseCodes::kRoutingActivationDeniedSaAlreadyRegsiteredAndActive;
                    }
                    // DoIP-092, ISO13400-2:2012
                    else
                    {
                        ack_code = DoIpRoutingActivationResponseCodes::kRoutingActivationSuccessfullyActivated;
                    }
                }
                else
                {
                    // DoIP-090, ISO13400-2:2012
                    if (GetCurrentlyRegisterSocketsCount() < GetMaxConcurrentTcpSockets())
                    {
                        ack_code = DoIpRoutingActivationResponseCodes::kRoutingActivationSuccessfullyActivated;
                    }
                    // DoIP-094, ISO13400-2:2012
                    else
                    {
                        // DoIP-095, ISO13400-2:2012
                        if (PerformAliveCheckForAllRegisterSockets())
                        {
                            ack_code = DoIpRoutingActivationResponseCodes::kRoutingActivationSuccessfullyActivated;
                        }
                        // DoIP-096, ISO13400-2:2012
                        else
                        {
                            ack_code = DoIpRoutingActivationResponseCodes::kRoutingActivationDeniedAllSocketsRegistered;
                        }
                    }
                }
            }
            else
            {
                // DoIP-106, ISO13400-2:2012
                if (doip_connection.GetSourceAddress() != source_address)
                {
                    LOG_WARN << __FUNCTION__ << "RoutingActivationRequest SA Different error. ";
                    ack_code = DoIpRoutingActivationResponseCodes::kRoutingActivationDeniedSaDifferent;
                }
                // DoIP-089, ISO13400-2:2012
                else
                {
                    ack_code = DoIpRoutingActivationResponseCodes::kRoutingActivationSuccessfullyActivated;
                }
            }
        }
        else
        {
            ack_code = DoIpRoutingActivationResponseCodes::kRoutingActivationSuccessfullyActivated;
        }
    }
    if (ack_code == DoIpRoutingActivationResponseCodes::kRoutingActivationSuccessfullyActivated)
    {
        doip_connection.SetSourceAddress(source_address);
        doip_connection.SetConnectionState(DoIpConnectionState::kRegistered);
    }
    return ack_code;
}

} /* namespace connection */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
