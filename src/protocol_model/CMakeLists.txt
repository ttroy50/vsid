project(vsid_protocol_model)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_SOURCE_DIR}/contrib/easyloggingpp)
include_directories(${CMAKE_SOURCE_DIR}/contrib/hash/inc)
include_directories(${CMAKE_SOURCE_DIR}/src/common/inc)

set(vsid_protocol_model_SOURCES 
    src/ProtocolModelDb.cpp
    src/ProtocolModel.cpp
    src/AttributeMeter.cpp
    src/AttributeMeterFactory.cpp
    src/ByteFrequency.cpp
    src/DirectionBytesCountMeter.cpp
    src/ByteFrequencyFirstOrigToDestPacket.cpp
    src/First4ByteFrequencyMeter.cpp
    src/First16ByteFrequencyMeter.cpp
    src/DirectionChangesMeter.cpp
    src/ByteFrequencyFirstDestToOrigPacket.cpp
    src/DirectionBytesCountFirst10PacketMeter.cpp
    src/ByteFrequencyFirst8PacketsMeter.cpp
    src/DirectionChangesFirst8PacketsMeter.cpp
    src/RtmpRegexMatchMeter.cpp
    src/EntropyFirstOrigToDestPacket.cpp
    src/ActionReactionFirst3ByteHashMeter.cpp
    src/PayloadSizeFirstPacketMeter.cpp
    src/FirstBitPositionsMeter.cpp
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_library(${PROJECT_NAME} STATIC ${vsid_protocol_model_SOURCES})


#############################################
# Unit tests 
# TODO move to unit_test folder?

enable_testing()

add_executable(test_model unit_test/test_model.cpp)

target_link_libraries(test_model
    vsid_hash
    vsid_common
    vsid_protocol_model
    ${YAMLCPP_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_definitions (-DBOOST_TEST_DYN_LINK)

add_test(test_model test_model ${CMAKE_CURRENT_SOURCE_DIR}/unit_test/test_db.yaml.totest)