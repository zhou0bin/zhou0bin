######################################################################################################################
#   COPYRIGHT                                                                                                        #
#   -----------------------------------------------------------------------------------------------------------------#
#             This software is copyright protected and proprietary to Neusoft Reach.                                 #
#             Neusoft Reach grants to you only those rights as set out in the license conditions.                    #
#             All other rights remain with Neusoft Reach.                                                            #
#   -----------------------------------------------------------------------------------------------------------------#
# ####################################################################################################################
# Cmake least version
cmake_minimum_required(VERSION 3.4.3)
project(UDSAPI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE on)
include(GNUInstallDirs)

# if($ENV{AUTOSAR_AP_VERSION} MATCHES "NEUSAR_AP_R1911")
#     message(STATUS "  Using NEUSAR_AP_R1911")
#     set(NEUSAR_AP_R1911 ON)
# elseif($ENV{AUTOSAR_AP_VERSION} MATCHES "NEUSAR_AP_R2111")
#     message(STATUS "Using NEUSAR_AP_R2111")
    # set(NEUSAR_AP_R2111 ON)
# else()
#     message(FATAL_ERROR "Please set AUTOSAR_AP_VERSION evn to proper value")
# endif()

# Common Library--------------------------------------------------------------
find_package(Threads REQUIRED)
# if(NEUSAR_AP_R1911)
#     find_package(ara-log REQUIRED)
# endif()
# if(NEUSAR_AP_R2111)
#     find_package(AraLog REQUIRED)
# endif()

###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << Start of Common Library >>        DO NOT CHANGE THIS COMMENT!
###################################################################################################################################
	
###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of Common Library >>          DO NOT CHANGE THIS COMMENT!
###################################################################################################################################

# Application Related --------------------------------------------------




# set(ARA_LIBRARIES ${ara_log_LIBS})
set(ARA_LIBRARIES)


if( QNX OR QNXNTO )
  LIST(APPEND ARA_LIBRARIES socket)
endif()





####################################################################################
#  << start 3RD add-on area of linkLib >>     	 	 DO NOT CHANGE THIS COMMENT!
####################################################################################

####################################################################################
#  << end 3RD add-on area  of linkLib >>       	 	 DO NOT CHANGE THIS COMMENT!
####################################################################################


set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set Env----------------------------------------------------------------------
include_directories(includes)
include_directories(includes/common)
include_directories(includes/communication)
include_directories(includes/interface)
include_directories(includes/processor)
include_directories(includes/public)

set(${MAIN_EXECUTABLE}_SRC)
#list(APPEND ${MAIN_EXECUTABLE}_SRC main.cpp)

# lib
set(LIB_UDS_INTERFACE_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interface/uds_c_bridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interface/uds_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interface/uds_instance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interface/uds_test_present.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/processor/uds_processor_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/processor/uds_processor_parallel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/processor/uds_processor_serial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/processor/uds_processor_base.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/processor/uds_processor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/communication/diag_libnet_uds_client.cpp
)

set(LIB_UDS_API_CTYPE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interface/uds_c_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/processor/uds_c_processor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/communication/diag_libnet_uds_client.cpp
)

set(LIB_UDS_API_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interface/async_uds_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interface/async_uds_response_processor_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interface/async_uds_response_processor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interface/sync_uds_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/processor/uds_processor_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/processor/uds_processor_parallel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/processor/uds_processor_serial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/processor/uds_processor_base.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/communication/diag_libnet_uds_client.cpp
)

set(LIB_UDS_API_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/public/midware/diag/uds_app_com.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/interface/async_uds_interface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/interface/sync_uds_interface.h
)

# Build-------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -D_GLIBCXX_USE_NANOSLEEP -DLINUX")

###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << Start of Build Parameters >>        DO NOT CHANGE THIS COMMENT!
###################################################################################################################################

find_package(neusar_net REQUIRED)
LIST(APPEND ARA_LIBRARIES ${neusar_net_LIBS})

find_package(log_wrapper REQUIRED)
LIST(APPEND ARA_LIBRARIES log_wrapper)
add_definitions(-DUSE_MIDWARE_LOG)

# if(NEUSAR_AP_R1911)
#     add_definitions(-DNEUSAR_AP_R1911)
# endif()
# if(NEUSAR_AP_R2111)
#     add_definitions(-DNEUSAR_AP_R2111)
    add_definitions(-DUSE_FS_LIBNET)
# endif()

###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of Build Parameters >>          DO NOT CHANGE THIS COMMENT!
###################################################################################################################################

add_library(asf_Uds SHARED ${LIB_UDS_INTERFACE_SRCS})
target_link_libraries(asf_Uds ${ARA_LIBRARIES})
install(TARGETS asf_Uds DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

add_library(asf_UdsInterfaceCtype SHARED ${LIB_UDS_API_CTYPE_SRC})
target_link_libraries(asf_UdsInterfaceCtype ${ARA_LIBRARIES})
install(TARGETS asf_UdsInterfaceCtype DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

add_library(asf_UdsInterface SHARED ${LIB_UDS_API_SRCS})
target_link_libraries(asf_UdsInterface ${ARA_LIBRARIES})
install(TARGETS asf_UdsInterface DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
install(FILES ${LIB_UDS_API_HDRS} DESTINATION include/midware/diag/
)
