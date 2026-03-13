#include "async_uds_interface.h"

#include "uds_packet.h"

#include "uds_common.h"

#include "uds_processor_manager.h"

#include "async_uds_response_processor_manager.h"

#include "log.h"

namespace midware
{
namespace diag
{

AsyncUdsInterface::AsyncUdsInterface(const uint16_t diagnostic_address)
    : diagnostic_address_(diagnostic_address)
{
    const auto processor = UdsProcessorManager::GetInstance().FindOrCreateUdsProcessorParallel(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUdsResponseCallback([this](const uint16_t source_address, const std::vector<uint8_t>& message)
                                               { HandleMessage(source_address, message); });
    }
}

void AsyncUdsInterface::Offer() const
{
    LOG_ERROR << __FUNCTION__ << "address: " << LOG_HEX(diagnostic_address_);
    const auto processor = UdsProcessorManager::GetInstance().FindOrCreateUdsProcessorParallel(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->Start();
    }
    UdsProcessorManager::GetInstance().SetUdsProcessorType(diagnostic_address_, common::ProcessorType::kParallel);
}

void AsyncUdsInterface::StopOffer() const
{
    LOG_ERROR << __FUNCTION__ << "address: " << LOG_HEX(diagnostic_address_);
    const auto processor = UdsProcessorManager::GetInstance().FindOrCreateUdsProcessorParallel(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->Stop();
    }
}

bool AsyncUdsInterface::RequestMessage(const uint16_t target_address, const std::vector<uint8_t>& message) const
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(diagnostic_address_) << "ta: " << LOG_HEX(target_address);
    const auto processor = UdsProcessorManager::GetInstance().FindOrCreateUdsProcessorParallel(diagnostic_address_);
    if (processor != nullptr)
    {
        if (processor->UdsRequest(target_address, message))
        {
            return true;
        }
    }
    return false;
}

void AsyncUdsInterface::HandleMessage(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    LOG_ERROR << __FUNCTION__ << "sa: " << LOG_HEX(source_address) << "ta: " << LOG_HEX(diagnostic_address_);
    if (message.size() < 1U)
    {
        LOG_ERROR << __FUNCTION__ << "payload size error! ";
        return;
    }
    const uint8_t sid = message.at(0U);
    if (sid == kUdsNackSid)
    {
        if ((message.size() != 3))
        {
            LOG_ERROR << __FUNCTION__ << "negative response size error! size: " << message.size();
            return;
        }
        ParseNegativeResponse(source_address, message);
        return;
    }
    LOG_INFO << __FUNCTION__ << "positive response sid: " << LOG_HEX(sid);
    switch (static_cast<uint32_t>(sid))
    {
    case UdsServiceID::ResponseSid::kRSessionControl:
        {
            ParseUds0x10Response(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRSecurityAccess:
        {
            ParseUds0x27Response(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kREcuResResponset:
        {
            ParseUds0x11Response(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRCommunicationControl:
        {
            ParseUds0x28Response(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRTesterPresent:
        {
            ParseUds0x3EResponse(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRReadDataByIndentifier:
        {
            ParseUds0x22Response(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRWriteDataByIdentifier:
        {
            ParseUds0x2EResponse(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRRoutineControResponsel:
        {
            ParseUds0x31Response(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRRequestDownload:
        {
            ParseUds0x34Response(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRRequestUpload:
        {
            ParseUds0x35Response(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRTransferData:
        {
            ParseUds0x36Response(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRTransferExit:
        {
            ParseUds0x37Response(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRTequestFileTransfer:
        {
            ParseUds0x38Response(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRAuthentication:
        {
            if (message.size() < 2U)
            {
                LOG_ERROR << __FUNCTION__ << "payload size error! ";
                break;
            }
            const uint8_t sub_function = message.at(1U);
            switch (static_cast<uint32_t>(sub_function))
            {
            case UdsSubFunction::Authentication::kDeAuthenticate:
                {
                    ParseUds0x2900Response(source_address, message);
                }
                break;
            case UdsSubFunction::Authentication::kVerifyCertificateUnidirectional:
                {
                    ParseUds0x2901Response(source_address, message);
                }
                break;
            case UdsSubFunction::Authentication::kVerifyCertificateBidirectional:
                {
                    ParseUds0x2902Response(source_address, message);
                }
                break;
            case UdsSubFunction::Authentication::kProofOfOwnership:
                {
                    ParseUds0x2903Response(source_address, message);
                }
                break;
            case UdsSubFunction::Authentication::kTransmitCertificate:
                {
                    ParseUds0x2904Response(source_address, message);
                }
                break;
            case UdsSubFunction::Authentication::kAuthenticationConfiguration:
                {
                    ParseUds0x2908Response(source_address, message);
                }
                break;
            default:
                {
                    LOG_ERROR << __FUNCTION__ << "unknown sub function: " << LOG_HEX(sub_function);
                }
                break;
            }
        }
        break;
    case UdsServiceID::ResponseSid::kRClearDiagnosticInformation:
        {
            ParseUds0x14Response(source_address, message);
        }
        break;
    case UdsServiceID::ResponseSid::kRReadDTCInformation:
        {
            if (message.size() < 2U)
            {
                LOG_ERROR << __FUNCTION__ << "payload size error! ";
                break;
            }
            const uint8_t sub_function = message.at(1U);
            switch (static_cast<uint32_t>(sub_function))
            {
            case UdsSubFunction::ReadDTCInformation::kReportNumberOfDTCByStatusMask:
                {
                    ParseUds0x1901Response(source_address, message);
                }
                break;
            case UdsSubFunction::ReadDTCInformation::kReportDTCByStatusMask:
                {
                    ParseUds0x1902Response(source_address, message);
                }
                break;
            case UdsSubFunction::ReadDTCInformation::kReportDTCSnapshotIdentification:
                {
                    ParseUds0x1903Response(source_address, message);
                }
                break;
            case UdsSubFunction::ReadDTCInformation::kReportDTCSnapshotRecordByDTCNumber:
                {
                    ParseUds0x1904Response(source_address, message);
                }
                break;
            case UdsSubFunction::ReadDTCInformation::kReportDTCExtDataRecordByDTCNumber:
                {
                    ParseUds0x1906Response(source_address, message);
                }
                break;
            case UdsSubFunction::ReadDTCInformation::kReportSupportedDTC:
                {
                    ParseUds0x190AResponse(source_address, message);
                }
                break;
            default:
                {
                    LOG_ERROR << __FUNCTION__ << "unknown sub function: " << LOG_HEX(sub_function);
                }
                break;
            }
        }
        break;
    case UdsServiceID::ResponseSid::kRControlDTCSetting:
        {
            ParseUds0x85Response(source_address, message);
        }
        break;
    default:
        {
            LOG_ERROR << __FUNCTION__ << "unknown sid: " << LOG_HEX(sid);
        }
        break;
    }
}

void AsyncUdsInterface::ParseUds0x10Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x10ResponseStruct response{};
    uds >> response.sid >> response.sub_function >> response.p2_server_max >> response.p2_star_server_max;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x10Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x27Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x27ResponseStruct response{};
    uds >> response.sid >> response.sub_function >> response.data;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x27Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x11Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x11ResponseStruct response{};
    uds >> response.sid >> response.sub_function;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x11Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x28Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x28ResponseStruct response{};
    uds >> response.sid >> response.sub_function;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x28Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x3EResponse(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x3EResponseStruct response{};
    uds >> response.sid >> response.sub_function;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x3EResponse(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x22Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x22ResponseStruct response{};
    uds >> response.sid >> response.did >> response.data;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x22Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x2EResponse(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x2EResponseStruct response{};
    uds >> response.sid >> response.did;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x2EResponse(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x31Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x31ResponseStruct response{};
    uds >> response.sid >> response.sub_function >> response.routine_identifier >> response.routine_status_record;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x31Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x34Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x34ResponseStruct response{};
    uds >> response.sid >> response.length_format_identifier >> response.max_number_of_block_length;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x34Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x35Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x35ResponseStruct response{};
    uds >> response.sid >> response.length_format_identifier >> response.max_number_of_block_length;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x35Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x36Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x36ResponseStruct response{};
    uds >> response.sid >> response.block_sequence_counter >> response.transfer_response_parameter_record;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x36Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x37Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x37ResponseStruct response{};
    uds >> response.sid;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x37Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x38Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x38ResponseStruct response{};
    uds >> response.sid >> response.mode_of_operation;
    if (response.mode_of_operation != 0x02U)
    {
        uds >> response.length_format_identifier >> UdsPacket::os_vec_type(response.max_number_of_block_length, response.length_format_identifier) >> response.data_format_identifier;
    }
    if ((response.mode_of_operation != 0x01U) && (response.mode_of_operation != 0x02U) && (response.mode_of_operation != 0x03U) && (response.mode_of_operation != 0x06U))
    {
        uds >> response.file_size_or_dir_info_parameter_length >> UdsPacket::os_vec_type(response.file_size_uncompressed_or_dir_info_length, response.file_size_or_dir_info_parameter_length);
    }
    if ((response.mode_of_operation != 0x01U) && (response.mode_of_operation != 0x02U) && (response.mode_of_operation != 0x03U) && (response.mode_of_operation != 0x05U) && (response.mode_of_operation != 0x06U))
    {
        uds >> UdsPacket::os_vec_type(response.file_size_compressed, response.file_size_or_dir_info_parameter_length);
    }
    if ((response.mode_of_operation != 0x01U) && (response.mode_of_operation != 0x02U) && (response.mode_of_operation != 0x03U) && (response.mode_of_operation != 0x04U) && (response.mode_of_operation != 0x05U))
    {
        uds >> UdsPacket::os_vec_type(response.file_position, kFilePositionSize);
    }

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x38Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x2900Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x29ResponseStruct response{};
    uds >> response.sid >> response.sub_function >> response.return_value;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x2900Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x2901Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x2901ResponseStruct response{};
    uds >> response.sid >> response.sub_function >> response.return_value
        >> response.length_of_challenge_server >> UdsPacket::os_vec_type(response.challenge_server, response.length_of_challenge_server)
        >> response.length_of_ephemeral_public_key_server;
    if (static_cast<bool>(response.length_of_ephemeral_public_key_server))
    {
        uds >> response.ephemeral_public_key_server;
    }

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x2901Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x2902Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x2902ResponseStruct response{};
    uds >> response.sid >> response.sub_function >> response.return_value
        >> response.length_of_challenge_server >> UdsPacket::os_vec_type(response.challenge_server, response.length_of_challenge_server)
        >> response.length_of_certificate_server >> UdsPacket::os_vec_type(response.certificate_server, response.length_of_certificate_server)
        >> response.length_of_proof_of_ownership_server >> UdsPacket::os_vec_type(response.proof_of_ownership_server, response.length_of_proof_of_ownership_server)
        >> response.length_of_ephemeral_public_key_server;
    if (static_cast<bool>(response.length_of_ephemeral_public_key_server))
    {
        uds >> UdsPacket::os_vec_type(response.ephemeral_public_key_server, response.length_of_ephemeral_public_key_server);
    }

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x2902Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x2903Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x2903ResponseStruct response{};
    uds >> response.sid >> response.sub_function >> response.return_value
        >> response.length_of_session_key_info;
    if (static_cast<bool>(response.length_of_session_key_info))
    {
        uds >> UdsPacket::os_vec_type(response.session_key_info, response.length_of_session_key_info);
    }

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x2903Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x2904Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x29ResponseStruct response{};
    uds >> response.sid >> response.sub_function >> response.return_value;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x2904Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x2908Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x29ResponseStruct response{};
    uds >> response.sid >> response.sub_function >> response.return_value;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x2908Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x14Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x14ResponseStruct response{};
    uds >> response.sid;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x14Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x1901Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x1901ResponseStruct response{};
    uds >> response.sid >> response.report_type >> response.dtc_status_availability_mask >> response.dtc_format_identifier >> response.dtc_count;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x1901Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x1902Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x1902ResponseStruct response{};
    uds >> response.sid >> response.report_type >> response.dtc_status_availability_mask >> response.dtc_and_status_record;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x1902Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x1903Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x1903ResponseStruct response{};
    uds >> response.sid >> response.report_type >> response.dtc_record;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x1903Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x1904Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x1904ResponseStruct response{};
    uds >> response.sid >> response.report_type >> UdsPacket::os_vec_type(response.dtc_and_status_record, kDtcSize + 1U) >> response.dtc_snapshot_record;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x1904Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x1906Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x1906ResponseStruct response{};
    uds >> response.sid >> response.report_type >> UdsPacket::os_vec_type(response.dtc_and_status_record, kDtcSize + 1U) >> response.dtc_ext_data_record;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x1906Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x190AResponse(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x190AResponseStruct response{};
    uds >> response.sid >> response.report_type >> response.dtc_status_availability_mask >> response.dtc_and_status_record;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x190AResponse(source_address, response);
    }
}

void AsyncUdsInterface::ParseUds0x85Response(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    UdsPacket uds{message};
    Uds0x85ResponseStruct response{};
    uds >> response.sid >> response.dtc_setting_type;

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleUds0x85Response(source_address, response);
    }
}

void AsyncUdsInterface::ParseNegativeResponse(const uint16_t source_address, const std::vector<uint8_t>& message)
{
    NegativeResponseStruct negative_response{};
    negative_response.sid = message.at(1U);
    negative_response.nrc = message.at(2U);

    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->HandleNegativeResponse(source_address, negative_response);
    }
}

bool AsyncUdsInterface::Uds0x10Request(const uint16_t target_address, const Uds0x10RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kSessionControl)
        << request.sub_function;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x27Request(const uint16_t target_address, const Uds0x27RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kSecurityAccess)
        << request.sub_function << request.data;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x11Request(const uint16_t target_address, const Uds0x11RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kEcuRest)
        << request.sub_function;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x28Request(const uint16_t target_address, const Uds0x28RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kCommunicationControl)
        << request.sub_function << request.communication_type;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x3ERequest(const uint16_t target_address, const Uds0x3ERequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kTesterPresent)
        << request.sub_function;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x22Request(const uint16_t target_address, const Uds0x22RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDataByIndentifier)
        << request.did;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x2ERequest(const uint16_t target_address, const Uds0x2ERequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kWriteDataByIdentifier)
        << request.did << request.data_record;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x31Request(const uint16_t target_address, const Uds0x31RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kRoutineControl)
        << request.sub_function << request.routine_identifier << request.routine_control_option_record;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x34Request(const uint16_t target_address, const Uds0x34RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kRequestDownload)
        << request.data_format_identifier << request.address_and_length_format_identifier << request.memory_address << request.memory_size;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x35Request(const uint16_t target_address, const Uds0x35RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kRequestUpload)
        << request.data_format_identifier << request.address_and_length_format_identifier << request.memory_address << request.memory_size;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x36Request(const uint16_t target_address, const Uds0x36RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kTransferData)
        << request.block_sequence_counter << request.transfer_request_parameter_record;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x37Request(const uint16_t target_address)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kTransferExit);
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x38Request(const uint16_t target_address, const Uds0x38RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kTequestFileTransfer)
        << request.mode_of_operation << request.file_path_and_name_length << request.file_path_and_name;
    if ((request.mode_of_operation != 0x02U) && (request.mode_of_operation != 0x05U))
    {
        uds << request.data_format_identifier;
    }
    if ((request.mode_of_operation != 0x02U) && (request.mode_of_operation != 0x04U) && (request.mode_of_operation != 0x05U))
    {
        uds << request.file_size_parameter_length << request.file_size_uncompressed << request.file_size_compressed;
    }
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x2900Request(const uint16_t target_address)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kAuthentication) << static_cast<uint8_t>(UdsSubFunction::Authentication::kDeAuthenticate);
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x2901Request(const uint16_t target_address, const Uds0x2901RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kAuthentication) << static_cast<uint8_t>(UdsSubFunction::Authentication::kVerifyCertificateUnidirectional)
        << request.communication_configuration << request.length_of_certificate_client << request.certificate_client << request.length_of_challenge_client << request.challenge_client;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x2902Request(const uint16_t target_address, const Uds0x2902RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kAuthentication) << static_cast<uint8_t>(UdsSubFunction::Authentication::kVerifyCertificateBidirectional)
        << request.communication_configuration << request.length_of_certificate_client << request.certificate_client << request.length_of_challenge_client << request.challenge_client;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x2903Request(const uint16_t target_address, const Uds0x2903RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kAuthentication) << static_cast<uint8_t>(UdsSubFunction::Authentication::kProofOfOwnership)
        << request.length_of_proof_of_ownership_client << request.proof_of_ownership_client << request.length_of_ephemeral_public_key_client << request.ephemeral_public_key_client;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x2904Request(const uint16_t target_address, const Uds0x2904RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kAuthentication) << static_cast<uint8_t>(UdsSubFunction::Authentication::kTransmitCertificate)
        << request.certificate_evaluation_id << request.length_of_certificate_data << request.certificate_data;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x2908Request(const uint16_t target_address)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kAuthentication) << static_cast<uint8_t>(UdsSubFunction::Authentication::kAuthenticationConfiguration);
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x14Request(const uint16_t target_address, const Uds0x14RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kClearDiagnosticInformation)
        << request.group_of_dtc;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x1901Request(const uint16_t target_address, const Uds0x1901RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDTCInformation) << static_cast<uint8_t>(UdsSubFunction::ReadDTCInformation::kReportNumberOfDTCByStatusMask)
        << request.dtc_status_mask;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x1902Request(const uint16_t target_address, const Uds0x1902RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDTCInformation) << static_cast<uint8_t>(UdsSubFunction::ReadDTCInformation::kReportDTCByStatusMask)
        << request.dtc_status_mask;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x1903Request(const uint16_t target_address)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDTCInformation) << static_cast<uint8_t>(UdsSubFunction::ReadDTCInformation::kReportDTCSnapshotIdentification);
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x1904Request(const uint16_t target_address, const Uds0x1904RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDTCInformation) << static_cast<uint8_t>(UdsSubFunction::ReadDTCInformation::kReportDTCSnapshotRecordByDTCNumber)
        << request.dtc_mask_record << request.dtc_snapshot_record_number;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x1906Request(const uint16_t target_address, const Uds0x1906RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDTCInformation) << static_cast<uint8_t>(UdsSubFunction::ReadDTCInformation::kReportDTCExtDataRecordByDTCNumber)
        << request.dtc_mask_record << request.dtc_ext_data_record_number;
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x190ARequest(const uint16_t target_address)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kReadDTCInformation) << static_cast<uint8_t>(UdsSubFunction::ReadDTCInformation::kReportSupportedDTC);
    return RequestMessage(target_address, uds.Packet());
}

bool AsyncUdsInterface::Uds0x85Request(const uint16_t target_address, const Uds0x85RequestStruct& request)
{
    UdsPacket uds{};
    uds << static_cast<uint8_t>(UdsServiceID::RequestSid::kControlDTCSetting)
        << request.sub_function << request.dtc_setting_control_option_record;
    return RequestMessage(target_address, uds.Packet());
}

void AsyncUdsInterface::RegisterUds0x10ResponseCallback(const Uds0x10ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x10Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x27ResponseCallback(const Uds0x27ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x27Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x11ResponseCallback(const Uds0x11ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x11Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x28ResponseCallback(const Uds0x28ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x28Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x3EResponseCallback(const Uds0x3EResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x3ECallback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x22ResponseCallback(const Uds0x22ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x22Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x2EResponseCallback(const Uds0x2EResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x2ECallback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x31ResponseCallback(const Uds0x31ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x31Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x34ResponseCallback(const Uds0x34ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x34Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x35ResponseCallback(const Uds0x35ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x35Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x36ResponseCallback(const Uds0x36ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x36Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x37ResponseCallback(const Uds0x37ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x37Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x38ResponseCallback(const Uds0x38ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x38Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x2900ResponseCallback(const Uds0x2900ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x2900Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x2901ResponseCallback(const Uds0x2901ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x2901Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x2902ResponseCallback(const Uds0x2902ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x2902Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x2903ResponseCallback(const Uds0x2903ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x2903Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x2904ResponseCallback(const Uds0x2904ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x2904Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x2908ResponseCallback(const Uds0x2908ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x2908Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x14ResponseCallback(const Uds0x14ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x14Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x1901ResponseCallback(const Uds0x1901ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x1901Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x1902ResponseCallback(const Uds0x1902ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x1902Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x1903ResponseCallback(const Uds0x1903ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x1903Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x1904ResponseCallback(const Uds0x1904ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x1904Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x1906ResponseCallback(const Uds0x1906ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x1906Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x190AResponseCallback(const Uds0x190AResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x190ACallback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterUds0x85ResponseCallback(const Uds0x85ResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterUds0x85Callback(fun, target_address);
    }
}

void AsyncUdsInterface::RegisterNegativeResponseCallback(const NegativeResponseCallback fun, const uint16_t target_address)
{
    const auto processor = AsyncResponseProcessorManager::GetInstance().FindOrCreateAsyncResponseProcessor(diagnostic_address_);
    if (processor != nullptr)
    {
        processor->RegisterNegativeCallback(fun, target_address);
    }
}


}
}
