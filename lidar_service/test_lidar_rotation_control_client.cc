/******************************************************************************
 * Copyright 2025 AutoX. All Rights Reserved.
 *****************************************************************************/

#include <chrono>
#include <thread>

#include "onboard/canbus/proto/lidar_motion.pb.h"
#include "onboard/common/adapters/adapter_gflags.h"
#include "onboard/common/util/http_client.h"
#include "onboard/drivers/xlidar/proto/config.pb.h"
#include "onboard/xrt/common/file.h"
#include "onboard/xrt/xrt.h"

using autox::canbus::LidarMotionCmd;
using autox::canbus::LidarMotionStatus;
using autox::drivers::xlidar::Config;

int main(int /*argc*/, char* argv[]) {
  autox::xrt::Init(argv[0]);

  std::shared_ptr<autox::xrt::Node> node(
      autox::xrt::CreateNode("example_client_node"));

  auto cmd_writer =
      node->CreateWriter<LidarMotionCmd>(FLAGS_lidar_motion_cmd_topic);
  AINFO << "client created";
  auto driver_msg = std::make_shared<LidarMotionCmd>();

  driver_msg->set_xlidar(LidarMotionCmd::START);
  driver_msg->set_blidar_front(LidarMotionCmd::START);
  driver_msg->set_blidar_rear(LidarMotionCmd::START);

  autox::xrt::RegisterSignalHandler();
  while (autox::xrt::OK()) {
    cmd_writer->Write(driver_msg);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  autox::xrt::WaitForShutdown();
  return 0;
}
