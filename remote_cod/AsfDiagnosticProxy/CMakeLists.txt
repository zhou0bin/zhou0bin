
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
# find_package(AraLog REQUIRED)
# find_package(AraCore REQUIRED)
# find_package(AraPer REQUIRED)



# find_package(AraIamApi REQUIRED)

# set(ARA_LIBRARIES diagnostic_proxy ProxyCommon ${AraCore_LIBS} ${ara_log_LIBS} ${ara_phm_LIBS} apd::rng ${Boost_LIBRARIES} ${iam_api_LIBS})
set(ARA_LIBRARIES diagnostic_proxy ProxyCommon)


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

set(APP_NAME AsfDiagnosticProxy)
set(MAIN_EXECUTABLE AsfDiagnosticProxy)
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
list(APPEND ${MAIN_EXECUTABLE}_SRC AsfDiagnosticProxy.cpp)

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

LIST(APPEND ARA_LIBRARIES mqttbridge)

find_package(neusar_net REQUIRED)
LIST(APPEND ARA_LIBRARIES ${neusar_net_LIBS})

find_package(log_wrapper REQUIRED)
LIST(APPEND ARA_LIBRARIES log_wrapper)
add_definitions(-DUSE_MIDWARE_LOG)

###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of Build Parameters >>          DO NOT CHANGE THIS COMMENT!
###################################################################################################################################

add_executable(AsfDiagnosticProxy ${ALL_COMPILE_SRC})
# target_link_libraries(AsfDiagnosticProxy ${CMAKE_THREAD_LIBS_INIT} ${ARA_LIBRARIES} ${ara_exec_LIBS} ${AdaptivePlatform_LIBS})
target_link_libraries(AsfDiagnosticProxy ${CMAKE_THREAD_LIBS_INIT} ${ARA_LIBRARIES})
install(TARGETS AsfDiagnosticProxy RUNTIME DESTINATION opt/AsfDiagnosticProxy/bin)
# set(NEUSAR_SDK_PATH "$ENV{SDKTARGETSYSROOT}")
# if (NEUSAR_SDK_PATH)
# 	install(FILES ${NEUSAR_SDK_PATH}/usr/NeuSAR_ARA_Version PERMISSIONS OWNER_READ GROUP_READ WORLD_READ DESTINATION opt/AsfDiagnosticProxy/etc)
# endif()
install(DIRECTORY
    ${MANIFESTDIR}/AsfDiagnosticProxy
    DESTINATION opt
)



