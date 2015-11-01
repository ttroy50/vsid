project(vsid_hash)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/murmur3.c PROPERTIES LANGUAGE CXX )

set(vsid_hash_SOURCES 
    src/murmur3.c
)

add_library(${PROJECT_NAME} STATIC ${vsid_hash_SOURCES})