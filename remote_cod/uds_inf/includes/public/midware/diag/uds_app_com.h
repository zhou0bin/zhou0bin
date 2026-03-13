#ifndef UDS_INF_UDS_APP_COM_H__
#define UDS_INF_UDS_APP_COM_H__

#include <cstdint>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <memory>
#include <map>
#include <vector>
#include <queue>
#include <tuple>
#include <thread>
#include <functional>

using Uds0x10ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x10ResponseStruct& response)>;
using Uds0x27ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x27ResponseStruct& response)>;
using Uds0x11ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x11ResponseStruct& response)>;
using Uds0x28ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x28ResponseStruct& response)>;
using Uds0x3EResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x3EResponseStruct& response)>;
using Uds0x22ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x22ResponseStruct& response)>;
using Uds0x2EResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x2EResponseStruct& response)>;
using Uds0x31ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x31ResponseStruct& response)>;
using Uds0x34ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x34ResponseStruct& response)>;
using Uds0x35ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x35ResponseStruct& response)>;
using Uds0x36ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x36ResponseStruct& response)>;
using Uds0x37ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x37ResponseStruct& response)>;
using Uds0x38ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x38ResponseStruct& response)>;
using Uds0x2900ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x29ResponseStruct& response)>;
using Uds0x2901ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x2901ResponseStruct& response)>;
using Uds0x2902ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x2902ResponseStruct& response)>;
using Uds0x2903ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x2903ResponseStruct& response)>;
using Uds0x2904ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x29ResponseStruct& response)>;
using Uds0x2908ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x29ResponseStruct& response)>;
using Uds0x14ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x14ResponseStruct& response)>;
using Uds0x1901ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x1901ResponseStruct& response)>;
using Uds0x1902ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x1902ResponseStruct& response)>;
using Uds0x1903ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x1903ResponseStruct& response)>;
using Uds0x1904ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x1904ResponseStruct& response)>;
using Uds0x1906ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x1906ResponseStruct& response)>;
using Uds0x190AResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x190AResponseStruct& response)>;
using Uds0x85ResponseCallback = std::function<void(const uint16_t source_address, struct Uds0x85ResponseStruct& response)>;
using NegativeResponseCallback = std::function<void(const uint16_t source_address, struct NegativeResponseStruct& response)>;

namespace midware
{
namespace diag
{
constexpr uint16_t kDefaultAddress {0U};
}
}

enum class UdsResult : uint8_t
{
    kRequestFailed = 0x00,
    kRequestSuccess = 0x01,
    kPositiveResponse = 0x02,
    kNegativeResponse = 0x03,
    kPendingOverrideMaxNum = 0x04,
    kResponseTimeout = 0x05,
    kUdsResponseMismatch = 0x06
};

struct Uds0x10RequestStruct
{
    uint8_t sub_function;
};

struct Uds0x27RequestStruct
{
    uint8_t sub_function;
    std::vector<uint8_t> data;
};

struct Uds0x11RequestStruct
{
    uint8_t sub_function;
};

struct Uds0x28RequestStruct
{
    uint8_t sub_function;
    uint8_t communication_type;
};

struct Uds0x3ERequestStruct
{
    uint8_t sub_function;
};

struct Uds0x22RequestStruct
{
    uint16_t did;
};

struct Uds0x2ERequestStruct
{
    uint16_t did;
    std::vector<uint8_t> data_record;
};

struct Uds0x31RequestStruct
{
    uint8_t sub_function;
    uint16_t routine_identifier;
    std::vector<uint8_t> routine_control_option_record;
};

struct Uds0x34RequestStruct
{
    uint8_t data_format_identifier;
    uint8_t address_and_length_format_identifier;
    std::vector<uint8_t> memory_address;
    std::vector<uint8_t> memory_size;
};

struct Uds0x35RequestStruct
{
    uint8_t data_format_identifier;
    uint8_t address_and_length_format_identifier;
    std::vector<uint8_t> memory_address;
    std::vector<uint8_t> memory_size;
};

struct Uds0x36RequestStruct
{
    uint8_t block_sequence_counter;
    std::vector<uint8_t> transfer_request_parameter_record;
};

struct Uds0x38RequestStruct
{
    uint8_t mode_of_operation;
    uint16_t file_path_and_name_length;
    std::vector<uint8_t> file_path_and_name;
    uint8_t data_format_identifier;
    uint8_t file_size_parameter_length;
    std::vector<uint8_t> file_size_uncompressed;
    std::vector<uint8_t> file_size_compressed;
};

struct Uds0x2901RequestStruct
{
    uint8_t communication_configuration;
    uint16_t length_of_certificate_client;
    std::vector<uint8_t> certificate_client;
    uint16_t length_of_challenge_client;
    std::vector<uint8_t> challenge_client;
};

struct Uds0x2902RequestStruct
{
    uint8_t communication_configuration;
    uint16_t length_of_certificate_client;
    std::vector<uint8_t> certificate_client;
    uint16_t length_of_challenge_client;
    std::vector<uint8_t> challenge_client;
};

struct Uds0x2903RequestStruct
{
    uint16_t length_of_proof_of_ownership_client;
    std::vector<uint8_t> proof_of_ownership_client;
    uint16_t length_of_ephemeral_public_key_client;
    std::vector<uint8_t> ephemeral_public_key_client;
};

struct Uds0x2904RequestStruct
{
    uint16_t certificate_evaluation_id;
    uint16_t length_of_certificate_data;
    std::vector<uint8_t> certificate_data;
};

struct Uds0x14RequestStruct
{
    std::vector<uint8_t> group_of_dtc;
};

struct Uds0x1901RequestStruct
{
    uint8_t dtc_status_mask;
};

struct Uds0x1902RequestStruct
{
    uint8_t dtc_status_mask;
};

struct Uds0x1904RequestStruct
{
    std::vector<uint8_t> dtc_mask_record;
    uint8_t dtc_snapshot_record_number;
};

struct Uds0x1906RequestStruct
{
    std::vector<uint8_t> dtc_mask_record;
    uint8_t dtc_ext_data_record_number;
};

struct Uds0x85RequestStruct
{
    uint8_t sub_function;
    std::vector<uint8_t> dtc_setting_control_option_record;
};

struct Uds0x10ResponseStruct
{
    uint8_t sid;
    uint8_t sub_function;
    uint16_t p2_server_max;
    uint16_t p2_star_server_max;
};

struct Uds0x27ResponseStruct
{
    uint8_t sid;
    uint8_t sub_function;
    std::vector<uint8_t> data;
};

struct Uds0x11ResponseStruct
{
    uint8_t sid;
    uint8_t sub_function;
};

struct Uds0x28ResponseStruct
{
    uint8_t sid;
    uint8_t sub_function;
};

struct Uds0x3EResponseStruct
{
    uint8_t sid;
    uint8_t sub_function;
};

struct Uds0x22ResponseStruct
{
    uint8_t sid;
    uint16_t did;
    std::vector<uint8_t> data;
};

struct Uds0x2EResponseStruct
{
    uint8_t sid;
    uint16_t did;
};

struct Uds0x31ResponseStruct
{
    uint8_t sid;
    uint8_t sub_function;
    uint16_t routine_identifier;
    std::vector<uint8_t> routine_status_record;
};

struct Uds0x34ResponseStruct
{
    uint8_t sid;
    uint8_t length_format_identifier;
    std::vector<uint8_t> max_number_of_block_length;
};

struct Uds0x35ResponseStruct
{
    uint8_t sid;
    uint8_t length_format_identifier;
    std::vector<uint8_t> max_number_of_block_length;
};

struct Uds0x36ResponseStruct
{
    uint8_t sid;
    uint8_t block_sequence_counter;
    std::vector<uint8_t> transfer_response_parameter_record;
};

struct Uds0x37ResponseStruct
{
    uint8_t sid;
};

struct Uds0x38ResponseStruct
{
    uint8_t sid;
    uint8_t mode_of_operation;
    uint8_t length_format_identifier;
    std::vector<uint8_t> max_number_of_block_length;
    uint8_t data_format_identifier;
    uint16_t file_size_or_dir_info_parameter_length;
    std::vector<uint8_t> file_size_uncompressed_or_dir_info_length;
    std::vector<uint8_t> file_size_compressed;
    std::vector<uint8_t> file_position;
};

struct Uds0x29ResponseStruct
{
    uint8_t sid;
    uint8_t sub_function;
    uint8_t return_value;
};

struct Uds0x2901ResponseStruct
{
    uint8_t sid;
    uint8_t sub_function;
    uint8_t return_value;
    uint16_t length_of_challenge_server;
    std::vector<uint8_t> challenge_server;
    uint16_t length_of_ephemeral_public_key_server;
    std::vector<uint8_t> ephemeral_public_key_server;
};

struct Uds0x2902ResponseStruct
{
    uint8_t sid;
    uint8_t sub_function;
    uint8_t return_value;
    uint16_t length_of_challenge_server;
    std::vector<uint8_t> challenge_server;
    uint16_t length_of_certificate_server;
    std::vector<uint8_t> certificate_server;
    uint16_t length_of_proof_of_ownership_server;
    std::vector<uint8_t> proof_of_ownership_server;
    uint16_t length_of_ephemeral_public_key_server;
    std::vector<uint8_t> ephemeral_public_key_server;
};

struct Uds0x2903ResponseStruct
{
    uint8_t sid;
    uint8_t sub_function;
    uint8_t return_value;
    uint16_t length_of_session_key_info;
    std::vector<uint8_t> session_key_info;
};

struct Uds0x14ResponseStruct
{
    uint8_t sid;
};

struct Uds0x1901ResponseStruct
{
    uint8_t sid;
    uint8_t report_type;
    uint8_t dtc_status_availability_mask;
    uint8_t dtc_format_identifier;
    uint16_t dtc_count;
};

struct Uds0x1902ResponseStruct
{
    uint8_t sid;
    uint8_t report_type;
    uint8_t dtc_status_availability_mask;
    std::vector<uint8_t> dtc_and_status_record;
};

struct Uds0x1903ResponseStruct
{
    uint8_t sid;
    uint8_t report_type;
    std::vector<uint8_t> dtc_record;
};

struct Uds0x1904ResponseStruct
{
    uint8_t sid;
    uint8_t report_type;
    std::vector<uint8_t> dtc_and_status_record;
    std::vector<uint8_t> dtc_snapshot_record;
};

struct Uds0x1906ResponseStruct
{
    uint8_t sid;
    uint8_t report_type;
    std::vector<uint8_t> dtc_and_status_record;
    std::vector<uint8_t> dtc_ext_data_record;
};

struct Uds0x190AResponseStruct
{
    uint8_t sid;
    uint8_t report_type;
    uint8_t dtc_status_availability_mask;
    std::vector<uint8_t> dtc_and_status_record;
};

struct Uds0x85ResponseStruct
{
    uint8_t sid;
    uint8_t dtc_setting_type;
};

struct NegativeResponseStruct
{
    uint8_t sid;
    uint8_t nrc;
};

#endif
