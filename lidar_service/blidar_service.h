/******************************************************************************
 * Copyright 2025 AutoX. All Rights Reserved.
 *****************************************************************************/

#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include "onboard/drivers/blidar/proto/config.pb.h"
#include "onboard/drivers/lidar_service/proto/lidar_status.pb.h"
#include "onboard/drivers/utils/input.h"
#include "onboard/xrt/xrt.h"

namespace autox::drivers::lidar_service {

using BLidarStatusWriter =
    autox::xrt::Writer<autox::drivers::lidar_service::BLidarStatus>;
using BLidarStatusWriterPtr = std::shared_ptr<BLidarStatusWriter>;

struct BLidarServiceConfig {
  autox::drivers::blidar::Config lidar_config;
  BLidarStatusWriterPtr status_writer;
};

class BLidarService {
 public:
  explicit BLidarService(const BLidarServiceConfig& config);
  ~BLidarService();
  int GetLidarRotationSpeed();
  bool SetLidarOperation(bool operation);
  void Start();

 private:
  void LidarStatusRecvTask();
  void Stop();

 private:
  std::string lidar_ip_;
  std::atomic<bool> stop_{false};
  std::unique_ptr<Input> input_;
  std::thread lidar_status_recv_thr_;
  BLidarStatusWriterPtr status_writer_;
  bool enable_status_recv_{false};
};
}  // namespace autox::drivers::lidar_service
