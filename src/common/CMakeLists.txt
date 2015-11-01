project(vsid_common)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_SOURCE_DIR}/contrib/easyloggingpp)
include_directories(${CMAKE_SOURCE_DIR}/contrib/hash/inc)
include_directories(${CMAKE_SOURCE_DIR}/src/protocol_model/inc)

set(vsid_common_SOURCES 
    src/Flow.cpp
    src/FlowManager.cpp
    src/IPv4.cpp
    src/CommonConfig.cpp
    src/FlowClassificationLogger.cpp
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_library(${PROJECT_NAME} STATIC ${vsid_common_SOURCES})

#############################################
# Unit tests 
# TODO move to unit_test folder?

enable_testing()

add_executable(test_common unit_test/test_common.cpp)

target_link_libraries(test_common
    vsid_hash
    vsid_common
    vsid_protocol_model
    ${YAMLCPP_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_definitions (-DBOOST_TEST_DYN_LINK)

add_test(test_common test_common)