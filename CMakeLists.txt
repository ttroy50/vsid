# @author Thom Troy
# 
# Copyright (C) 2015 Thom Troy
# 

cmake_minimum_required (VERSION 2.8.12)

enable_testing()

project(vsid)

set (vsid_VERSION_MAJOR 0)
set (vsid_VERSION_MINOR 2)
set (vsid_VERSION_PATCH 1)


# Options that can be passed to CMake using 'cmake -DKEY=VALUE'.
option ("BUILD_VSID_PCAP_CLASSIFIER" "Build the pcap classifier" "ON")
option ("BUILD_VSID_PCAP_TRAINER" "Build the vsid pcap trainer" "ON")
option ("BUILD_VSID_NETFILTER_CLASSIFIER" "Build the vsid netfilter classifier" "ON")


set(CONF_INSTALL_DIR "etc/vsid" CACHE STRING "That folder under CMAKE_INSTALL_DIR that conf files will be installed to")

set(SCRIPT_INSTALL_DIR "bin" CACHE STRING "That folder under CMAKE_INSTALL_DIR that script (.py/.sh) will be installed to")

###############################################################
# Set Compiler Flags
##############################################################

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

#TODO add in --pedantic -Wall
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DELPP_THREAD_SAFE")


################################################################
# Modules / includes
#################################################################


set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules
                      ${CMAKE_MODULE_PATH})

set(Boost_USE_STATIC_LIBS OFF) 

find_package(Boost 1.54.0 REQUIRED COMPONENTS unit_test_framework) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
else()
    message (FATAL_ERROR "Cannot find Boost")
endif()

find_package(YamlCpp REQUIRED) 

find_package(PCAP REQUIRED) 

find_package(NetfilterNetlink REQUIRED) 
find_package(NetfilterQueue REQUIRED) 

#################################################################
# Sub projects
#################################################################

add_subdirectory(contrib)
add_subdirectory(src)
add_subdirectory(config)
add_subdirectory(tools)

##################################################################
# Documentation
##################################################################
FIND_PACKAGE(Doxygen)

IF (DOXYGEN_FOUND)

SET(DOXYGEN_INPUT ${CMAKE_BINARY_DIR}/Doxyfile)
SET(DOXYGEN_OUTPUT ${CMAKE_BINARY_DIR}/doxygen)

configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${DOXYGEN_INPUT} @ONLY)

ADD_CUSTOM_COMMAND(
  OUTPUT ${DOXYGEN_OUTPUT}
  COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
  COMMAND ${CMAKE_COMMAND} -E echo "Done."
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${DOXYGEN_INPUT}
  )

ADD_CUSTOM_TARGET(doc DEPENDS ${DOXYGEN_OUTPUT})

ADD_CUSTOM_TARGET(doc_forced
  COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
  COMMAND ${CMAKE_COMMAND} -E echo "Done."
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

ENDIF (DOXYGEN_FOUND)