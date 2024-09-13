# - Find POPT
# Find the native POPT includes and library
# You can specify these variables to search
#
#  POPT_DIR
#
# The module provides these variables:
#
#  POPT_INCLUDES_DIRS - where to find headers.
#  POPT_LIBRARY_DIRS  - where to find libraries
#  POPT_LIBRARIES     - List of libraries when using POPT.
#  POPT_FOUND         - True if GSL found.

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
  pkg_search_module(PC_POPT QUIET popt)
endif ()

find_path (POPT_INCLUDE_DIRS NAMES popt.h
  HINTS $ENV{POPT_DIR}/include
        ${PC_POPT_LIBRARYDIR}
        ${PC_POPT_LIBRARY_DIRS}
)

find_library (POPT_LIBRARIES NAMES popt
  HINTS $ENV{POPT_DIR}/lib
        ${PC_POPT_LIBRARYDIR}
        ${PC_POPT_LIBRARY_DIRS}
)

set( POPT_LIBRARY_DIRS "")
foreach( LIB "${POPT_LIBRARIES}" )
    get_filename_component( DIR "${LIB}" DIRECTORY )
    list( APPEND POPT_LIBRARY_DIRS "${DIR}" )
endforeach()
list( REMOVE_DUPLICATES POPT_LIBRARY_DIRS )

# Backward compat
set( POPT_LIBRARY "${POPT_LIBRARIES}")
set( POPT_INCLUDE_DIR "${POPT_INCLUDE_DIRS}")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(POPT REQUIRED_VARS POPT_LIBRARIES POPT_INCLUDE_DIRS POPT_LIBRARY_DIRS POPT_INCLUDE_DIR POPT_LIBRARY)
