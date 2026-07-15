/******************************************************************************
 * Copyright 2025 AutoX. All Rights Reserved.
 *****************************************************************************/

#include <memory>
#include <string>
#include <thread>

#include "onboard/drivers/lidar_service/proto/lidar_command.pb.h"
#include "onboard/drivers/lidar_service/proto/lidar_status.pb.h"
#include "onboard/drivers/utils/input.h"
#include "onboard/drivers/xlidar/proto/config.pb.h"
#include "onboard/xrt/xrt.h"

namespace autox::drivers::lidar_service {

using XLidarStatusWriter =
    autox::xrt::Writer<autox::drivers::lidar_service::XLidarStatus>;
using XLidarStatusWriterPtr = std::shared_ptr<XLidarStatusWriter>;

struct XLidarServiceConfig {
  autox::drivers::xlidar::Config lidar_config;
  XLidarStatusWriterPtr status_writer;
};

class XLidarService {
 public:
  enum class WiperMode { Stop, Continuous, Counted };
  explicit XLidarService(const XLidarServiceConfig& config);
  ~XLidarService();
  int GetLidarRotationSpeed();
  bool SetLidarOperation(bool operation);
  bool SetXlidarWiper(WiperMode mode,
                      XLidarWiperCommand::WiperCycles wiper_cycles,
                      XLidarWiperCommand::WiperFrequency frequency);

 private:
  std::string lidar_ip_;
  bool stop_{false};
  std::unique_ptr<Input> input_;
  std::thread lidar_status_recv_thr_;
  XLidarStatusWriterPtr status_writer_;
  bool enable_status_recv_{false};
};
}  // namespace autox::drivers::lidar_service
