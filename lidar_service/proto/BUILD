load("@com_google_protobuf//:protobuf.bzl", "py_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

package(default_visibility = ["//visibility:public"])

py_proto_library(
    name = "lidar_status_py_proto",
    srcs = ["lidar_status.proto"],
    deps = [
        "//onboard/common/proto:header_py_proto",
    ],
)

proto_library(
    name = "lidar_status_proto_lib",
    srcs = [
        "lidar_status.proto",
    ],
    deps = [
        "//onboard/common/proto:header_proto_lib",
    ],
)

cc_proto_library(
    name = "lidar_status_proto",
    deps = [
        ":lidar_status_proto_lib",
    ],
)

py_proto_library(
    name = "lidar_command_py_proto",
    srcs = ["lidar_command.proto"],
    deps = [
        "//onboard/common/proto:header_py_proto",
    ],
)

proto_library(
    name = "lidar_command_proto_lib",
    srcs = [
        "lidar_command.proto",
    ],
    deps = [
        "//onboard/common/proto:header_proto_lib",
    ],
)

cc_proto_library(
    name = "lidar_command_proto",
    deps = [
        ":lidar_command_proto_lib",
    ],
)
