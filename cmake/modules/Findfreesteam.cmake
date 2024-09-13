# - Find FREESTEAM
# Find the native FREESTEAM includes and library
# You can specify these variables to search
#
#  FREESTEAM_DIR
#
# The module provides these variables:
#
#  FREESTEAM_INCLUDES_DIRS - where to find headers.
#  FREESTEAM_LIBRARY_DIRS  - where to find libraries
#  FREESTEAM_LIBRARIES     - List of libraries when using FREESTEAM.
#  FREESTEAM_FOUND         - True if GSL found.

find_path (FREESTEAM_INCLUDE_DIRS NAMES freesteam/steam.h
    HINTS $ENV{FREESTEAM_DIR}/include
)

find_library (FREESTEAM_LIBRARIES NAMES freesteam
  HINTS $ENV{FREESTEAM_DIR}/lib
)

set( FREESTEAM_LIBRARY_DIRS "")
foreach( LIB "${FREESTEAM_LIBRARIES}" )
    get_filename_component( DIR "${LIB}" DIRECTORY )
    list( APPEND FREESTEAM_LIBRARY_DIRS "${DIR}" )
endforeach()
list( REMOVE_DUPLICATES FREESTEAM_LIBRARY_DIRS )

# Backward compat
set( FREESTEAM_LIB "${FREESTEAM_LIBRARIES}")
set( FREESTEAM_INCLUDES "${FREESTEAM_INCLUDE_DIRS}")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(FREESTEAM REQUIRED_VARS FREESTEAM_LIBRARIES FREESTEAM_INCLUDE_DIRS FREESTEAM_LIBRARY_DIRS FREESTEAM_INCLUDES FREESTEAM_LIB)
