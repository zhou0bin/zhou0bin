######################################################################################################################
#   COPYRIGHT                                                                                                        #
#   -----------------------------------------------------------------------------------------------------------------#
#             This software is copyright protected and proprietary to Neusoft Reach.                                 #
#             Neusoft Reach grants to you only those rights as set out in the license conditions.                    #
#             All other rights remain with Neusoft Reach.                                                            #
#   -----------------------------------------------------------------------------------------------------------------#
# ####################################################################################################################
# Cmake least version
cmake_minimum_required(VERSION 3.4)
project(scriptdaemon)

set(CMAKE_VERBOSE_MAKEFILE on)
include(GNUInstallDirs)

# if($ENV{AUTOSAR_AP_VERSION} MATCHES "NEUSAR_AP_R1911")
#     message(STATUS "  Using NEUSAR_AP_R1911")
#     set(NEUSAR_AP_R1911 ON)
# elseif($ENV{AUTOSAR_AP_VERSION} MATCHES "NEUSAR_AP_R2111")
#     message(STATUS "Using NEUSAR_AP_R2111")
#    set(NEUSAR_AP_R2111 ON)
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

set(ALL_COMPILE_SRC
    src/common.cpp
    src/condition.cpp
    src/context.cpp
    src/loop.cpp
    src/manager.cpp
    src/server.cpp
    src/task.cpp
    src/tester.cpp
    src/vehicle_data.cpp
)

# Build-------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -Wall -Wextra -Wpedantic -Wsign-promo -Wnon-virtual-dtor -Wctor-dtor-privacy -Woverloaded-virtual -Wold-style-cast -Wpointer-arith -Wshadow -Wunused -Wuninitialized -Winit-self -Wfloat-equal -Werror")

###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << Start of Build Parameters >>        DO NOT CHANGE THIS COMMENT!
###################################################################################################################################

find_package(Boost 1.72.0 REQUIRED COMPONENTS system filesystem thread)
LIST(APPEND ARA_LIBRARIES ${Boost_LIBRARIES})

find_package(log_wrapper REQUIRED)
LIST(APPEND ARA_LIBRARIES ${log_wrapper_LIB})

set(USE_MIDWARE_LOG ON CACHE BOOL "Use midware log.")
if(USE_MIDWARE_LOG)
    add_definitions(-DUSE_MIDWARE_LOG)
endif()

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

###################################################################################################################################
# [NeuSAR Sign]DO NOT CHANGE THIS COMMENT!           << End of Build Parameters >>          DO NOT CHANGE THIS COMMENT!
###################################################################################################################################

add_library(daemonsrcipt STATIC  ${ALL_COMPILE_SRC})
target_link_libraries(daemonsrcipt ${ARA_LIBRARIES})
target_link_libraries(daemonsrcipt ${CMAKE_DL_LIBS})
target_link_libraries(daemonsrcipt ${py_version})
target_include_directories(daemonsrcipt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS daemonsrcipt DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)
