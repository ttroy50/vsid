# Locate nfnetlink
#
# This module defines
#  NFNETLINK_FOUND, if false, do not try to link to libnfnetlink
#  NFNETLINK_LIBRARY, where to find libnfnetlink
#  NFNETLINK_INCLUDE_DIR, where to find libnfnetlink/libnfnetlink.h
#
#
# If nfnetlink is not installed in a standard path, you can use the NFNETLINK_ROOT_DIR CMake variable
# to tell CMake where nfnetlink is.

find_path(NFNETLINK_ROOT_DIR
    NAMES libnfnetlink/libnfnetlink.h
)

find_path(NFNETLINK_INCLUDE_DIR
    NAMES libnfnetlink/libnfnetlink.h
    HINTS ${NFQUEUE_ROOT_DIR}/include
)

# find the library
find_library(NFNETLINK_LIBRARY
                 NAMES nfnetlink
                 HINTS ${NFNETLINK_ROOT_DIR}/lib 
                 )

# handle the QUIETLY and REQUIRED arguments and set NFNETLINK_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NFNETLINK DEFAULT_MSG 
            NFNETLINK_INCLUDE_DIR 
            NFNETLINK_LIBRARY
            )

mark_as_advanced(NFNETLINK_ROOT_DIR 
                    NFNETLINK_INCLUDE_DIR 
                    NFNETLINK_LIBRARY
                    )