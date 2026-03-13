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

message(STATUS "---------------------------------------------------")
message(STATUS "./src/ - Importing dependencies -------------------")

if($ENV{NEUSAR_WITH_CONTAINER} MATCHES "YES")
add_definitions(-DWITH_CONTAINER)
endif()

add_definitions(-DUDS_ON_NEUSAR_NET)

#add_definitions(-DGATEWAY_SYNC)

#add_definitions(-DPARTITION_SYNC)

#add_definitions(-DAUTH_DEAL_MESSAGE)

add_definitions(-DAUTH_CHECK_MESSAGE)

add_definitions(-DAUTH_TARGET_EXEMPTION)

add_definitions(-DSOCKET_KEEPALIVE)

# Application
include_directories(
    ${PROXYCOMMON_INCLUDE_DIRS}
    ${PROXY_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src/diagnostic_proxy/
    ${PROJECT_SOURCE_DIR}/src/diagnostic_proxy/includes/
)

set(DiagnosticProxy_src
    main.cpp
    runtime_proxy.cpp
    common/diagnostic_proxy.cpp
    common/random_number_generator.cpp
    common/runtime.cpp
    common/wheels_timer_executor.cpp
    common/wheels_timer.cpp
    common/dcm/connection/uds_message.cpp
    common/dcm/connection/uds_message_ext.cpp
    common/dcm/route/route.cpp
    dcm/diagnostic_communication_manager.cpp
    dcm/arbitration/arbitration_timer.cpp
    dcm/arbitration/arbitration.cpp
    dcm/authentication/authentication_timer.cpp
    dcm/authentication/authentication.cpp
    dcm/connection/connection.cpp
    dcm/connection/do_ip_communication.cpp
    dcm/connection/do_ip_connection.cpp
    dcm/connection/do_ip_connection_manager.cpp
    dcm/connection/do_ip_constants.cpp
    dcm/connection/do_ip_handler.cpp
    dcm/connection/do_ip_packet.cpp
    dcm/connection/do_ip_udp_handler.cpp
    dcm/connection/do_ip_udp_info.cpp
    dcm/connection/entity_identification.cpp
    dcm/connection/uds_on_neusar_net.cpp
    dcm/conversation/conversation.cpp
    dcm/conversation/conversation_manager.cpp
    dcm/route/route_factory.cpp
    dcm/route/routing_table.cpp
    dcm/route/routing_timer.cpp
    dcm/route/routing.cpp
    dcm/route/gateway/gateway_processor.cpp
    dcm/route/gateway/gateway_route.cpp
    dcm/route/gateway/gateway_sync.cpp
    dcm/route/node/node_processor.cpp
    dcm/route/node/node_route.cpp
    dcm/route/partition/partition_processor.cpp
    dcm/route/partition/partition_route.cpp
    dcm/route/partition/partition_sync_table.cpp
    dcm/route/partition/partition_sync.cpp
    dcm/route/routing/routing_connection_factory.cpp
    dcm/route/routing/routing_connection.cpp
    dcm/route/routing/routing_do_ip_connection.cpp
    dcm/route/routing/routing_pripro_client.cpp
    dcm/route/routing/routing_pripro_connection.cpp
    dcm/route/routing/routing_pripro_server.cpp
    dcm/route/routing/routing_pripro_transmision_protocol.cpp
    dcm/tester/tester_table.cpp
    uds_transport/transport_protocol_mgr.cpp
)

# Append generated sources
foreach(f IN LISTS dmMiddlewareInterfaces_SOURCES)
    if(f MATCHES ".*vsomeip_service_mapping-.*\\.cpp")
        # Intentionally empty.
        # DM has a custom service_mapping file, which solves the issue
        # of having several auto-generated mappings. So auto-generated
        # ones shouldn't be added to sources here.
    else()
        list(APPEND DiagnosticProxy_src ${f})
    endif()
endforeach()





if(1)
add_library(diagnostic_proxy STATIC ${DiagnosticProxy_src})

target_link_libraries (diagnostic_proxy ${PROXYCOMMON_LIBRARIES})
target_link_libraries (diagnostic_proxy ${ARA_LIBRARIES})
# if(NEUSAR_AP_R1911)
# target_link_libraries (diagnostic_proxy ${ARAEXEC_LIBRARIES})
# target_link_libraries (diagnostic_proxy ${ADAPTIVEPLATFORM_LIBRARIES})
# endif()
# if(NEUSAR_AP_R2111)
# target_link_libraries (diagnostic_proxy ${AraCore_LIBS})
# target_link_libraries (diagnostic_proxy ${ara_exec_LIBS})
# target_link_libraries (diagnostic_proxy ${AdaptivePlatform_LIBS})
# endif()
target_link_libraries (diagnostic_proxy ${CMAKE_DL_LIBS})
target_link_libraries (diagnostic_proxy Threads::Threads)

message(STATUS "A: " ${PROXYCOMMON_LIBRARIES})

install(TARGETS diagnostic_proxy DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
else()
add_executable(AsfDiagnosticProxy ${DiagnosticProxy_src})

target_link_libraries (AsfDiagnosticProxy ${PROXYCOMMON_LIBRARIES})
target_link_libraries (AsfDiagnosticProxy ${ARA_LIBRARIES})
# if(NEUSAR_AP_R1911)
# target_link_libraries (AsfDiagnosticProxy ${ARAEXEC_LIBRARIES})
# target_link_libraries (AsfDiagnosticProxy ${ADAPTIVEPLATFORM_LIBRARIES})
# endif()
# if(NEUSAR_AP_R2111)
# target_link_libraries (AsfDiagnosticProxy ${AraCore_LIBS})
# target_link_libraries (AsfDiagnosticProxy ${ara_exec_LIBS})
# target_link_libraries (AsfDiagnosticProxy ${AdaptivePlatform_LIBS})
# endif()
target_link_libraries (AsfDiagnosticProxy ${CMAKE_DL_LIBS})
target_link_libraries (AsfDiagnosticProxy Threads::Threads)

message(STATUS "A: " ${PROXYCOMMON_LIBRARIES})

install(TARGETS AsfDiagnosticProxy DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
