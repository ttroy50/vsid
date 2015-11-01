project(vsid_nfqueue_test)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_SOURCE_DIR}/contrib/easyloggingpp)
include_directories(${CMAKE_SOURCE_DIR}/contrib/hash/inc)
include_directories(${CMAKE_SOURCE_DIR}/src/protocol_model/inc)
include_directories(${CMAKE_SOURCE_DIR}/src/common/inc)

set(vsid_nfqueue_test_SOURCES 
    src/Config.cpp
    src/Process.cpp
    src/PacketHandler.cpp
    src/main.cpp
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(${PROJECT_NAME} ${vsid_nfqueue_test_SOURCES})

target_link_libraries(${PROJECT_NAME}
    vsid_common
    vsid_protocol_model
    vsid_hash
    ${YAMLCPP_LIBRARY}
    ${PCAP_LIBRARY}
    ${NFNETLINK_LIBRARY}
    ${NFQUEUE_LIBRARY}
)

###################################################
# Install

# Binaries
install (TARGETS ${PROJECT_NAME} DESTINATION bin)

# Config
install (FILES ${CMAKE_SOURCE_DIR}/config/config_nfqueue_test.yaml DESTINATION ${CONF_INSTALL_DIR})