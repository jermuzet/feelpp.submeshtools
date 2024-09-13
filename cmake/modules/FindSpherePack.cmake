# - Find SPHEREPACK
# Find the native SPHEREPACK includes and library
# You can specify these variables to search
#
#  SPHEREPACK_DIR
#
# The module provides these variables:
#
#  SPHEREPACK_INCLUDES_DIRS - where to find headers.
#  SPHEREPACK_LIBRARY_DIRS  - where to find libraries
#  SPHEREPACK_LIBRARIES     - List of libraries when using SPHEREPACK.
#  SPHEREPACK_FOUND         - True if GSL found.

find_path (SPHEREPACK_INCLUDES spherepack.h HINTS ENV SPHEREPACK_DIR)
if (SPHEREPACK_INCLUDES)
  MESSAGE(STATUS "Found spherepack/spherepack.h in ${SPHEREPACK_INCLUDES}")
else()
  find_path (SPHEREPACK_INCLUDES spherepack/spherepack.h HINTS ENV SPHEREPACK_DIR)
   # if (SPHEREPACK_INCLUDES)
   #   MESSAGE(STATUS "Found spherepack/spherepack.h in ${SPHEREPACK_INCLUDES}")
   # endif()
endif()

find_library (SPHEREPACK_LIBRARIES NAMES sphere
  HINTS $ENV{SPHEREPACK_DIR}/lib
)

set( SPHEREPACK_LIBRARY_DIRS "")
foreach( LIB "${SPHEREPACK_LIBRARIES}" )
    get_filename_component( DIR "${LIB}" DIRECTORY )
    list( APPEND SPHEREPACK_LIBRARY_DIRS "${DIR}" )
endforeach()
list( REMOVE_DUPLICATES SPHEREPACK_LIBRARY_DIRS )

# Backward compat
set( SPHEREPACK_LIB "${SPHEREPACK_LIBRARIES}")
set( SPHEREPACK_INCLUDES "${SPHEREPACK_INCLUDE_DIRS}")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(SPHEREPACK REQUIRED_VARS SPHEREPACK_LIBRARIES SPHEREPACK_INCLUDE_DIRS SPHEREPACK_LIBRARY_DIRS SPHEREPACK_INCLUDES SPHEREPACK_LIB)
