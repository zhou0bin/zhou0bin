#  --------------------------------------------------------------------------
#  |                _     _              _____         _____                |
#  |               |  \  | |            / ____|  /\   |  __ \               |
#  |               | | \ | |  __       | (___   /  \  | |__) |              |
#  |               | |\ \| | /__\|   |  \___ \ / /\ \ |  _  /               |
#  |               | | \ \ ||    |   |   ___) / /__\ \| | \ \               |
#  |               |_|  \_\| \__/ \_/|/|_____/________\_|  \_\              |
#  |                                                                        |
#  --------------------------------------------------------------------------
#  COPYRIGHT
#  --------------------------------------------------------------------------
#
#  This software is copyright protected and proprietary to Neusoft Reach.
#  Neusoft Reach grants to you only those rights as set out in the license 
#  conditions.
#  All other rights remain with Neusoft Reach.
#  --------------------------------------------------------------------------

set(PROXYCOMMON_SRC
    communication/diag_libnet_client.cpp
    communication/diag_libnet_server.cpp
)

add_library(ProxyCommon SHARED ${PROXYCOMMON_SRC})

target_link_libraries (ProxyCommon ${ARA_LIBRARIES})

set(PROXYCOMMON_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR})

include_directories(
  ${PROXYCOMMON_INCLUDE_DIRS}
  ${PROXY_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/diagnostic_proxy/
)

install (
    TARGETS ProxyCommon
    EXPORT ProxyCommonTargets
    DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}"
)

file (RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${CMAKE_INSTALL_FULL_INCLUDEDIR}")

