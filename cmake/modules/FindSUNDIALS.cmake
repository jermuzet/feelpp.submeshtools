# - Find SUNDIALS
# Find the native SUNDIALS includes and library
# You can specify these variables to search
#
#  SUNDIALS_DIR
#
# The module provides these variables:
#
#  SUNDIALS_INCLUDES_DIRS - where to find headers.
#  SUNDIALS_LIBRARY_DIRS  - where to find libraries
#  SUNDIALS_LIBRARIES     - List of libraries when using SUNDIALS.
#  SUNDIALS_FOUND         - True if GSL found.

find_path (SUNDIALS_INCLUDE_DIRS NAMES sundials/sundials_config.h
    HINTS $ENV{SUNDIALS_DIR}/include
)

find_library (SUNDIALS_LIBRARIES NAMES sundials_ida sundials_nvecserial
  HINTS $ENV{SUNDIALS_DIR}/lib
)

set( SUNDIALS_LIBRARY_DIRS "")
foreach( LIB "${SUNDIALS_LIBRARIES}" )
    get_filename_component( DIR "${LIB}" DIRECTORY )
    list( APPEND SUNDIALS_LIBRARY_DIRS "${DIR}" )
endforeach()
list( REMOVE_DUPLICATES SUNDIALS_LIBRARY_DIRS )

# Backward compat
set( SUNDIALS_INCLUDES "${SUNDIALS_INCLUDE_DIRS}")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(SUNDIALS REQUIRED_VARS SUNDIALS_LIBRARIES SUNDIALS_INCLUDE_DIRS SUNDIALS_LIBRARY_DIRS SUNDIALS_INCLUDES)
