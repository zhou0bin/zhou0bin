package(default_visibility = ["//visibility:public"])

cc_library(
    name = "xlidar_service_lib",
    srcs =
        [
            "xlidar_service.cc",
        ],
    hdrs =
        [
            "xlidar_service.h",
        ],
    deps = [
        "//onboard/common/util:http_client",
        "//onboard/drivers/lidar_service/proto:lidar_command_proto",
        "//onboard/drivers/lidar_service/proto:lidar_status_proto",
        "//onboard/drivers/utils:utils_lib",
        "//onboard/drivers/xlidar/proto:xlidar_proto",
        "//onboard/xrt",
    ],
)

cc_library(
    name = "blidar_service_lib",
    srcs =
        [
            "blidar_service.cc",
        ],
    hdrs =
        [
            "blidar_service.h",
        ],
    deps = [
        "//onboard/common/util:http_client",
        "//onboard/drivers/blidar/proto:blidar_proto",
        "//onboard/drivers/lidar_service/proto:lidar_status_proto",
        "//onboard/drivers/utils:utils_lib",
        "//onboard/xrt",
    ],
)

cc_binary(
    name = "lidar_control_service",
    srcs =
        [
            "lidar_rotation_control_service.cc",
        ],
    deps = [
        ":blidar_service_lib",
        ":xlidar_service_lib",
        "//onboard/canbus/proto:lidar_motion_proto",
        "//onboard/common/adapters:adapter_gflags",
        "//onboard/common/util:http_client",
        "//onboard/common/util:message_util",
        "//onboard/drivers/blidar/proto:blidar_proto",
        "//onboard/drivers/lidar_service/proto:lidar_command_proto",
        "//onboard/drivers/lidar_service/proto:lidar_status_proto",
        "//onboard/drivers/xlidar/proto:xlidar_proto",
        "//onboard/xrt",
    ],
)

cc_binary(
    name = "test_lidar_control_client",
    srcs =
        [
            "test_lidar_rotation_control_client.cc",
        ],
    deps = [
        "//onboard/canbus/proto:lidar_motion_proto",
        "//onboard/common/adapters:adapter_gflags",
        "//onboard/common/util:http_client",
        "//onboard/common/util:message_util",
        "//onboard/drivers/blidar/proto:blidar_proto",
        "//onboard/drivers/xlidar/proto:xlidar_proto",
        "//onboard/xrt",
    ],
)

cc_binary(
    name = "test_lidar_wiper_continuous",
    srcs =
        [
            "test_lidar_wiper_continuous.cc",
        ],
    deps = [
        "//onboard/canbus/proto:lidar_motion_proto",
        "//onboard/common/adapters:adapter_gflags",
        "//onboard/common/util:http_client",
        "//onboard/common/util:message_util",
        "//onboard/drivers/blidar/proto:blidar_proto",
        "//onboard/drivers/lidar_service/proto:lidar_command_proto",
        "//onboard/drivers/xlidar/proto:xlidar_proto",
        "//onboard/xrt",
    ],
)

cc_binary(
    name = "test_lidar_wiper_cycle",
    srcs =
        [
            "test_lidar_wiper_cycle.cc",
        ],
    deps = [
        "//onboard/canbus/proto:lidar_motion_proto",
        "//onboard/common/adapters:adapter_gflags",
        "//onboard/common/util:http_client",
        "//onboard/common/util:message_util",
        "//onboard/drivers/blidar/proto:blidar_proto",
        "//onboard/drivers/lidar_service/proto:lidar_command_proto",
        "//onboard/drivers/xlidar/proto:xlidar_proto",
        "//onboard/xrt",
    ],
)

cc_binary(
    name = "test_lidar_wiper_stop",
    srcs =
        [
            "test_lidar_wiper_stop.cc",
        ],
    deps = [
        "//onboard/canbus/proto:lidar_motion_proto",
        "//onboard/common/adapters:adapter_gflags",
        "//onboard/common/util:http_client",
        "//onboard/common/util:message_util",
        "//onboard/drivers/blidar/proto:blidar_proto",
        "//onboard/drivers/lidar_service/proto:lidar_command_proto",
        "//onboard/drivers/xlidar/proto:xlidar_proto",
        "//onboard/xrt",
    ],
)
