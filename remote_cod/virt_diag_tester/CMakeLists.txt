######################################################################################################################
#   COPYRIGHT                                                                                                        #
#   -----------------------------------------------------------------------------------------------------------------#
#             This software is copyright protected and proprietary to Neusoft Reach.                                 #
#             Neusoft Reach grants to you only those rights as set out in the license conditions.                    #
#             All other rights remain with Neusoft Reach.                                                            #
#   -----------------------------------------------------------------------------------------------------------------#
# ####################################################################################################################
cmake_minimum_required(VERSION 3.2)
project(virtual_tester VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE on)


# if($ENV{AUTOSAR_AP_VERSION} MATCHES "NEUSAR_AP_R1911")
#     message(STATUS "  Using NEUSAR_AP_R1911")
#     set(NEUSAR_AP_R1911 ON)
# elseif($ENV{AUTOSAR_AP_VERSION} MATCHES "NEUSAR_AP_R2111")
#     message(STATUS "Using NEUSAR_AP_R2111")
#     set(NEUSAR_AP_R2111 ON)
# else()
#     message(FATAL_ERROR "Please set AUTOSAR_AP_VERSION evn to proper value")
# endif()

# Common Library--------------------------------------------------------------

# if(NEUSAR_AP_R1911)
#     find_package(ara-exec REQUIRED)
#     find_package(AdaptivePlatform REQUIRED)
#     find_package(ara-log REQUIRED)
#     find_package(Threads REQUIRED)
#     find_package(security_event_wrapper)
# endif()
# if(NEUSAR_AP_R2111)
#     find_package(AraExec REQUIRED)
#     find_package(AdaptivePlatform REQUIRED)
#     find_package(AraLog REQUIRED)
#     find_package(AraCore REQUIRED)
    find_package(Threads REQUIRED)
# endif()

# find_package(security_event_wrapper)
# if (security_event_wrapper_FOUND)
#     message(STATUS "  security_event_wrapper has found")
#     add_definitions(-DSECURITY_EVENT_WRAPPER)
# endif()


###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << Start of Common Library >>        DO NOT CHANGE THIS COMMENT!
###################################################################################################################################
	
###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of Common Library >>          DO NOT CHANGE THIS COMMENT!
###################################################################################################################################

# Application Related --------------------------------------------------
# if(NEUSAR_AP_R1911)
#     set(ARA_LIBRARIES ${ara_log_LIBS} ${ARAEXEC_LIBRARIES} ${ADAPTIVEPLATFORM_LIBRARIES})
# endif()
# if(NEUSAR_AP_R2111)
    # set(ARA_LIBRARIES ${AraCore_LIBS} ${ara_log_LIBS} ${ara_exec_LIBS} ${AdaptivePlatform_LIBS})
# endif()

set(ARA_LIBRARIES)
find_package(log_wrapper REQUIRED)
LIST(APPEND ARA_LIBRARIES ${log_wrapper_LIB})

if( QNX OR QNXNTO )
  LIST(APPEND ARA_LIBRARIES socket)
endif()


if( QNX OR QNXNTO )
  LIST(APPEND ARA_LIBRARIES socket)
endif()



####################################################################################
#  << start 3RD add-on area of linkLib >>     	 	 DO NOT CHANGE THIS COMMENT!
####################################################################################

####################################################################################
#  << end 3RD add-on area  of linkLib >>       	 	 DO NOT CHANGE THIS COMMENT!
####################################################################################

set(APP_NAME AsfVirtualDiagnosticTester)
set(LIBRARY_NAME asf_virtual_diagnostic)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set Env----------------------------------------------------------------------

# if(NEUSAR_AP_R1911)
#     set(APFOLODERNAME ap_1911/)
# endif()
# if(NEUSAR_AP_R2111)
#     set(APFOLODERNAME ap_2111/)
# endif()

# if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../${APP_NAME}/gen)
#     set(GEN_DEST ${CMAKE_CURRENT_SOURCE_DIR}/../${APP_NAME}/gen)
# elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${APFOLODERNAME}gen)
#     set(GEN_DEST ${CMAKE_CURRENT_SOURCE_DIR}/${APFOLODERNAME}gen)
# else()
#     message(FATAL_ERROR "gen direcotory not found.")
# endif()

# if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../${APP_NAME}/GenData)
#     set(GENDATA_DEST ${CMAKE_CURRENT_SOURCE_DIR}/../${APP_NAME}/GenData)
# elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${APFOLODERNAME}GenData)
#     set(GENDATA_DEST ${CMAKE_CURRENT_SOURCE_DIR}/${APFOLODERNAME}GenData)
# else()
#     message(FATAL_ERROR "GenDate direcotory not found.")
# endif()

# if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../__genData)
#     set(GENDATA_TYPE_DEST ${CMAKE_CURRENT_SOURCE_DIR}/../__genData)
# elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${APFOLODERNAME}__genData)
#     set(GENDATA_TYPE_DEST ${CMAKE_CURRENT_SOURCE_DIR}/${APFOLODERNAME}__genData)
# else()
#     message(FATAL_ERROR "GenDate type direcotory not found.")
# endif()

message(STATUS "GEN_DEST_PATH: ${GEN_DEST}")

file(GLOB_RECURSE userdata_srcs ${CMAKE_CURRENT_SOURCE_DIR}/core/src/*.cpp)
file(GLOB_RECURSE userdata_c_srcs ${CMAKE_CURRENT_SOURCE_DIR}/core/src/*.c)
file(GLOB_RECURSE userdata_hdrs ${CMAKE_CURRENT_SOURCE_DIR}/core/include/*.h)
file(GLOB_RECURSE userdata_hpp_hdrs ${CMAKE_CURRENT_SOURCE_DIR}/core/include/*.hpp)

include(GNUInstallDirs)
include_directories(
    ${GEN_DEST}
    ${GEN_DEST}/${APP_NAME}/includes
    ${GENDATA_DEST}
    ${GENDATA_TYPE_DEST}
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
)

# Application
set(ALL_COMPILE_SRC
    ${userdata_srcs}
    ${userdata_c_srcs}
    ${userdata_hdrs}
    ${userdata_hpp_hdrs}
    ${ALL_COMPILE_SRC}
)

# Build-------------------------------------------------------------------------

# if(NEUSAR_AP_R1911)
#     set(CMAKE_CXX_STANDARD 14)
# endif()
# if(NEUSAR_AP_R2111)
    set(CMAKE_CXX_STANDARD 17)
# endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -D_GLIBCXX_USE_NANOSLEEP -DLINUX")
set(OECORE_TARGET_ARCH "$ENV{OECORE_TARGET_ARCH}")

###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << Start of Build Parameters >>        DO NOT CHANGE THIS COMMENT!
###################################################################################################################################

LIST(APPEND ARA_LIBRARIES crypto_wrapper ssl)
LIST(APPEND ARA_LIBRARIES asf_DiagAuthenticationState)

# if(NEUSAR_AP_R1911)
#     add_definitions(-DNEUSAR_AP_R1911)
# endif()
# if(NEUSAR_AP_R2111)
#     add_definitions(-DNEUSAR_AP_R2111)
# endif()

if ($ENV{NEUSAR_SECURE_STARTUP} MATCHES "YES")
    message(">>>Project Is Secure Startup<<<")
    add_definitions(-DSECURE_STARTUP)
endif()

# add_definitions(-DNEUSAR_DEPLOYMENT_PATH_APP)

add_definitions(-DHTTP_WRAPPER)
#add_definitions(-DAP_CRYPTO)
add_definitions(-DMIDWARE_CRYPTO)
#add_definitions(-DLOCAL_DIAG_CLIENT)
#add_definitions(-DCONDITION_CHECK)
add_definitions(-DREQUEST_PUBLIC_KEY)
add_definitions(-DVEHICLE_CONFIG)
#add_definitions(-DOTA_INTERACTION)

###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of Build Parameters >>          DO NOT CHANGE THIS COMMENT!
###################################################################################################################################

add_library(${LIBRARY_NAME} STATIC ${ALL_COMPILE_SRC})
target_link_libraries(
    ${LIBRARY_NAME} 
    ${CMAKE_THREAD_LIBS_INIT} 
    ${ARA_LIBRARIES}
# if (security_event_wrapper_FOUND)
#     security_event_wrapper
# endif()
    mqttbridge
    )

if(${OECORE_TARGET_ARCH} STREQUAL "aarch64")
    set(CURRENT_PLATFORM "x64")
    message(STATUS "Current Platform is ${CURRENT_PLATFORM}")
    #target_link_libraries(VirtualTesterAPP ${CMAKE_CURRENT_SOURCE_DIR}/libs/SA/s32g/libSA.so)
    #target_link_libraries(VirtualTesterAPP ${CMAKE_CURRENT_SOURCE_DIR}/libs/httpwrapper/s32g/libhttp_wrapper.so)
else()
    set(CURRENT_PLATFORM "x86")
    message(STATUS "Current Platform is ${CURRENT_PLATFORM}")
    #target_link_libraries(VirtualTesterAPP ${CMAKE_CURRENT_SOURCE_DIR}/libs/SA/x86/libSA.so)
    #target_link_libraries(VirtualTesterAPP ${CMAKE_CURRENT_SOURCE_DIR}/libs/httpwrapper/x86/libhttp_wrapper.so)
endif()

install(TARGETS ${LIBRARY_NAME} ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/core/include/virtual_diag/virtual_diagnostics.h DESTINATION include/midware/diag/)
