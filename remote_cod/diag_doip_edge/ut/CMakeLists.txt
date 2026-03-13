cmake_minimum_required(VERSION 3.2)
set(CMAKE_VERBOSE_MAKEFILE on)
include(GNUInstallDirs)

find_package(ara-exec REQUIRED)
find_package(AdaptivePlatform REQUIRED)
find_package(ara-log REQUIRED)
find_package(Threads REQUIRED)
find_package(neusar_net REQUIRED)

find_package(GTest REQUIRED)
find_library(GMOCK gmock)

set(ARA_LIBRARIES ara::log)

set(LIBS ${GTEST_BOTH_LIBRARIES} ${GMOCK} Threads::Threads ProxyCommon
    ${ARA_LIBRARIES} ${CMAKE_DL_LIBS} ${ARAEXEC_LIBRARIES} ${ADAPTIVEPLATFORM_LIBRARIES}
    ${neusar_net_LIBS} log_wrapper gtest gcov
)

set(TEST_TARGET unit_diag_proxy)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/private
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/public
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/diagnostic_proxy
)

file(GLOB_RECURSE common_srcs ${CMAKE_CURRENT_SOURCE_DIR}/../src/common/*.cpp)
file(GLOB_RECURSE diag_common_srcs ${CMAKE_CURRENT_SOURCE_DIR}/../src/diagnostic_proxy/common/*.cpp)
file(GLOB_RECURSE diag_dcm_srcs ${CMAKE_CURRENT_SOURCE_DIR}/../src/diagnostic_proxy/dcm/*.cpp)
file(GLOB_RECURSE diag_uds_transport_srcs ${CMAKE_CURRENT_SOURCE_DIR}/../src/diagnostic_proxy/uds_transport/*.cpp)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src DEV_SRC_LIST)

set(ALL_COMPILE_SRC
    ${common_srcs}
    ${diag_common_srcs}
    ${diag_dcm_srcs}
    ${diag_uds_transport_srcs}
    ${DEV_SRC_LIST}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -fno-access-control")

add_executable(${TEST_TARGET} ${ALL_COMPILE_SRC})

target_link_libraries(${TEST_TARGET} PRIVATE ${LIBS})
