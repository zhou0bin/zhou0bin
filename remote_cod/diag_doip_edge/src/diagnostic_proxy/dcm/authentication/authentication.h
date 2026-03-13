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

#ifndef SOURCE_DCM_AUTHENTICATION_AUTHENTICATION_H_
#define SOURCE_DCM_AUTHENTICATION_AUTHENTICATION_H_

#include "dcm/authentication/authentication_timer.h"

#include "diag_proxy_common.h"

#include "diag_libnet_common.h"

#include "communication/diag_libnet_server.h"

// #include "ara/core/map.h"
#include <map>

namespace asf
{
namespace diag
{
namespace dcm
{
namespace authentication
{

class Authentication
{
public:
    using AuthenticationServer = std::shared_ptr<communication::Server>;
    using AuthenticationClientMap = std::map<uint64_t, std::string>;
    using AuthenticationResponseCallback = std::function<void(const std::vector<uint8_t>& )>;

public:
    explicit Authentication(const std::string& config);
    virtual ~Authentication() = default;

    virtual void Initialize();
    virtual void Deinitialize();

    bool AuthenticationEnable() const;

    void StartAuthenticate(const uint8_t conversation_id);

    uint8_t Authenticate(const uint8_t conversation_id);

    uint8_t Authenticate();

    bool IsAuthenticationService(const uint16_t service_address, const uint16_t target_address, const uint8_t sid);

    bool IsAuthenticationExemption(const uint16_t service_address, const uint16_t target_address);

    void StartAuthenticationTimer(const uint8_t conversation_id);

    void StopAuthenticationTimer(const uint8_t conversation_id);

    void AuthenticationTimeout();

    void TargetExemptionTimeout();

    void SetAuthenticationResult(const uint8_t conversation_id, const uint8_t result);

    void DealAuthenticationMessage(const std::vector<uint8_t>& data);

    void RegisterAuthenticationResponseCallback(const AuthenticationResponseCallback fun);

protected:
    void SetAuthenticationMode(const uint8_t value);
    uint8_t GetAuthenticationMode();

    void SetAuthenticationExemption(const uint8_t value);
    uint8_t GetAuthenticationExemption();

    void SetAuthenticationState(const uint8_t value);
    uint8_t GetAuthenticationState();

    void SetAuthenticationCid(const uint8_t value);
    uint8_t GetAuthenticationCid();

    void ClientAvaliable(const uint64_t client, const bool avaliable);

    void ReceiveProxyMessage(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& data);

    bool SendAuthenticationMessage(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& data);

    void HandleAuthenticationMode(const uint8_t value);

    void HandleAuthenticationResult(const uint8_t value);

    void UpdateAuthenticationState(const uint8_t value);

    void NotifyAuthenticationState(const uint8_t value);

    void NotifyAuthenticationStateChange(const uint8_t value);

    uint8_t GetAuthenticationResult();

    void UpdateAuthenticationExemption(const uint8_t value);

    bool AuthenticationExemption();

    void TargetExemption(const bool value);

    bool TargetExemption();

    void StartTargetExemption();

    void HandleAuthenticationMessage(const uint8_t error_code, const std::vector<uint8_t>& data);

    void SendAuthenticationResponse(const std::vector<uint8_t>& data);

private:
    explicit Authentication(Authentication&&) = delete;
    Authentication(Authentication&) = delete;
    Authentication& operator=(Authentication&) = delete;
    Authentication& operator=(Authentication&&) = delete;

private:
    std::string config_;

    bool authentication_enable_;
    int authentication_timeout_;

    std::atomic<uint8_t> authentication_exemption_;
    std::atomic<uint8_t> authentication_mode_;
    std::atomic<uint8_t> authentication_state_;
    std::atomic<uint8_t> authentication_cid_;

    AuthenticationServer authentication_server_;
    std::string authentication_path_com_;
    std::string authentication_specifier_;
    std::string authentication_mode_specifier_;
    std::string authentication_result_specifier_;
    std::string authentication_status_specifier_;

    AuthenticationClientMap client_map_;
    std::mutex client_map_mutex_;

    std::mutex send_mutex_;

    std::mutex mutex_;

    AuthenticationResponseCallback callback_;

    AuthenticationTimer::Ptr authentication_timer_;

    std::atomic_bool target_exemption_;

};

} /* namespace authentication */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */

#endif  // SOURCE_DCM_AUTHENTICATION_AUTHENTICATION_H_
