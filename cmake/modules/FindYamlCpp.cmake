# - Find YAMLCPP
# Find the native YAMLCPP includes and library
# You can specify these variables to search
#
#  YAMLCPP_DIR
#
# The module provides these variables:
#
#  YAMLCPP_INCLUDES_DIRS - where to find headers.
#  YAMLCPP_LIBRARY_DIRS  - where to find libraries
#  YAMLCPP_LIBRARIES     - List of libraries when using YAMLCPP.
#  YAMLCPP_FOUND         - True if GSL found.

find_path (YAMLCPP_INCLUDE_DIRS NAMES yaml-cpp/yaml.h
    HINTS $ENV{YAMLCPP_DIR}/include
)

find_library (YAMLCPP_LIBRARIES NAMES libyaml-cpp
  HINTS $ENV{YAMLCPP_DIR}/lib
)

set( YAMLCPP_LIBRARY_DIRS "")
foreach( LIB "${YAMLCPP_LIBRARIES}" )
    get_filename_component( DIR "${LIB}" DIRECTORY )
    list( APPEND YAMLCPP_LIBRARY_DIRS "${DIR}" )
endforeach()
list( REMOVE_DUPLICATES YAMLCPP_LIBRARY_DIRS )

# Backward compat
set( YAMLCPP_LIBRARY "${YAMLCPP_LIBRARIES}")
set( YAMLCPP_INCLUDE_DIR "${YAMLCPP_INCLUDE_DIRS}")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(YAMLCPP REQUIRED_VARS YAMLCPP_LIBRARIES YAMLCPP_INCLUDE_DIRS YAMLCPP_LIBRARY_DIRS YAMLCPP_INCLUDE_DIR YAMLCPP_LIBRARY)
