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

cmake_minimum_required(VERSION 3.4)
project(DiagnosticProxy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE on)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Choose the type of build, one of: Debug, Release, RelWithDebInfo, MinSizeRel"
        FORCE
    )
endif()

# if($ENV{AUTOSAR_AP_VERSION} MATCHES "NEUSAR_AP_R1911")
#     message(STATUS "  Using NEUSAR_AP_R1911")
#     set(NEUSAR_AP_R1911 ON)
# elseif($ENV{AUTOSAR_AP_VERSION} MATCHES "NEUSAR_AP_R2111")
#     message(STATUS "Using NEUSAR_AP_R2111")
#     set(NEUSAR_AP_R2111 ON)
# else()
#     message(FATAL_ERROR "Please set AUTOSAR_AP_VERSION evn to proper value")
# endif()

# if(NEUSAR_AP_R1911)
#     find_package(ara-exec REQUIRED)
#     find_package(AdaptivePlatform REQUIRED)
#     find_package(ara-log REQUIRED)
# endif()
# if(NEUSAR_AP_R2111)
#     find_package(AraExec REQUIRED)
#     find_package(AdaptivePlatform REQUIRED)
#     find_package(AraLog REQUIRED)
#     find_package(AraCore REQUIRED)
# endif()

# set(ARA_LIBRARIES ${ara_log_LIBS})
set(ARA_LIBRARIES)

# if(NEUSAR_AP_R1911)
#     add_definitions(-DNEUSAR_AP_R1911)
# endif()
# if(NEUSAR_AP_R2111)
    # add_definitions(-DNEUSAR_AP_R2111)
    add_definitions(-DUSE_FS_LIBNET)
# endif()

add_definitions(-DWITHOUT_PHM)

# add_definitions(-DUSE_ARA)

# find_package(phm_adapter REQUIRED)

find_package(neusar_net REQUIRED)
LIST(APPEND ARA_LIBRARIES ${neusar_net_LIBS})

find_package(log_wrapper REQUIRED)
LIST(APPEND ARA_LIBRARIES log_wrapper)
add_definitions(-DUSE_MIDWARE_LOG)

# dependencies
set(CMAKE_THREAD_PREFER_PTHREAD ON)
if("${CMAKE_VERSION}" VERSION_EQUAL "3.10.0" OR "${CMAKE_VERSION}" VERSION_GREATER "3.10.0")
    # This leads to build failure when cross-compiling in CMake below 3.10.0
    set(THREADS_PREFER_PTHREAD_FLAG ON)
endif()
find_package(Threads REQUIRED)
if("${CMAKE_VERSION}" VERSION_LESS "3.10.0" AND CMAKE_USE_PTHREADS_INIT)
    # Fallback to modifying CXX_FLAGS in older versions
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

if( QNX OR QNXNTO )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lsocket")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -Wall -Wextra -Wpedantic -Wsign-promo -Wnon-virtual-dtor -Wctor-dtor-privacy -Woverloaded-virtual -Wold-style-cast -Wpointer-arith -Wshadow -Wunused -Wuninitialized -Winit-self -Wdeprecated -Wfloat-equal -Werror")

# Offer the user the choice of overriding the installation directories
set(INSTALL_CMAKE_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/DiagnosticProxy" CACHE PATH "Installation directory for CMake files")

# setting some variables for ProxyCommon to be used in this project
set(PROXYCOMMON_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include/public")
set(PROXY_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include/private")
set(PROXYCOMMON_LIBRARIES ProxyCommon)

# Add actual DP core code
add_subdirectory(src)

export(TARGETS ProxyCommon FILE "${PROJECT_BINARY_DIR}/ProxyCommonTargets.cmake")

export(PACKAGE ProxyCommon)
install(EXPORT ProxyCommonTargets DESTINATION ${INSTALL_CMAKE_DIR}/lib_proxy_common)

# Tell cmake that we exported this package under the name "DiagnosticProxy"
# -> find_package(DiagnosticProxy)
export(PACKAGE DiagnosticProxy)

include_directories(${GTEST_INCLUDE_DIRS})

# Now create the configuration data that cmake wants to find when looking in this directory for "DCM"
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${CMAKE_INSTALL_FULL_INCLUDEDIR}")

