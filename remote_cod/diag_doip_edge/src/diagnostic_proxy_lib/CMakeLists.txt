# Cmake least version
cmake_minimum_required(VERSION 3.4.3)

# set CXXFLAGS
if( QNX OR QNXNTO )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pipe -g -std=c++14")
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pipe -g -std=c++14 -lpthread")
endif()

set(APPEND_LIBS ProxyCommon ${neusar_net_LIBS} log_wrapper)

set(AUTHENTICATION_MODE OFF)
set(AUTHENTICATION_RESULT OFF)
set(AUTHENTICATION_STATE ON)

# header file
set(INCLUDE_PATH ${PROJECT_SOURCE_DIR}/src/diagnostic_proxy_lib/include)
include_directories(
    ${PROXYCOMMON_INCLUDE_DIRS}
    ${INCLUDE_PATH}
)

set(LIB_DOIP_SRC
    ${PROJECT_SOURCE_DIR}/src/diagnostic_proxy_lib/doip/doip_activation_line.cpp
    ${PROJECT_SOURCE_DIR}/src/diagnostic_proxy_lib/doip/doip_group_identification.cpp
    ${PROJECT_SOURCE_DIR}/src/diagnostic_proxy_lib/doip/doip_power_mode.cpp
    ${PROJECT_SOURCE_DIR}/src/diagnostic_proxy_lib/doip/doip_trigger_announcement.cpp
    ${PROJECT_SOURCE_DIR}/src/diagnostic_proxy_lib/doip/doip_vehicle_identification.cpp
)

set(LIB_DIAG_AUTHENTICATION_MODE_SRC
    ${PROJECT_SOURCE_DIR}/src/diagnostic_proxy_lib/authentication/diag_authentication_mode.cpp
)

set(LIB_DIAG_AUTHENTICATION_RESULT_SRC
    ${PROJECT_SOURCE_DIR}/src/diagnostic_proxy_lib/authentication/diag_authentication_result.cpp
)

set(LIB_DIAG_AUTHENTICATION_STATE_SRC
    ${PROJECT_SOURCE_DIR}/src/diagnostic_proxy_lib/authentication/diag_authentication_state.cpp
)

add_library(asf_DoipInterface SHARED ${LIB_DOIP_SRC})
target_link_libraries(asf_DoipInterface ${APPEND_LIBS})

install(TARGETS asf_DoipInterface EXPORT
    ${PROJECT_NAME}-config LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES ${INCLUDE_PATH}/diag_proxy_app_common.h
    ${INCLUDE_PATH}/doip_activation_line.h
    ${INCLUDE_PATH}/doip_group_identification.h
    ${INCLUDE_PATH}/doip_power_mode.h
    ${INCLUDE_PATH}/doip_trigger_announcement.h
    ${INCLUDE_PATH}/doip_vehicle_identification.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/midware/diag/
)

if(AUTHENTICATION_MODE)
add_library(asf_DiagAuthenticationMode SHARED ${LIB_DIAG_AUTHENTICATION_MODE_SRC})

target_link_libraries(asf_DiagAuthenticationMode ${APPEND_LIBS})

install(TARGETS asf_DiagAuthenticationMode EXPORT
    ${PROJECT_NAME}-config LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES ${INCLUDE_PATH}/diag_authentication_mode.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/midware/diag/
)
endif()

if(AUTHENTICATION_RESULT)
add_library(asf_DiagAuthenticationResult SHARED ${LIB_DIAG_AUTHENTICATION_RESULT_SRC})

target_link_libraries(asf_DiagAuthenticationResult ${APPEND_LIBS})

install(TARGETS asf_DiagAuthenticationResult EXPORT
    ${PROJECT_NAME}-config LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES ${INCLUDE_PATH}/diag_authentication_result.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/midware/diag/
)
endif()

if(AUTHENTICATION_STATE)
add_library(asf_DiagAuthenticationState SHARED ${LIB_DIAG_AUTHENTICATION_STATE_SRC})

target_link_libraries(asf_DiagAuthenticationState ${APPEND_LIBS})

install(TARGETS asf_DiagAuthenticationState EXPORT
    ${PROJECT_NAME}-config LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES ${INCLUDE_PATH}/diag_authentication_state.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/midware/diag/
)
endif()

install(EXPORT ${PROJECT_NAME}-config DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
