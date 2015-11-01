project(vsid_pcap_classifier)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_SOURCE_DIR}/contrib/easyloggingpp)
include_directories(${CMAKE_SOURCE_DIR}/contrib/hash/inc)
include_directories(${CMAKE_SOURCE_DIR}/src/protocol_model/inc)
include_directories(${CMAKE_SOURCE_DIR}/src/common/inc)

set(vsid_pcap_classifier_SOURCES 
    src/Config.cpp
    src/PcapReader.cpp
    src/main.cpp
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(${PROJECT_NAME} ${vsid_pcap_classifier_SOURCES})

target_link_libraries(${PROJECT_NAME}
    vsid_common
    vsid_protocol_model
    vsid_hash
    ${YAMLCPP_LIBRARY}
    ${PCAP_LIBRARY}
)

###################################################
# Install

# Binaries
install (TARGETS ${PROJECT_NAME} DESTINATION bin)

# Config
install (FILES ${CMAKE_SOURCE_DIR}/config/config_pcap_classifier.yaml DESTINATION ${CONF_INSTALL_DIR})
install (FILES ${CMAKE_SOURCE_DIR}/config/logging_pcap_classifier.conf DESTINATION ${CONF_INSTALL_DIR})