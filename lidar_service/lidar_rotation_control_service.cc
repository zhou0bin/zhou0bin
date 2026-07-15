/******************************************************************************
 * Copyright 2025 AutoX. All Rights Reserved.
 *****************************************************************************/

#include <chrono>
#include <thread>

#include "onboard/canbus/proto/lidar_motion.pb.h"
#include "onboard/common/adapters/adapter_gflags.h"
#include "onboard/common/util/http_client.h"
#include "onboard/common/util/message_util.h"
#include "onboard/drivers/blidar/proto/config.pb.h"
#include "onboard/drivers/lidar_service/blidar_service.h"
#include "onboard/drivers/lidar_service/proto/lidar_command.pb.h"
#include "onboard/drivers/lidar_service/xlidar_service.h"
#include "onboard/drivers/xlidar/proto/config.pb.h"
#include "onboard/xrt/common/file.h"
#include "onboard/xrt/common/time_conversion.h"
#include "onboard/xrt/xrt.h"

using autox::canbus::LidarMotionCmd;
using autox::canbus::LidarMotionStatus;
using autox::drivers::lidar_service::BLidarService;
using autox::drivers::lidar_service::BLidarServiceConfig;
using autox::drivers::lidar_service::XLidarService;
using autox::drivers::lidar_service::XLidarServiceConfig;

LidarMotionStatus::LidarMotionStatusType GetLidarMotionStatusFromRotationSpeed(
    int rotation_speed, int max_rotation_speed, int last_rotation_speed) {
  // Error case
  if (rotation_speed < 0) {
    return LidarMotionStatus::UNKNOWN;
  }

  // Stopped case
  if (rotation_speed == 0) {
    return LidarMotionStatus::STOPPED;
  }

  // Full speed case
  if (rotation_speed >= max_rotation_speed) {
    return LidarMotionStatus::STARTED;
  }

  // Transitional states
  int delta_rotation_speed = rotation_speed - last_rotation_speed;
  return (delta_rotation_speed >= 0) ? LidarMotionStatus::STARTING
                                     : LidarMotionStatus::STOPPING;
}

int main(int /*argc*/, char* argv[]) {
  autox::xrt::Init(argv[0]);
  std::shared_ptr<autox::xrt::Node> node(
      autox::xrt::CreateNode("xlidar rotation control"));

  struct LidarServices {
    XLidarService xlidar;
    BLidarService blidar_front;
    BLidarService blidar_rear;
    BLidarService blidar_left;
    BLidarService blidar_right;
    LidarServices(XLidarServiceConfig xlidar_config,
                  BLidarServiceConfig blidar_front_config,
                  BLidarServiceConfig blidar_rear_config,
                  BLidarServiceConfig blidar_left_config,
                  BLidarServiceConfig blidar_right_config)
        : xlidar(xlidar_config),
          blidar_front(blidar_front_config),
          blidar_rear(blidar_rear_config),
          blidar_left(blidar_left_config),
          blidar_right(blidar_right_config) {}
  };

  auto loadConfig = [](const std::string& path,
                       google::protobuf::Message* config) {
    if (!autox::xrt::common::GetProtoFromFile(path, config)) {
      AERROR << "Failed to load config from: " << path;
      return false;
    }
    return true;
  };

  auto initializeLidarServices = [loadConfig,
                                  &node]() -> std::optional<LidarServices> {
    autox::drivers::xlidar::Config xlidar_config;
    autox::drivers::blidar::Config blidar_front_config, blidar_rear_config,
        blidar_left_config, blidar_right_config;

    const std::vector<std::pair<std::string, google::protobuf::Message*>>
        configs = {
            {"current_configs/drivers/xlidar/conf/xlidar_conf.pb.txt",
             &xlidar_config},
            {"current_configs/drivers/blidar/conf/blidar_front_conf.pb.txt",
             &blidar_front_config},
            {"current_configs/drivers/blidar/conf/blidar_rear_conf.pb.txt",
             &blidar_rear_config},
            {"current_configs/drivers/blidar/conf/blidar_left_conf.pb.txt",
             &blidar_left_config},
            {"current_configs/drivers/blidar/conf/blidar_right_conf.pb.txt",
             &blidar_right_config}};

    for (const auto& [path, config] : configs) {
      if (!loadConfig(path, config)) {
        return std::nullopt;
      }
    }

    XLidarServiceConfig xlidar_service_config;
    xlidar_service_config.lidar_config = xlidar_config;
    xlidar_service_config.status_writer =
        node->CreateWriter<autox::drivers::lidar_service::XLidarStatus>(
            FLAGS_xlidar_status_topic);

    // Create status writers for each BLidar
    auto create_status_writer = [&node](const std::string& topic) {
      return node->CreateWriter<autox::drivers::lidar_service::BLidarStatus>(
          topic);
    };

    BLidarServiceConfig front_config{
        .lidar_config = blidar_front_config,
        .status_writer = create_status_writer(FLAGS_blidar_front_status_topic)};
    BLidarServiceConfig rear_config{
        .lidar_config = blidar_rear_config,
        .status_writer = create_status_writer(FLAGS_blidar_rear_status_topic)};
    BLidarServiceConfig left_config{
        .lidar_config = blidar_left_config,
        .status_writer = create_status_writer(FLAGS_blidar_left_status_topic)};
    BLidarServiceConfig right_config{
        .lidar_config = blidar_right_config,
        .status_writer = create_status_writer(FLAGS_blidar_right_status_topic)};
    return std::make_optional<LidarServices>(xlidar_service_config,
                                             front_config, rear_config,
                                             left_config, right_config);
  };

  auto services = initializeLidarServices();
  if (!services) {
    AERROR << "Failed to initialize lidar services";
    return -1;
  }

  // Start all BLidar services
  services->blidar_front.Start();
  services->blidar_rear.Start();
  services->blidar_left.Start();
  services->blidar_right.Start();

  bool lidar_cmd[5] = {0};  // xlidar front rear left right
  // create lidar motion command reader
  auto lidar_cmd_reader = node->CreateReader<LidarMotionCmd>(
      FLAGS_lidar_motion_cmd_topic,
      [&services, &lidar_cmd](const std::shared_ptr<LidarMotionCmd>& request) {
        if (!request) {
          AERROR << "Received null LidarMotionCmd request";
          return;
        }

        AINFO << request->header().timestamp_sec()
              << ": Received lidar motion command";

        auto processLidarCommand = [](auto& service, int index, auto& lidar_cmd,
                                      LidarMotionCmd::LidarMotionCmdType cmd) {
          if (cmd != LidarMotionCmd::NOREQUEST) {
            bool operation = (cmd == LidarMotionCmd::START);
            lidar_cmd[index] = operation;
            try {
              service.SetLidarOperation(operation);
              AINFO << "Successfully set lidar " << index << " operation to "
                    << (operation ? "START" : "STOP");
            } catch (const std::exception& e) {
              AERROR << "Failed to set lidar " << index
                     << " operation: " << e.what();
            }
          }
        };

        processLidarCommand(services->xlidar, 0, lidar_cmd, request->xlidar());
        processLidarCommand(services->blidar_front, 1, lidar_cmd,
                            request->blidar_front());
        processLidarCommand(services->blidar_rear, 2, lidar_cmd,
                            request->blidar_rear());
        processLidarCommand(services->blidar_left, 3, lidar_cmd,
                            request->blidar_left());
        processLidarCommand(services->blidar_right, 4, lidar_cmd,
                            request->blidar_right());
      });
  AINFO << "reader created";
  // create xlidar wiper command reader
  auto xlidar_wiper_cmd_reader =
      node->CreateReader<autox::drivers::lidar_service::XLidarWiperCommand>(
          FLAGS_xlidar_wiper_cmd_topic,
          [&services](
              const std::shared_ptr<
                  autox::drivers::lidar_service::XLidarWiperCommand>& request) {
            if (!request) {
              AERROR << "Received null XLidarWiperCommand request";
              return;
            }

            AINFO << request->header().timestamp_sec()
                  << ": Received xlidar wiper command";

            try {
              using WiperCmd =
                  autox::drivers::lidar_service::XLidarWiperCommand;

              // Helper lambda to convert proto values to WiperMode
              auto getWiperMode =
                  [request]() -> std::optional<XLidarService::WiperMode> {
                switch (request->wiping_mode()) {
                  case WiperCmd::WIPER_CYCLES_WIPING_MODE:
                    return XLidarService::WiperMode::Counted;

                  case WiperCmd::WIPER_CONTINUOUS_WIPING_MODE:
                    switch (request->continuous_level()) {
                      case WiperCmd::WIPING_ACTIVATE_CONTINUOUS:
                        return XLidarService::WiperMode::Continuous;
                      case WiperCmd::WIPING_DEACTIVATE_CONTINUOUS:
                        return XLidarService::WiperMode::Stop;
                      default:
                        AERROR << "Invalid continuous level: "
                               << request->continuous_level();
                        return std::nullopt;
                    }

                  default:
                    AERROR << "Invalid wiping mode: " << request->wiping_mode();
                    return std::nullopt;
                }
              };

              auto mode = getWiperMode();
              if (!mode.has_value()) {
                return;
              }

              if (!services->xlidar.SetXlidarWiper(mode.value(),
                                                   request->wiper_cycles(),
                                                   request->frequence())) {
                AWARN << "Failed to set xlidar wiper";
              }
            } catch (const std::exception& e) {
              AERROR << "Failed to set xlidar wiper: " << e.what();
            }
          });

  // xrt writers
  std::shared_ptr<autox::xrt::Writer<LidarMotionStatus>>
      lidar_motion_status_topic_writer = node->CreateWriter<LidarMotionStatus>(
          FLAGS_lidar_motion_status_topic);

  LidarMotionStatus lidar_motion_status;

  uint64_t msg_seq_num = 0;
  int last_rotation_speeds[5] = {0};

  const int max_rotation_speed = 540;  // 10% erros, actual is 600
  while (autox::xrt::OK()) {
    auto updateLidarStatus = [&](auto& service, auto setter, int& last_speed) {
      int rotation_speed = service.GetLidarRotationSpeed();
      auto status = GetLidarMotionStatusFromRotationSpeed(
          rotation_speed, max_rotation_speed, last_speed);
      (lidar_motion_status.*setter)(status);
      last_speed = rotation_speed;
    };

    // Update all lidar statuses
    updateLidarStatus(services->xlidar, &LidarMotionStatus::set_xlidar,
                      last_rotation_speeds[0]);
    updateLidarStatus(services->blidar_front,
                      &LidarMotionStatus::set_blidar_front,
                      last_rotation_speeds[1]);
    updateLidarStatus(services->blidar_rear,
                      &LidarMotionStatus::set_blidar_rear,
                      last_rotation_speeds[2]);
    updateLidarStatus(services->blidar_left,
                      &LidarMotionStatus::set_blidar_left,
                      last_rotation_speeds[3]);
    updateLidarStatus(services->blidar_right,
                      &LidarMotionStatus::set_blidar_right,
                      last_rotation_speeds[4]);

    autox::common::util::FillHeader("lidar_motion_status_writer",
                                    &lidar_motion_status, msg_seq_num++);

    lidar_motion_status_topic_writer->Write(
        std::make_shared<LidarMotionStatus>(lidar_motion_status));

    // max 10 fps
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  autox::xrt::WaitForShutdown();
  return 0;
}
