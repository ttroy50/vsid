project(vsid_training)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_SOURCE_DIR}/contrib/easyloggingpp)
include_directories(${CMAKE_SOURCE_DIR}/contrib/hash/inc)
include_directories(${CMAKE_SOURCE_DIR}/src/protocol_model/inc)
include_directories(${CMAKE_SOURCE_DIR}/src/common/inc)

set(vsid_training_SOURCES 
    src/Config.cpp
    src/PcapReader.cpp
    src/ProtocolModelUpdater.cpp
    src/TrainingInput.cpp
    src/main.cpp
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(${PROJECT_NAME} ${vsid_training_SOURCES})

target_link_libraries(${PROJECT_NAME}
    vsid_hash
    vsid_common
    vsid_protocol_model
    ${YAMLCPP_LIBRARY}
    ${PCAP_LIBRARY}
)

###################################################
# Install

# Binaries
install (TARGETS ${PROJECT_NAME} DESTINATION bin)

# Config
install (FILES ${CMAKE_SOURCE_DIR}/config/config_training.yaml DESTINATION ${CONF_INSTALL_DIR})
install (FILES ${CMAKE_SOURCE_DIR}/config/training_input.yaml DESTINATION ${CONF_INSTALL_DIR})


