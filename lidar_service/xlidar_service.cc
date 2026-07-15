/******************************************************************************
 * Copyright 2025 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "onboard/drivers/lidar_service/xlidar_service.h"

#include <vector>

#include "onboard/common/util/http_client.h"

namespace autox::drivers::lidar_service {
XLidarService::XLidarService(const XLidarServiceConfig& config) {
  lidar_ip_ = config.lidar_config.ip();
  status_writer_ = config.status_writer;
  enable_status_recv_ = config.lidar_config.has_status_port();
  if (enable_status_recv_) {
    input_ = std::make_unique<Input>(config.lidar_config.status_port());
    if (!input_->isValid()) {
      return;
    }
  }
}

XLidarService::~XLidarService() {}

int XLidarService::GetLidarRotationSpeed() {
  struct MotorSpeed {
    uint8_t header[12];
    uint32_t speed;
  };

  const std::string url =
      "http://" + lidar_ip_ + "/getSReg-info?Gt0x08&addr=0xA0010084";
  uint64_t response_code;
  std::string result;

  AINFO << "Get xlidar rotation speed.";
  AINFO << "url : " << url;
  if (autox::common::util::HttpClient::Get(url, &response_code, &result, 1, 1,
                                           1)
          .ok()) {
    auto response = reinterpret_cast<const MotorSpeed*>(result.data());
    if (response->speed > 0) {
      return 1200000 / response->speed;
    }
    return 0;
  }
  AERROR << "lidar response: " << response_code;
  return -1;
}

bool XLidarService::SetLidarOperation(bool operation) {
  std::string url;
  if (operation) {
    url = "http://" + lidar_ip_ +
          "/motor-info?Gt0x12&command=0x0001&val=600&type=0";
  } else {
    url = "http://" + lidar_ip_ +
          "/setMechanical-info?Gt0x12&command=0x0002&val=316&type=0";
  }
  uint64_t response_code;
  std::string result;

  AINFO << "Set xlidar operation : " << operation;
  AINFO << "url : " << url;
  if (autox::common::util::HttpClient::Get(url, &response_code, &result, 1, 1,
                                           1)
          .ok()) {
    AINFO << "lidar response: " << response_code;
    return true;
  }
  AERROR << "lidar response: " << response_code;
  return false;
}

bool XLidarService::SetXlidarWiper(
    WiperMode mode, XLidarWiperCommand::WiperCycles wiper_cycles,
    XLidarWiperCommand::WiperFrequency frequency) {
  std::vector<std::string> wiper_commands;
  int wiper_frequency = 0;
  switch (frequency) {
    case XLidarWiperCommand::FREQUENCY_LOW:
      wiper_frequency = 10;
      break;
    case XLidarWiperCommand::FREQUENCY_MEDIUM:
      wiper_frequency = 30;
      break;
    case XLidarWiperCommand::FREQUENCY_HIGH:
      wiper_frequency = 60;
      break;
    default:
      AERROR << "Invalid wiper frequency";
  }
  int counted = 0;
  switch (wiper_cycles) {
    case XLidarWiperCommand::WIPING_ONE_CYCLE:
      counted = 1;
      break;
    case XLidarWiperCommand::WIPING_TWO_CYCLES:
      counted = 2;
      break;
    case XLidarWiperCommand::WIPING_THREE_CYCLES:
      counted = 3;
      break;
    case XLidarWiperCommand::WIPING_FOUR_CYCLES:
      counted = 4;
      break;
    case XLidarWiperCommand::WIPING_FIVE_CYCLES:
      counted = 5;
      break;
    case XLidarWiperCommand::WIPING_SIX_CYCLES:
      counted = 6;
      break;
    default:
      AERROR << "Invalid wiper counted level";
  }
  switch (mode) {
    case WiperMode::Stop:
      // stop wiper
      wiper_commands.push_back(
          "http://" + lidar_ip_ +
          "/wiperWriteOperation-info?Gt0x47&command=1000&val=2&sid=2E&len=7");
      AERROR << "STOP WIPER";
      break;
    case WiperMode::Continuous:
      if (wiper_frequency == 0) return false;

      // continuous wiper
      wiper_commands.push_back(
          "http://" + lidar_ip_ +
          "/wiperWriteOperation-info?Gt0x47&command=1000&val=1&sid=2E&len=7");
      wiper_commands.push_back(
          "http://" + lidar_ip_ +
          "/wiperWriteOperation-info?Gt0x47&command=1004&val=" +
          std::to_string(wiper_frequency) + "&sid=2E&len=7");
      wiper_commands.push_back(
          "http://" + lidar_ip_ +
          "/wiperWriteOperation-info?Gt0x47&command=1018&val=" +
          std::to_string(7) + "&sid=2E&len=7");
      AERROR << "CONTINUOUS WIPER, FREQUENCY: " << wiper_frequency;
      break;
    case WiperMode::Counted:
      if (counted == 0 || wiper_frequency == 0) return false;

      // counted wiper
      wiper_commands.push_back(
          "http://" + lidar_ip_ +
          "/wiperWriteOperation-info?Gt0x47&command=1000&val=1&sid=2E&len=7");
      wiper_commands.push_back(
          "http://" + lidar_ip_ +
          "/wiperWriteOperation-info?Gt0x47&command=1004&val=" +
          std::to_string(wiper_frequency) + "&sid=2E&len=7");
      wiper_commands.push_back(
          "http://" + lidar_ip_ +
          "/wiperWriteOperation-info?Gt0x47&command=1018&val=" +
          std::to_string(counted) + "&sid=2E&len=7");
      AERROR << "COUNTED WIPER, FREQUENCY: " << wiper_frequency
             << ", COUNT: " << counted;
      break;
    default:
      AERROR << "Invalid wiper mode";
      return false;
  }

  for (const auto& url : wiper_commands) {
    uint64_t response_code;
    std::string result;

    AINFO << "Set xlidar wiper command : " << url;
    if (autox::common::util::HttpClient::Get(url, &response_code, &result, 1, 1,
                                             1)
            .ok()) {
      AINFO << "lidar response: " << response_code;
    } else {
      AERROR << "lidar response: " << response_code;
      return false;
    }
  }
  return true;
}

}  // namespace autox::drivers::lidar_service
