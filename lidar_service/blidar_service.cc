/******************************************************************************
 * Copyright 2025 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "onboard/drivers/lidar_service/blidar_service.h"

#include "onboard/common/util/http_client.h"

struct LidarStatusMsg {
  uint8_t msg_header[4];
  uint32_t fpga_version;
  uint32_t ps_version;
  uint32_t system_monitor;
  uint32_t clock0;
  uint32_t clock1;
  uint32_t clock2;
  uint32_t clock3;
  uint32_t fpga_temperature;
  uint32_t board_temperature;
  uint32_t tx_temperature;
  uint32_t rx_temperature;
  uint32_t power12_v;
  uint32_t wl_status;
  uint32_t wl_distance;
  uint32_t wl_rx_packet;
  uint32_t wl_err_packet;
  uint32_t power_consumption;
  uint32_t motor_speed;

  uint32_t pl_version;
  uint32_t pl_temperature;
  uint32_t voltage12_v;
  uint32_t wl_com_status;
  uint32_t wl_distance_count;
  uint32_t wl_rx_count;
  uint32_t wl_err_packet_count;

  uint8_t rotor_fpga_flt;
  uint8_t rotor_board_flt;
  uint8_t rotor_tx_flt;
  uint8_t rotor_rx_flt;
  uint8_t stator_pl_flt;
  uint8_t temp_worst_level;

  uint8_t reserved[138];
  uint16_t roll_count;
  uint32_t crc32;
}; /* total len:256 bytes */

namespace autox::drivers::lidar_service {
using std::chrono_literals::operator""us;

BLidarService::BLidarService(const BLidarServiceConfig& config) {
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

void BLidarService::Start() {
  if (!enable_status_recv_) {
    AINFO << "BLidar status receive is disabled.";
    return;
  }
  Stop();
  stop_ = false;
  lidar_status_recv_thr_ =
      std::thread(&BLidarService::LidarStatusRecvTask, this);
  autox::xrt::scheduler::Instance()->SetInnerThreadAttr(
      "blidar_status_recv", &lidar_status_recv_thr_);
}

void BLidarService::Stop() {
  stop_ = true;

  if (lidar_status_recv_thr_.joinable()) {
    lidar_status_recv_thr_.join();
  }
}

BLidarService::~BLidarService() { Stop(); }

int BLidarService::GetLidarRotationSpeed() {
  struct MotorSpeed {
    uint8_t header[12];
    uint32_t speed;
  };

  const std::string url =
      "http://" + lidar_ip_ + "/getRReg-info?Gt0x08&addr=0x80040000";
  uint64_t response_code;
  std::string result;

  AINFO << "Get blidar rotation speed.";
  AINFO << "url : " << url;
  if (autox::common::util::HttpClient::Get(url, &response_code, &result, 1, 1,
                                           1)
          .ok()) {
    auto response = reinterpret_cast<const MotorSpeed*>(result.data());
    return response->speed;
  }
  AERROR << "lidar response: " << response_code;
  return -1;
}

bool BLidarService::SetLidarOperation(bool operation) {
  std::string http_cmd = "/Standby-info?Gt0x490";
  if (operation) {
    http_cmd = "/Operation-info?Gt0x491";
  }
  const std::string url = "http://" + lidar_ip_ + http_cmd;
  uint64_t response_code;
  std::string result;

  AINFO << "Set blidar operation : " << operation;
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

void BLidarService::LidarStatusRecvTask() {
  UdpPacket pkt;
  while (!stop_) {
    if (input_->getPacket(&pkt) == -1) {
      // AERROR << "wrong packet receives";
      std::this_thread::sleep_for(10us);
      continue;
    }
    auto udp_pkt = reinterpret_cast<const LidarStatusMsg*>(pkt.data);
    autox::drivers::lidar_service::BLidarStatus status_msg;
    status_msg.set_fpga_version(udp_pkt->fpga_version);
    status_msg.set_ps_version(udp_pkt->ps_version);
    status_msg.set_system_monitor(udp_pkt->system_monitor);
    status_msg.set_clock0(udp_pkt->clock0);
    status_msg.set_clock1(udp_pkt->clock1);
    status_msg.set_clock2(udp_pkt->clock2);
    status_msg.set_clock3(udp_pkt->clock3);
    status_msg.set_fpga_temperature(udp_pkt->fpga_temperature);
    status_msg.set_board_temperature(udp_pkt->board_temperature);
    status_msg.set_tx_temperature(udp_pkt->tx_temperature);
    status_msg.set_rx_temperature(udp_pkt->rx_temperature);
    status_msg.set_power_12v(udp_pkt->power12_v);
    status_msg.set_wl_status(udp_pkt->wl_status);
    status_msg.set_wl_distance(udp_pkt->wl_distance);
    status_msg.set_wl_rx_packet(udp_pkt->wl_rx_packet);
    status_msg.set_wl_err_packet(udp_pkt->wl_err_packet);
    status_msg.set_power_consumption(udp_pkt->power_consumption);
    status_msg.set_motor_speed(udp_pkt->motor_speed);
    status_msg.set_pl_version(udp_pkt->pl_version);
    status_msg.set_pl_temperature(udp_pkt->pl_temperature);
    status_msg.set_voltage_12v(udp_pkt->voltage12_v);
    status_msg.set_wl_com_status(udp_pkt->wl_com_status);
    status_msg.set_wl_distance_count(udp_pkt->wl_distance_count);
    status_msg.set_wl_rx_count(udp_pkt->wl_rx_count);
    status_msg.set_wl_err_packet_count(udp_pkt->wl_err_packet_count);
    status_msg.set_rotor_fpga_flt(udp_pkt->rotor_fpga_flt);
    status_msg.set_rotor_board_flt(udp_pkt->rotor_board_flt);
    status_msg.set_rotor_tx_flt(udp_pkt->rotor_tx_flt);
    status_msg.set_rotor_rx_flt(udp_pkt->rotor_rx_flt);
    status_msg.set_stator_pl_flt(udp_pkt->stator_pl_flt);
    status_msg.set_temp_worst_level(udp_pkt->temp_worst_level);
    status_msg.set_roll_count(udp_pkt->roll_count);
    status_writer_->Write(status_msg);
  }
}

}  // namespace autox::drivers::lidar_service
