######################################################################################################################
#   COPYRIGHT                                                                                                        #
#   -----------------------------------------------------------------------------------------------------------------#
#             This software is copyright protected and proprietary to Neusoft Reach.                                 #
#             Neusoft Reach grants to you only those rights as set out in the license conditions.                    #
#             All other rights remain with Neusoft Reach.                                                            #
#   -----------------------------------------------------------------------------------------------------------------#
# ####################################################################################################################
cmake_minimum_required(VERSION 3.2)
set(CMAKE_VERBOSE_MAKEFILE on)
include(GNUInstallDirs)

# Common Library--------------------------------------------------------------
# find_package(AraExec REQUIRED)
# find_package(AdaptivePlatform REQUIRED)
# find_package(AraLog REQUIRED)
find_package(Threads REQUIRED)

###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << Start of Common Library >>        DO NOT CHANGE THIS COMMENT!
###################################################################################################################################

###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of Common Library >>          DO NOT CHANGE THIS COMMENT!
###################################################################################################################################

# Application Related --------------------------------------------------
# add_definitions(-DHAS_VSOMEIP_BINDING)
# add_definitions(-DWITH_ARA_IAM)
# find_package(AraCom REQUIRED) 
# find_package(AraLog REQUIRED)
# find_package(AraCore REQUIRED)
# find_package(AraPer REQUIRED)
# find_package(AraPhm)



# find_package(AraIamApi REQUIRED)
# find_package(AraCryptoApi REQUIRED)

# set(ARA_LIBRARIES ${AraCore_LIBS} ${AraCom_LIBS} ${ara_log_LIBS} ${ara_phm_LIBS} ${iam_api_LIBS} ${ara_crypto_api_LIBS})
set(ARA_LIBRARIES)


if( QNX OR QNXNTO )
  LIST(APPEND ARA_LIBRARIES socket)
endif()


if( QNX OR QNXNTO )
  LIST(APPEND ARA_LIBRARIES socket)
endif()


####################################################################################
#  << start 3RD add-on area of linkLib >>            DO NOT CHANGE THIS COMMENT!
####################################################################################

####################################################################################
#  << end 3RD add-on area  of linkLib >>             DO NOT CHANGE THIS COMMENT!
####################################################################################

set(APP_NAME AsfVirtualDiagnosticTester)
set(MAIN_EXECUTABLE AsfVirtualDiagnosticTester)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set Env----------------------------------------------------------------------
file(GLOB_RECURSE userdata_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE userdata_c_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE userdata_hdrs ${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h)
file(GLOB_RECURSE userdata_hpp_hdrs ${CMAKE_CURRENT_SOURCE_DIR}/includes/*.hpp)
include_directories(includes)

set(GEN_DEST ${CMAKE_CURRENT_SOURCE_DIR}/gen/)
file(GLOB_RECURSE aragen_srcs ${GEN_DEST}/*.cpp)
file(GLOB_RECURSE aragen_hdrs ${GEN_DEST}/*.h)
set(GENDATA_DEST ${CMAKE_CURRENT_SOURCE_DIR}/GenData)
file(GLOB_RECURSE aragendata_srcs ${GENDATA_DEST}/*.cpp)
file(GLOB_RECURSE aragendata_hdrs ${GENDATA_DEST}/*.h)
include_directories(${GEN_DEST}/${APP_NAME}/includes)
include_directories(GenData)
include_directories(../__genData)

message(STATUS "GEN_DEST_PATH: ${GEN_DEST}")
message(STATUS "aragen_srcs: ${aragen_srcs}")

if(MANIFESTDIR)
    message(STATUS "MANIFESTDIR ${MANIFESTDIR}")
else()
    set(MANIFESTDIR ${CMAKE_CURRENT_SOURCE_DIR}/../../manifest)
    message(STATUS "setting MANIFESTDIR ${MANIFESTDIR}")
endif()


set(${MAIN_EXECUTABLE}_SRC)
list(APPEND ${MAIN_EXECUTABLE}_SRC AsfVirtualDiagnosticTester.cpp)

# Application
set(ALL_COMPILE_SRC
    ${${MAIN_EXECUTABLE}_SRC}
    ${userdata_srcs}
    ${userdata_c_srcs}
    ${userdata_hdrs}
    ${userdata_hpp_hdrs}
    ${aragen_srcs}
    ${aragen_hdrs}
    ${aragendata_srcs}
    ${aragendata_hdrs}
)

# Build------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -D_GLIBCXX_USE_NANOSLEEP -DLINUX")

###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << Start of Build Parameters >>        DO NOT CHANGE THIS COMMENT!
###################################################################################################################################
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -UNDEF_WITH_ARA_IAM")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -UNDEF_WITH_ARA_IAM") 

# if($ENV{AUTOSAR_AP_VERSION} MATCHES "NEUSAR_AP_R1911")
#     message(STATUS "  Using NEUSAR_AP_R1911")
#     set(NEUSAR_AP_R1911 ON)
# elseif($ENV{AUTOSAR_AP_VERSION} MATCHES "NEUSAR_AP_R2111")
#     message(STATUS "Using NEUSAR_AP_R2111")
#     set(NEUSAR_AP_R2111 ON)
# else()
#     message(FATAL_ERROR "Please set AUTOSAR_AP_VERSION evn to proper value")
# endif()

LIST(APPEND ARA_LIBRARIES asf_virtual_diagnostic)

LIST(APPEND ARA_LIBRARIES asf_DiagAuthenticationState)

LIST(APPEND ARA_LIBRARIES http_wrapper)

# LIST(APPEND ARA_LIBRARIES security_event_wrapper crypto_wrapper crypto ssl log_wrapper PocoZip PocoFoundation dl tar z)
LIST(APPEND ARA_LIBRARIES crypto_wrapper crypto ssl log_wrapper PocoZip PocoFoundation dl tar z)

#add_definitions(-DHEALTH_RELIABILITY_CHAIN_OPTION)

###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of Build Parameters >>          DO NOT CHANGE THIS COMMENT!
###################################################################################################################################

add_executable(AsfVirtualDiagnosticTester ${ALL_COMPILE_SRC})
# target_link_libraries(AsfVirtualDiagnosticTester ${CMAKE_THREAD_LIBS_INIT} ${ARA_LIBRARIES} ${ara_exec_LIBS} ${AdaptivePlatform_LIBS} ${system_interface_LIBS})
target_link_libraries(AsfVirtualDiagnosticTester ${CMAKE_THREAD_LIBS_INIT} ${ARA_LIBRARIES} mqttbridge)

install(TARGETS AsfVirtualDiagnosticTester RUNTIME DESTINATION opt/AsfVirtualDiagnosticTester/bin)
# set(NEUSAR_SDK_PATH "$ENV{SDKTARGETSYSROOT}")
# if (NEUSAR_SDK_PATH)
#     install(FILES ${NEUSAR_SDK_PATH}/usr/NeuSAR_ARA_Version PERMISSIONS OWNER_READ GROUP_READ WORLD_READ DESTINATION opt/AsfVirtualDiagnosticTester/etc)
# endif()
install(DIRECTORY
    ${MANIFESTDIR}/AsfVirtualDiagnosticTester
    DESTINATION opt
)



