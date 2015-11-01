# Locate netfilter-queue
#
# This module defines
#  NFQUEUE_FOUND, if false, do not try to link to libnetfilter_queue
#  NFQUEUE_LIBRARY, where to find libnetfilter_queue
#  NFQUEUE_INCLUDE_DIR, where to find libnetfilter_queue/libnetfilter_queue.h
#
#
# If NFQUEUE is not installed in a standard path, you can use the NFQUEUE_ROOT_DIR CMake variable
# to tell CMake where netfilter-queue is.

find_path(NFQUEUE_ROOT_DIR
    NAMES libnetfilter_queue/libnetfilter_queue.h
)

find_path(NFQUEUE_INCLUDE_DIR
    NAMES libnetfilter_queue/libnetfilter_queue.h
    HINTS ${NFQUEUE_ROOT_DIR}/include
)

# find thelibrary
find_library(NFQUEUE_LIBRARY
                 NAMES netfilter_queue
                 HINTS ${NFQUEUE_ROOT_DIR}/lib 
                 )

# handle the QUIETLY and REQUIRED arguments and set NFQUEUE_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NFQUEUE DEFAULT_MSG 
            NFQUEUE_INCLUDE_DIR 
            NFQUEUE_LIBRARY
            )

mark_as_advanced(NFQUEUE_ROOT_DIR 
                    NFQUEUE_INCLUDE_DIR 
                    NFQUEUE_LIBRARY
                    )