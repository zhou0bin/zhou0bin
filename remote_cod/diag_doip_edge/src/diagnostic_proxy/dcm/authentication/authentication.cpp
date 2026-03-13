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

#include "dcm/authentication/authentication.h"

#include "dcm/connection/uds_services.h"
using asf::diag::dcm::connection::UdsServices;

#include "dcm/connection/uds_error_code.h"
using asf::diag::dcm::connection::UdsNegativeResponseCode;

#include "log.h"

namespace
{
enum OBDTester : uint8_t
{
    kDoipDiagProcessing = 0x04
};

#ifdef AUTH_TBOX_EXEMPTION
constexpr uint16_t kEcuTboxLogicalAddress{0x002D};
#endif
}

namespace asf
{
namespace diag
{
namespace dcm
{
namespace authentication
{

using asf::diag::msg_on_neusar_net::AuthMsgType;

Authentication::Authentication(const std::string& config)
    : config_(config)
    , authentication_enable_(false)
    , authentication_timeout_(5000)
    , authentication_exemption_(FactoryMode::kInitialMode)
    , authentication_mode_(AuthenticationMode::kAuthClose)
    , authentication_state_(AuthenticationState::kInitial)
    , authentication_cid_(0U)
    , authentication_server_(std::make_shared<communication::Server>())
    , authentication_path_com_("/tmp/ASF_Diag_Authentication_Interface")
    , authentication_specifier_("Diag_Authentication_Interface")
    , authentication_mode_specifier_("Diag_Authentication_Mode_Interface")
    , authentication_result_specifier_("Diag_Authentication_Result_Interface")
    , authentication_status_specifier_("Diag_Authentication_Status_Interface")
    , client_map_()
    , client_map_mutex_()
    , send_mutex_()
    , mutex_()
    , callback_(nullptr)
    , authentication_timer_(nullptr)
    , target_exemption_(false)
{
}

void Authentication::Initialize()
{
    std::ifstream ifs(config_);
    if (ifs.good())
    {
        rapidjson::IStreamWrapper ist(ifs);
        rapidjson::Document doc;
        (void)doc.ParseStream(ist);

        const rapidjson::Value& diag_authentication = doc["DiagnosticAuthentication"];
        if ((diag_authentication.HasMember("AuthenticationEnable") && diag_authentication["AuthenticationEnable"].IsBool())
         && (diag_authentication.HasMember("AuthenticationTime") && diag_authentication["AuthenticationTime"].IsInt()))
        {
            authentication_enable_ = diag_authentication["AuthenticationEnable"].GetBool();
            authentication_timeout_ = diag_authentication["AuthenticationTime"].GetInt();
        }
    }
    LOG_ERROR << __FUNCTION__ << "AuthenticationEnable: " << authentication_enable_;
    if (authentication_enable_ == true)
    {
        authentication_mode_ = AuthenticationMode::kAuthService29;
    }
    LOG_ERROR << __FUNCTION__ << "AuthenticationMode: " << authentication_mode_ << "(0: Close, 1: 0x29, 2: 0x27)";

    authentication_timer_ = std::make_shared<AuthenticationTimer>(*this, authentication_timeout_);

    authentication_server_->RegisterHandleAvaliableCallback([this](const uint64_t client, const bool avaliable)
                                                            { ClientAvaliable(client, avaliable); });
    authentication_server_->RegisterHandleMessageCallback([this](const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t> &data)
                                                          { ReceiveProxyMessage(client_handle, type, data); });
    authentication_server_->OfferService(authentication_path_com_);
}

void Authentication::Deinitialize()
{
    try {
        authentication_server_->Stop();
    } catch(...) { }
}

bool Authentication::AuthenticationEnable() const
{
    return authentication_enable_;
}

void Authentication::StartAuthenticate(const uint8_t conversation_id)
{
    LOG_DEBUG << __FUNCTION__ << "conversation_id: " << conversation_id;
    SetAuthenticationCid(conversation_id);
    if (GetAuthenticationState() != AuthenticationState::kProcessing)
    {
        LOG_INFO << __FUNCTION__ << "kProcessing. ";
        UpdateAuthenticationState(AuthenticationState::kProcessing);
    }
}

uint8_t Authentication::Authenticate(const uint8_t conversation_id)
{
    const auto cid = GetAuthenticationCid();
    if (cid != conversation_id)
    {
        LOG_WARN << __FUNCTION__ << "conversation_id: " << conversation_id << "mismatched: " << cid;
        return AuthenticateResult::kAuthenticateInitial;
    }
    LOG_DEBUG << __FUNCTION__ << "conversation_id: " << conversation_id;
    const uint8_t authenticate_result = GetAuthenticationResult();
    LOG_DEBUG << __FUNCTION__ << "AuthenticateResult: " << authenticate_result << "(0: Initial, 1: Success, 2: Failure)";
    if (authenticate_result == AuthenticateResult::kAuthenticateFailure)
    {
        LOG_ERROR << __FUNCTION__ << "kAuthenticateFailure! AuthenticationMode: " << GetAuthenticationMode() << "(0: Close, 1: 0x29, 2: 0x27)";
    }
    return authenticate_result;
}

uint8_t Authentication::Authenticate()
{
#ifdef AUTH_TARGET_EXEMPTION
    if (TargetExemption())
    {
        return AuthenticateResult::kAuthenticateSuccess;
    }
#endif
    if (AuthenticationExemption())
    {
        return AuthenticateResult::kAuthenticateInitial;
    }
    const uint8_t authenticate_result = GetAuthenticationResult();
    LOG_DEBUG << __FUNCTION__ << "AuthenticateResult: " << authenticate_result << "(0: Initial, 1: Success, 2: Failure)";
    if (authenticate_result == AuthenticateResult::kAuthenticateFailure)
    {
        LOG_ERROR << __FUNCTION__ << "kAuthenticateFailure! AuthenticationMode: " << GetAuthenticationMode() << "(0: Close, 1: 0x29, 2: 0x27)";
    }
    return authenticate_result;
}

bool Authentication::IsAuthenticationService(const uint16_t service_address, const uint16_t target_address, const uint8_t sid)
{
    if (target_address != service_address)
    {
        LOG_DEBUG << __FUNCTION__ << "Ta: " << LOG_HEX(target_address);
        return false;
    }
    if (GetAuthenticationExemption() == FactoryMode::kFactoryMode)
    {
        LOG_DEBUG << __FUNCTION__ << "kFactoryMode! ";
        return false;
    }
    if (GetAuthenticationMode() == AuthenticationMode::kAuthService29)
    {
        if (sid == UdsServices::kAuthentication)
        {
            return true;
        }
    }
    else if (GetAuthenticationMode() == AuthenticationMode::kAuthService27)
    {
        if (sid == UdsServices::kSecurityAccess)
        {
            return true;
        }
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "kAuthClose! ";
    }
    return false;
}

bool Authentication::IsAuthenticationExemption(const uint16_t service_address, const uint16_t target_address)
{
    bool is_authentication_exemption{false};
    if (GetAuthenticationExemption() == FactoryMode::kFactoryMode)
    {
        LOG_DEBUG << __FUNCTION__ << "kFactoryMode! ";
        is_authentication_exemption = true;
    }
    else
    {
        if (GetAuthenticationMode() == AuthenticationMode::kAuthClose)
        {
            LOG_DEBUG << __FUNCTION__ << "kAuthClose! ";
            is_authentication_exemption = true;
        }
        else
        {
            if (target_address == service_address)
            {
                LOG_DEBUG << __FUNCTION__ << "kCCUDiag! ";
                is_authentication_exemption = true;
            }
#ifdef AUTH_TBOX_EXEMPTION
            else if (target_address == kEcuTboxLogicalAddress)
            {
                LOG_DEBUG << __FUNCTION__ << "kTboxDiag! ";
                is_authentication_exemption = true;
            }
#endif
            else
            {
                LOG_DEBUG << __FUNCTION__ << "Ta: " << LOG_HEX(target_address);
            }
        }
    }
    LOG_DEBUG << __FUNCTION__ << "is_authentication_exemption: " << is_authentication_exemption;
    if (is_authentication_exemption == true)
    {
        StartTargetExemption();
    }
    return is_authentication_exemption;
}

void Authentication::StartAuthenticationTimer(const uint8_t conversation_id)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (GetAuthenticationState() == AuthenticationState::kInitial)
    {
        LOG_VERBOSE << __FUNCTION__ << "kInitial";
        return;
    }
    const auto cid = GetAuthenticationCid();
    if (cid != conversation_id)
    {
        LOG_DEBUG << __FUNCTION__ << "conversation_id: " << conversation_id << "mismatched: " << cid;
        return;
    }
    LOG_DEBUG << __FUNCTION__ << "conversation_id: " << conversation_id;
    authentication_timer_->StartAuthenticationTimer();
}

void Authentication::StopAuthenticationTimer(const uint8_t conversation_id)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const auto cid = GetAuthenticationCid();
    if (cid != conversation_id)
    {
        LOG_DEBUG << __FUNCTION__ << "conversation_id: " << conversation_id << "mismatched: " << cid;
        return;
    }
    LOG_DEBUG << __FUNCTION__ << "conversation_id: " << conversation_id;
    authentication_timer_->StopAuthenticationTimer();
}

void Authentication::AuthenticationTimeout()
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (GetAuthenticationState() != AuthenticationState::kInitial)
    {
        UpdateAuthenticationState(AuthenticationState::kInitial);
    }
    SetAuthenticationCid(0U);
}

void Authentication::SetAuthenticationMode(const uint8_t value)
{
    LOG_ERROR << __FUNCTION__ << "AuthenticationMode: " << value << "(0: Close, 1: 0x29, 2: 0x27)";
    authentication_mode_.store(value, std::memory_order_release);
}

uint8_t Authentication::GetAuthenticationMode()
{
    return authentication_mode_.load(std::memory_order_acquire);
}

void Authentication::SetAuthenticationExemption(const uint8_t value)
{
    LOG_ERROR << __FUNCTION__ << "AuthenticationState: " << value << "(0: Initial, FF: FactoryMode)";
    authentication_exemption_.store(value, std::memory_order_release);
}

uint8_t Authentication::GetAuthenticationExemption()
{
    return authentication_exemption_.load(std::memory_order_acquire);
}

void Authentication::SetAuthenticationState(const uint8_t value)
{
    LOG_ERROR << __FUNCTION__ << "AuthenticationState: " << value << "(0: Initial, 1: Success, 2: Failure, 3: Processing)";
    authentication_state_.store(value, std::memory_order_release);
}

uint8_t Authentication::GetAuthenticationState()
{
    return authentication_state_.load(std::memory_order_acquire);
}

void Authentication::SetAuthenticationCid(const uint8_t value)
{
    LOG_DEBUG << __FUNCTION__ << "conversation_id: " << value;
    authentication_cid_.store(value, std::memory_order_release);
}

uint8_t Authentication::GetAuthenticationCid()
{
    return authentication_cid_.load(std::memory_order_acquire);
}

void Authentication::ClientAvaliable(const uint64_t client, const bool avaliable)
{
    const std::lock_guard<std::mutex> locker_client{client_map_mutex_};
    if (avaliable == false)
    {
        const auto it = client_map_.find(client);
        if (it != client_map_.end())
        {
            (void)client_map_.erase(it);
        }
    }
}

void Authentication::ReceiveProxyMessage(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& data)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (!(data.size() > 0))
    {
        LOG_ERROR << __FUNCTION__ << "data size is 0. ";
        return;
    }
    LOG_INFO << __FUNCTION__ << "client: " << client_handle;
    if (CommMsgType::kOfferMessage == type)
    {
        std::string specifier{};
        (void)specifier.assign(data.begin(), data.end());
        LOG_INFO << __FUNCTION__ << "Offer! specifier: " << specifier << "client: " << client_handle;
        {
            const std::lock_guard<std::mutex> locker_offermsg{client_map_mutex_};
            if (client_map_[client_handle] != specifier)
            {
                client_map_[client_handle] = specifier;
            }
        }
#if 0
        if (specifier.compare(authentication_mode_specifier_) == 0)
        {
            const uint8_t value{0x01};
            const std::vector<uint8_t> buffer{value};
            (void)SendAuthenticationMessage(client_handle, AuthMsgType::kGetAuthMode, buffer);
        }
#endif
    }
    else if (CommMsgType::kStopOfferMessage == type)
    {
        std::string specifier{};
        (void)specifier.assign(data.begin(), data.end());
        LOG_INFO << __FUNCTION__ << "StopOffer! specifier: " << specifier << "client: " << client_handle;
        {
            const std::lock_guard<std::mutex> locker_stopoffermsg{client_map_mutex_};
            const auto it = client_map_.find(client_handle);
            if (it != client_map_.end())
            {
                (void)client_map_.erase(it);
            }
        }
    }
    else
    {
        LOG_INFO << __FUNCTION__ << "AuthMsg type: " << LOG_HEX(type);
        switch (type)
        {
            case AuthMsgType::kGetAuthMode:
            {
                const uint8_t value = data.at(0);
                LOG_ERROR << __FUNCTION__ << "kGetAuthMode value: " << LOG_HEX(value);
                HandleAuthenticationMode(value);
                break;
            }
            case AuthMsgType::kUpdateAuthMode:
            {
                const uint8_t value = data.at(0);
                LOG_ERROR << __FUNCTION__ << "kUpdateAuthMode value: " << LOG_HEX(value);
                HandleAuthenticationMode(value);
                break;
            }
            case AuthMsgType::kUpdateAuthResult:
            {
                const uint8_t value = data.at(0);
                LOG_ERROR << __FUNCTION__ << "kUpdateAuthResult value: " << LOG_HEX(value);
                HandleAuthenticationResult(value);
                break;
            }
            case AuthMsgType::kAuthResponse:
            {
                uint8_t error_code{0U};
                if (data.size() == 1)
                {
                    error_code = data.at(0);
                }
                LOG_ERROR << __FUNCTION__ << "kAuthResponse error_code: " << LOG_HEX(error_code);
                HandleAuthenticationMessage(error_code, data);
                break;
            }
            default:
            {
                LOG_VERBOSE << __FUNCTION__ << "unknow type. ";
                break;
            }
        }
    }
}

bool Authentication::SendAuthenticationMessage(const uint64_t client_handle, const uint16_t type, const std::vector<uint8_t>& data)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (!(data.size() > 0))
    {
        LOG_ERROR << __FUNCTION__ << "data size is 0. ";
        return false;
    }
    LOG_ERROR << __FUNCTION__ << "client_handle: " << client_handle << "type: " << LOG_HEX(type) << "value: " << LOG_HEX(data.at(0));
    const std::lock_guard<std::mutex> locker_send(send_mutex_);
    if (!authentication_server_->SendMessage(client_handle, type, data))
    {
        return false;
    }
    return true;
}

void Authentication::HandleAuthenticationMode(const uint8_t value)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if (GetAuthenticationMode() != value)
    {
        SetAuthenticationMode(value);
    }
}

void Authentication::HandleAuthenticationResult(const uint8_t value)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    if ((value == FactoryMode::kInitialMode) || (value == FactoryMode::kFactoryMode))
    {
        UpdateAuthenticationExemption(value);
        return;
    }
    if (AuthenticationExemption())
    {
        return;
    }
    if (GetAuthenticationState() != AuthenticationState::kProcessing)
    {
        LOG_WARN << __FUNCTION__ << "Not kProcessing! ";
        return;
    }
    LOG_DEBUG << __FUNCTION__ << "value: " << value;
    uint8_t authentication_state = AuthenticationState::kInitial;
    if (value == AuthenticationResult::kAuthenticationGeneralReject)
    {
        authentication_state = AuthenticationState::kFailure;
    }
    else if (value == AuthenticationResult::kAuthenticationComplete)
    {
        authentication_state = AuthenticationState::kSuccess;
    }
    else if (value == AuthenticationResult::kDeAuthenticationSuccessful)
    {
        authentication_state = AuthenticationState::kInitial;
    }
    else
    {
        return;
    }
    if (GetAuthenticationState() != authentication_state)
    {
        UpdateAuthenticationState(authentication_state);
    }
    if (GetAuthenticationState() == AuthenticationState::kInitial)
    {
        SetAuthenticationCid(0U);
    }
}

void Authentication::UpdateAuthenticationState(const uint8_t value)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    SetAuthenticationState(value);
    NotifyAuthenticationState(value);
}

void Authentication::NotifyAuthenticationState(const uint8_t value)
{
    LOG_INFO << __FUNCTION__ << "in! ";
#ifdef AUTH_TARGET_EXEMPTION
    if (value == AuthenticationState::kInitial)
    {
        if (TargetExemption())
        {
            LOG_INFO << __FUNCTION__ << "kDoipDiagProcessing! ";
            NotifyAuthenticationStateChange(OBDTester::kDoipDiagProcessing);
            return;
        }
    }
#endif
    NotifyAuthenticationStateChange(value);
}

void Authentication::NotifyAuthenticationStateChange(const uint8_t value)
{
    LOG_ERROR << __FUNCTION__ << "value: " << value;
    const std::unique_lock<std::mutex> lock_client{client_map_mutex_};
    for (const auto &it : client_map_)
    {
        const auto client = it.first;
        const auto specifier = it.second;
        if (specifier.compare(authentication_status_specifier_) == 0)
        {
            const std::vector<uint8_t> buffer{value};
            (void)SendAuthenticationMessage(client, AuthMsgType::kUpdateAuthStatus, buffer);
        }
    }
}

uint8_t Authentication::GetAuthenticationResult()
{
    LOG_INFO << __FUNCTION__ << "in! ";
    const uint8_t authentication_state = GetAuthenticationState();
    if (authentication_state == AuthenticationState::kSuccess)
    {
        return AuthenticateResult::kAuthenticateSuccess;
    }
    else if (authentication_state == AuthenticationState::kInitial)
    {
        return AuthenticateResult::kAuthenticateInitial;
    }
    else
    {
        return AuthenticateResult::kAuthenticateFailure;
    }
}

void Authentication::UpdateAuthenticationExemption(const uint8_t value)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    SetAuthenticationExemption(value);
    if (value == FactoryMode::kFactoryMode)
    {
        if (GetAuthenticationState() != AuthenticationState::kInitial)
        {
            SetAuthenticationState(AuthenticationState::kInitial);
        }
        NotifyAuthenticationState(FactoryMode::kFactoryMode);
    }
    else
    {
        if (GetAuthenticationState() == AuthenticationState::kInitial)
        {
            NotifyAuthenticationState(FactoryMode::kInitialMode);
        }
    }
}

bool Authentication::AuthenticationExemption()
{
    if (GetAuthenticationExemption() == FactoryMode::kFactoryMode)
    {
        LOG_INFO << __FUNCTION__ << "kFactoryMode! ";
        return true;
    }
    if (GetAuthenticationMode() == AuthenticationMode::kAuthClose)
    {
        LOG_INFO << __FUNCTION__ << "kAuthClose! ";
        return true;
    }
    return false;
}

void Authentication::TargetExemption(const bool value)
{
    target_exemption_.store(value, std::memory_order_release);
}

bool Authentication::TargetExemption()
{
    return target_exemption_.load(std::memory_order_acquire);
}

void Authentication::StartTargetExemption()
{
    LOG_INFO << __FUNCTION__ << "in! ";
    TargetExemption(true);
    if (GetAuthenticationState() == AuthenticationState::kInitial)
    {
        LOG_ERROR << __FUNCTION__ << "kDoipDiagProcessing! ";
        NotifyAuthenticationStateChange(OBDTester::kDoipDiagProcessing);
    }
    authentication_timer_->StartTargetExemptionTimer();
}

void Authentication::TargetExemptionTimeout()
{
    LOG_INFO << __FUNCTION__ << "in! ";
    TargetExemption(false);
    if (GetAuthenticationExemption() == FactoryMode::kFactoryMode)
    {
        LOG_INFO << __FUNCTION__ << "kFactoryMode! ";
        NotifyAuthenticationStateChange(FactoryMode::kFactoryMode);
        return;
    }
    if (GetAuthenticationState() == AuthenticationState::kInitial)
    {
        LOG_INFO << __FUNCTION__ << "kInitial! ";
        NotifyAuthenticationStateChange(AuthenticationState::kInitial);
    }
}

void Authentication::SetAuthenticationResult(const uint8_t conversation_id, const uint8_t result)
{
    const auto cid = GetAuthenticationCid();
    if (cid != conversation_id)
    {
        LOG_WARN << __FUNCTION__ << "conversation_id: " << conversation_id << "mismatched: " << cid;
        return;
    }
    if (GetAuthenticationState() != result)
    {
        UpdateAuthenticationState(result);
    }
    if (GetAuthenticationState() == AuthenticationState::kInitial)
    {
        SetAuthenticationCid(0U);
    }
}

void Authentication::DealAuthenticationMessage(const std::vector<uint8_t>& data)
{
    LOG_INFO << __FUNCTION__ << "in! ";
    {
        const std::unique_lock<std::mutex> lock_client{client_map_mutex_};
        for (const auto &it : client_map_)
        {
            const auto client = it.first;
            const auto specifier = it.second;
            if (specifier.compare(authentication_specifier_) == 0)
            {
                if (SendAuthenticationMessage(client, AuthMsgType::kAuthRequest, data))
                {
                    return;
                }
            }
        }
    }
    LOG_ERROR << __FUNCTION__ << "Authentication Server error! ";
    const std::vector<uint8_t> payload{0x7f, UdsServices::kAuthentication, UdsNegativeResponseCode::kGeneralReject};
    SendAuthenticationResponse(payload);
}

void Authentication::HandleAuthenticationMessage(const uint8_t error_code, const std::vector<uint8_t>& data)
{
    std::vector<uint8_t> payload;
    if (error_code != 0)
    {
        (void)payload.assign({0x7f, UdsServices::kAuthentication, error_code});
    }
    else
    {
        (void)payload.assign(data.begin(), data.end());
    }
    SendAuthenticationResponse(payload);
}

void Authentication::SendAuthenticationResponse(const std::vector<uint8_t>& data)
{
    const std::lock_guard<std::mutex> locker(mutex_);
    if (callback_ != nullptr)
    {
        callback_(data);
    }
}

void Authentication::RegisterAuthenticationResponseCallback(const AuthenticationResponseCallback fun)
{
    const std::lock_guard<std::mutex> locker(mutex_);
    callback_ = fun;
}

} /* namespace authentication */
} /* namespace dcm */
} /* namespace diag */
} /* namespace asf */
