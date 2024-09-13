# - Find FADBAD
# Find the native FADBAD includes and library
#  You can specify these variables to search
#
#  FADBAD_DIR
#
# The module provides these variables:
#
#  FADBAD_INCLUDE_DIRS - where to find headers.
#  FADBAD_FOUND         - True if GSL found.

find_path (FADBAD_INCLUDE_DIRS NAMES fadbad.h
    HINTS $ENV{FADBAD_DIR}/include
)

# Backward compat
set( FADBAD_INCLUDE_DIR "${FADBAD_INCLUDE_DIRS}")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(FADBAD REQUIRED_VARS FADBAD_INCLUDE_DIRS FADBAD_INCLUDE_DIR)
