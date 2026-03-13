#include "sync_uds_interface.h"

#include "uds_packet.h"

#include "uds_common.h"

#include "uds_processor_manager.h"

#include "log.h"

namespace midware
{
namespace diag
{

SyncUdsInterface::SyncUdsInterface(const uint16_t diagnostic_address)
    : diagnostic_address_(diagnostic_address)
{
}

void SyncUdsInterface::Offer() const
{
    LOG_ERROR << __FUNCTION__ << "address: " << LOG_HEX(diagnostic_address_);
    const auto processor = UdsProcessorManager::GetInstance().FindOrCreateUdsProcessorSerial(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->Start();
    }
    UdsProcessorManager::GetInstance().SetUdsProcessorType(diagnostic_address_, common::ProcessorType::kSerial);
}

void SyncUdsInterface::StopOffer() const
{
    LOG_ERROR << __FUNCTION__ << "address: " << LOG_HEX(diagnostic_address_);
    const auto processor = UdsProcessorManager::GetInstance().FindOrCreateUdsProcessorSerial(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->Stop();
    }
}

void SyncUdsInterface::SetTimeoutPeriod(const uint16_t p2, const uint16_t p2_star, const uint16_t max_num_of_pending)
{
    LOG_INFO << __FUNCTION__ << "p2: " << static_cast<unsigned>(p2) << "p2_star: " << static_cast<unsigned>(p2_star) << "max_num_of_pending: " << static_cast<unsigned>(max_num_of_pending);
    const auto processor = UdsProcessorManager::GetInstance().FindOrCreateUdsProcessorSerial(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->SetTimeoutPeriod(p2, p2_star * kUdsP2StarResolution, max_num_of_pending);
    }
}

UdsResult SyncUdsInterface::RequestMessage(const uint16_t target_address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response) const
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(target_address);
    UdsResult result = UdsResult::kRequestFailed;
    const auto processor = UdsProcessorManager::GetInstance().FindOrCreateUdsProcessorSerial(diagnostic_address_);
    if (processor != nullptr)
    {
        result = processor->UdsRequest(target_address, request, response);
    }
    LOG_ERROR << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    return result;
}

UdsResult SyncUdsInterface::RequestMessage(const uint16_t target_address, const std::vector<uint8_t>& request) const
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(target_address);
    UdsResult result = UdsResult::kRequestFailed;
    const auto processor = UdsProcessorManager::GetInstance().FindOrCreateUdsProcessorSerial(diagnostic_address_);
    if (processor != nullptr)
    {
        result = processor->UdsRequest(target_address, request);
    }
    LOG_ERROR << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    return result;
}

UdsResult SyncUdsInterface::Uds0x10(const uint16_t target_address, const Uds0x10RequestStruct& request, Uds0x10ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kSessionControl)
                << request.sub_function;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x10ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.sub_function >> p_response.p2_server_max >> p_response.p2_star_server_max;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x27(const uint16_t target_address, const Uds0x27RequestStruct& request, Uds0x27ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kSecurityAccess)
                << request.sub_function << request.data;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x27ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.sub_function >> p_response.data;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x11(const uint16_t target_address, const Uds0x11RequestStruct& request, Uds0x11ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kEcuRest)
                << request.sub_function;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x11ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.sub_function;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x28(const uint16_t target_address, const Uds0x28RequestStruct& request, Uds0x28ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kCommunicationControl)
                << request.sub_function << request.communication_type;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x28ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.sub_function;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x3E(const uint16_t target_address, const Uds0x3ERequestStruct& request, Uds0x3EResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kTesterPresent)
                << request.sub_function;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x3EResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.sub_function;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x22(const uint16_t target_address, const Uds0x22RequestStruct& request, Uds0x22ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDataByIndentifier)
                << request.did;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x22ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.did >> p_response.data;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x2E(const uint16_t target_address, const Uds0x2ERequestStruct& request, Uds0x2EResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kWriteDataByIdentifier)
                << request.did << request.data_record;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x2EResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.did;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x31(const uint16_t target_address, const Uds0x31RequestStruct& request, Uds0x31ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kRoutineControl)
                << request.sub_function << request.routine_identifier << request.routine_control_option_record;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x31ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.sub_function >> p_response.routine_identifier >> p_response.routine_status_record;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x34(const uint16_t target_address, const Uds0x34RequestStruct& request, Uds0x34ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kRequestDownload)
                << request.data_format_identifier << request.address_and_length_format_identifier << request.memory_address << request.memory_size;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x34ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.length_format_identifier >> p_response.max_number_of_block_length;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x35(const uint16_t target_address, const Uds0x35RequestStruct& request, Uds0x35ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kRequestUpload)
                << request.data_format_identifier << request.address_and_length_format_identifier << request.memory_address << request.memory_size;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x35ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.length_format_identifier >> p_response.max_number_of_block_length;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x36(const uint16_t target_address, const Uds0x36RequestStruct& request, Uds0x36ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kTransferData)
                << request.block_sequence_counter << request.transfer_request_parameter_record;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x36ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.block_sequence_counter >> p_response.transfer_response_parameter_record;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x37(const uint16_t target_address, Uds0x37ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kTransferExit);
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x37ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x38(const uint16_t target_address, const Uds0x38RequestStruct& request, Uds0x38ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kTequestFileTransfer)
                << request.mode_of_operation << request.file_path_and_name_length << request.file_path_and_name;
    if ((request.mode_of_operation != 0x02U) && (request.mode_of_operation != 0x05U))
    {
        uds_request << request.data_format_identifier;
    }
    if ((request.mode_of_operation != 0x02U) && (request.mode_of_operation != 0x04U) && (request.mode_of_operation != 0x05U))
    {
        uds_request << request.file_size_parameter_length << request.file_size_uncompressed << request.file_size_compressed;
    }
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x38ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.mode_of_operation;
        if (p_response.mode_of_operation != 0x02U)
        {
            uds_response >> p_response.length_format_identifier >> UdsPacket::os_vec_type(p_response.max_number_of_block_length, p_response.length_format_identifier) >> p_response.data_format_identifier;
        }
        if ((p_response.mode_of_operation != 0x01U) && (p_response.mode_of_operation != 0x02U) && (p_response.mode_of_operation != 0x03U) && (p_response.mode_of_operation != 0x06U))
        {
            uds_response >> p_response.file_size_or_dir_info_parameter_length >> UdsPacket::os_vec_type(p_response.file_size_uncompressed_or_dir_info_length, p_response.file_size_or_dir_info_parameter_length);
        }
        if ((p_response.mode_of_operation != 0x01U) && (p_response.mode_of_operation != 0x02U) && (p_response.mode_of_operation != 0x03U) && (p_response.mode_of_operation != 0x05U) && (p_response.mode_of_operation != 0x06U))
        {
            uds_response >> UdsPacket::os_vec_type(p_response.file_size_compressed, p_response.file_size_or_dir_info_parameter_length);
        }
        if ((p_response.mode_of_operation != 0x01U) && (p_response.mode_of_operation != 0x02U) && (p_response.mode_of_operation != 0x03U) && (p_response.mode_of_operation != 0x04U) && (p_response.mode_of_operation != 0x05U))
        {
            uds_response >> UdsPacket::os_vec_type(p_response.file_position, kFilePositionSize);
        }
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x2900(const uint16_t target_address, Uds0x29ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kAuthentication) << static_cast<uint8_t>(UdsSubFunction::Authentication::kDeAuthenticate);
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x29ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.sub_function >> p_response.return_value;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x2901(const uint16_t target_address, const Uds0x2901RequestStruct& request, Uds0x2901ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kAuthentication) << static_cast<uint8_t>(UdsSubFunction::Authentication::kVerifyCertificateUnidirectional)
                << request.communication_configuration << request.length_of_certificate_client << request.certificate_client << request.length_of_challenge_client << request.challenge_client;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x2901ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.sub_function >> p_response.return_value
                     >> p_response.length_of_challenge_server >> UdsPacket::os_vec_type(p_response.challenge_server, p_response.length_of_challenge_server)
                     >> p_response.length_of_ephemeral_public_key_server;
        if (static_cast<bool>(p_response.length_of_ephemeral_public_key_server))
        {
            uds_response >> p_response.ephemeral_public_key_server;
        }
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x2902(const uint16_t target_address, const Uds0x2902RequestStruct& request, Uds0x2902ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kAuthentication) << static_cast<uint8_t>(UdsSubFunction::Authentication::kVerifyCertificateBidirectional)
                << request.communication_configuration << request.length_of_certificate_client << request.certificate_client << request.length_of_challenge_client << request.challenge_client;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x2902ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.sub_function >> p_response.return_value
                     >> p_response.length_of_challenge_server >> UdsPacket::os_vec_type(p_response.challenge_server, p_response.length_of_challenge_server)
                     >> p_response.length_of_certificate_server >> UdsPacket::os_vec_type(p_response.certificate_server, p_response.length_of_certificate_server)
                     >> p_response.length_of_proof_of_ownership_server >> UdsPacket::os_vec_type(p_response.proof_of_ownership_server, p_response.length_of_proof_of_ownership_server)
                     >> p_response.length_of_ephemeral_public_key_server;
        if (static_cast<bool>(p_response.length_of_ephemeral_public_key_server))
        {
            uds_response >> UdsPacket::os_vec_type(p_response.ephemeral_public_key_server, p_response.length_of_ephemeral_public_key_server);
        }
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x2903(const uint16_t target_address, const Uds0x2903RequestStruct& request, Uds0x2903ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kAuthentication) << static_cast<uint8_t>(UdsSubFunction::Authentication::kProofOfOwnership)
                << request.length_of_proof_of_ownership_client << request.proof_of_ownership_client << request.length_of_ephemeral_public_key_client << request.ephemeral_public_key_client;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x2903ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.sub_function >> p_response.return_value
                     >> p_response.length_of_session_key_info;
        if (static_cast<bool>(p_response.length_of_session_key_info))
        {
            uds_response >> UdsPacket::os_vec_type(p_response.session_key_info, p_response.length_of_session_key_info);
        }
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x2904(const uint16_t target_address, const Uds0x2904RequestStruct& request, Uds0x29ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kAuthentication) << static_cast<uint8_t>(UdsSubFunction::Authentication::kTransmitCertificate)
                << request.certificate_evaluation_id << request.length_of_certificate_data << request.certificate_data;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x29ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.sub_function >> p_response.return_value;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x2908(const uint16_t target_address, Uds0x29ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kAuthentication) << static_cast<uint8_t>(UdsSubFunction::Authentication::kAuthenticationConfiguration);
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x29ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.sub_function >> p_response.return_value;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x14(const uint16_t target_address, const Uds0x14RequestStruct& request, Uds0x14ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kClearDiagnosticInformation)
                << request.group_of_dtc;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x14ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x1901(const uint16_t target_address, const Uds0x1901RequestStruct& request, Uds0x1901ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDTCInformation) << static_cast<uint8_t>(UdsSubFunction::ReadDTCInformation::kReportNumberOfDTCByStatusMask)
                << request.dtc_status_mask;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x1901ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.report_type >> p_response.dtc_status_availability_mask >> p_response.dtc_format_identifier >> p_response.dtc_count;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x1902(const uint16_t target_address, const Uds0x1902RequestStruct& request, Uds0x1902ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDTCInformation) << static_cast<uint8_t>(UdsSubFunction::ReadDTCInformation::kReportDTCByStatusMask)
                << request.dtc_status_mask;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x1902ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.report_type >> p_response.dtc_status_availability_mask >> p_response.dtc_and_status_record;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x1903(const uint16_t target_address, Uds0x1903ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDTCInformation) << static_cast<uint8_t>(UdsSubFunction::ReadDTCInformation::kReportDTCSnapshotIdentification);
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x1903ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.report_type >> p_response.dtc_record;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x1904(const uint16_t target_address, const Uds0x1904RequestStruct& request, Uds0x1904ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDTCInformation) << static_cast<uint8_t>(UdsSubFunction::ReadDTCInformation::kReportDTCSnapshotRecordByDTCNumber)
                << request.dtc_mask_record << request.dtc_snapshot_record_number;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x1904ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.report_type >> UdsPacket::os_vec_type(p_response.dtc_and_status_record, kDtcSize + 1U) >> p_response.dtc_snapshot_record;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x1906(const uint16_t target_address, const Uds0x1906RequestStruct& request, Uds0x1906ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDTCInformation) << static_cast<uint8_t>(UdsSubFunction::ReadDTCInformation::kReportDTCExtDataRecordByDTCNumber)
                << request.dtc_mask_record << request.dtc_ext_data_record_number;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x1906ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.report_type >> UdsPacket::os_vec_type(p_response.dtc_and_status_record, kDtcSize + 1U) >> p_response.dtc_ext_data_record;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x190A(const uint16_t target_address, Uds0x190AResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDTCInformation) << static_cast<uint8_t>(UdsSubFunction::ReadDTCInformation::kReportSupportedDTC);
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x190AResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.report_type >> p_response.dtc_status_availability_mask >> p_response.dtc_and_status_record;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::Uds0x85(const uint16_t target_address, const Uds0x85RequestStruct& request, Uds0x85ResponseStruct& response, NegativeResponseStruct& negative_response)
{
    UdsPacket uds_request{};
    uds_request << static_cast<uint8_t>(UdsServiceID::RequestSid::kControlDTCSetting)
                << request.sub_function << request.dtc_setting_control_option_record;
    UdsPacket uds_response{};
    const UdsResult result = RequestMessage(target_address, uds_request.Packet(), uds_response.Packet());
    Uds0x85ResponseStruct p_response{};
    NegativeResponseStruct n_response{};
    if (result == UdsResult::kPositiveResponse)
    {
        uds_response >> p_response.sid >> p_response.dtc_setting_type;
    }
    else if (result == UdsResult::kNegativeResponse)
    {
        uds_response.PushSeek(1U);
        uds_response >> n_response.sid >> n_response.nrc;
    }
    else
    {
        LOG_DEBUG << __FUNCTION__ << "result: " << static_cast<unsigned>(result);
    }
    response = p_response;
    negative_response = n_response;
    return result;
}

UdsResult SyncUdsInterface::UdsCommon(const uint16_t target_address, const std::vector<uint8_t>& request, std::vector<uint8_t>& response)
{
    return RequestMessage(target_address, request, response);
}

}
}
