# - Find GSL
# Find the native GSL includes and library
# You can specify these variables to search
#
#  GSL_DIR
#
# The module provides these variables:
#
#  GSL_INCLUDES_DIRS - where to find headers.
#  GSL_LIBRARY_DIRS  - where to find libraries
#  GSL_LIBRARIES     - List of libraries when using GSL.
#  GSL_FOUND         - True if GSL found.


# If Cblas is missing, use cblas cmake module.

find_path (GSL_INCLUDE_DIRS NAMES gsl/gsl_math.h gsl/gsl_cadna.h
    HINTS $ENV{GSL_DIR}/include
)

find_library (GSL_LIBRARIES NAMES gsl
    HINTS $ENV{GSL_DIR}/lib
)

set( GSL_LIBRARY_DIRS "")
foreach( LIB "${GSL_LIBRARIES}" )
    get_filename_component( DIR "${LIB}" DIRECTORY )
    list( APPEND GSL_LIBRARY_DIRS "${DIR}" )
endforeach()
list( REMOVE_DUPLICATES GSL_LIBRARY_DIRS )

# Backward compat
set( GSL_LIB "${GSL_LIBRARIES}" )
set( GSL_INCLUDES "${GSL_INCLUDE_DIRS}" )

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(GSL REQUIRED_VARS GSL_LIBRARIES GSL_INCLUDE_DIRS GSL_LIBRARY_DIRS GSL_INCLUDES GSL_LIB)
