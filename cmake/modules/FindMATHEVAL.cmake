# - Find MATHEVAL
# Find the native MATHEVAL includes and library
# You can specify these variables to search
#
#  MATHEVAL_DIR
#
# The module provides these variables:
#
#  MATHEVAL_INCLUDES_DIRS - where to find headers.
#  MATHEVAL_LIBRARY_DIRS  - where to find libraries
#  MATHEVAL_LIBRARIES     - List of libraries when using MATHEVAL.
#  MATHEVAL_FOUND         - True if GSL found.

find_path (MATHEVAL_INCLUDE_DIRS NAMES matheval.h
    HINTS $ENV{MATHEVAL_DIR}/include
)

find_library (MATHEVAL_LIBRARIES NAMES matheval
  HINTS $ENV{MATHEVAL_DIR}/lib
)

set( MATHEVAL_LIBRARY_DIRS "")
foreach( LIB "${MATHEVAL_LIBRARIES}" )
    get_filename_component( DIR "${LIB}" DIRECTORY )
    list( APPEND MATHEVAL_LIBRARY_DIRS "${DIR}" )
endforeach()
list( REMOVE_DUPLICATES MATHEVAL_LIBRARY_DIRS )

# Backward compat
set( MATHEVAL_LIB_DIR "${MATHEVAL_LIBRARY_DIRS}")
set( MATHEVAL_INCLUDE_DIR "${MATHEVAL_INCLUDE_DIRS}")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(MATHEVAL REQUIRED_VARS MATHEVAL_LIBRARIES MATHEVAL_INCLUDE_DIRS MATHEVAL_LIBRARY_DIRS MATHEVAL_INCLUDE_DIR MATHEVAL_LIB_DIR)
