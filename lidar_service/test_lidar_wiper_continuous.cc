/******************************************************************************
 * Copyright 2025 AutoX. All Rights Reserved.
 *****************************************************************************/

#include <chrono>
#include <thread>

#include "onboard/canbus/proto/lidar_motion.pb.h"
#include "onboard/common/adapters/adapter_gflags.h"
#include "onboard/common/util/http_client.h"
#include "onboard/drivers/lidar_service/proto/lidar_command.pb.h"
#include "onboard/drivers/xlidar/proto/config.pb.h"
#include "onboard/xrt/common/file.h"
#include "onboard/xrt/xrt.h"

using autox::canbus::LidarMotionCmd;
using autox::canbus::LidarMotionStatus;
using autox::drivers::lidar_service::XLidarWiperCommand;
using autox::drivers::xlidar::Config;

int main(int /*argc*/, char* argv[]) {
  autox::xrt::Init(argv[0]);

  std::shared_ptr<autox::xrt::Node> node(
      autox::xrt::CreateNode("example_client_node"));

  auto cmd_writer =
      node->CreateWriter<autox::drivers::lidar_service::XLidarWiperCommand>(
          FLAGS_xlidar_wiper_cmd_topic);
  AINFO << "client created";
  auto driver_msg =
      std::make_shared<autox::drivers::lidar_service::XLidarWiperCommand>();

  driver_msg->set_wiping_mode(
      autox::drivers::lidar_service::XLidarWiperCommand::
          WIPER_CONTINUOUS_WIPING_MODE);
  driver_msg->set_continuous_level(
      autox::drivers::lidar_service::XLidarWiperCommand::
          WIPING_ACTIVATE_CONTINUOUS);
  driver_msg->set_wiper_cycles(
      autox::drivers::lidar_service::XLidarWiperCommand::WIPING_THREE_CYCLES);
  driver_msg->set_frequence(
      autox::drivers::lidar_service::XLidarWiperCommand::FREQUENCY_MEDIUM);

  autox::xrt::RegisterSignalHandler();
  while (autox::xrt::OK()) {
    cmd_writer->Write(driver_msg);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  autox::xrt::WaitForShutdown();
  return 0;
}
