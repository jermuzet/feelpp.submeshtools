# - Find EXPOKIT
# Find the native EXPOKIT includes and library
# You can specify these variables to search
#
#  EXPOKIT_DIR
#
# The module provides these variables:
#
#  EXPOKIT_LIBRARY_DIRS  - where to find libraries
#  EXPOKIT_LIBRARIES     - List of libraries when using EXPOKIT.
#  EXPOKIT_FOUND         - True if GSL found.

find_library (EXPOKIT_LIBRARIES NAMES expokit
  HINTS $ENV{EXPOKIT_DIR}/lib
)

set( EXPOKIT_LIBRARY_DIRS "")
foreach( LIB "${EXPOKIT_LIBRARIES}" )
    get_filename_component( DIR "${LIB}" DIRECTORY )
    list( APPEND EXPOKIT_LIBRARY_DIRS "${DIR}" )
endforeach()
list( REMOVE_DUPLICATES EXPOKIT_LIBRARY_DIRS )

# Backward compat
set( EXPOKIT_LIB "${EXPOKIT_LIBRARIES}")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(EXPOKIT REQUIRED_VARS EXPOKIT_LIBRARIES EXPOKIT_LIBRARY_DIRS EXPOKIT_LIB)
