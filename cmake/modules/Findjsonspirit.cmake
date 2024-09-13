# - Find JSON_SPIRIT
# Find the native JSON_SPIRIT includes and library
# You can specify these variables to search
#
#  JSON_SPIRIT_DIR
#
# The module provides these variables:
#
#  JSON_SPIRIT_INCLUDES_DIRS - where to find headers.
#  JSON_SPIRIT_LIBRARY_DIRS  - where to find libraries
#  JSON_SPIRIT_LIBRARIES     - List of libraries when using JSON_SPIRIT.
#  JSON_SPIRIT_FOUND         - True if GSL found.

find_path (JSON_SPIRIT_INCLUDE_DIRS NAMES json_spirit.h
    HINTS $ENV{JSON_SPIRIT_DIR}/include
)

find_library (JSON_SPIRIT_LIBRARIES NAMES json_spirit
  HINTS $ENV{JSON_SPIRIT_DIR}/lib
)

set( JSON_SPIRIT_LIBRARY_DIRS "")
foreach( LIB "${JSON_SPIRIT_LIBRARIES}" )
    get_filename_component( DIR "${LIB}" DIRECTORY )
    list( APPEND JSON_SPIRIT_LIBRARY_DIRS "${DIR}" )
endforeach()
list( REMOVE_DUPLICATES JSON_SPIRIT_LIBRARY_DIRS )

# Backward compat
set( JSON_SPIRIT_INCLUDE_DIR "${JSON_SPIRIT_INCLUDE_DIRS}")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(JSON_SPIRIT REQUIRED_VARS JSON_SPIRIT_LIBRARIES JSON_SPIRIT_INCLUDE_DIRS JSON_SPIRIT_LIBRARY_DIRS JSON_SPIRIT_INCLUDE_DIR)
