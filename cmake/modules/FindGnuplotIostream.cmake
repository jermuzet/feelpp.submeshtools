# - Find GNUPLOT_IOSTREAM
# Find the native GNUPLOT_IOSTREAM includes and library
# You can specify these variables to search
#
#  GNUPLOT_IOSTREAM_DIR
#
# The module provides these variables:
#
#  GNUPLOT_IOSTREAM_INCLUDES_DIRS - where to find headers.
#  GNUPLOT_IOSTREAM_FOUND         - True if GSL found.

find_path (GNUPLOT_IOSTREAM_INCLUDE_DIRS NAMES gnuplot-iostream.h
    HINTS $ENV{GNUPLOT_IOSTREAM_DIR}/include
)

# Backward compat
set( GNUPLOT_IOSTREAM_INCLUDE_DIR "${GNUPLOT_IOSTREAM_INCLUDE_DIRS}")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(GNUPLOT_IOSTREAM REQUIRED_VARS GNUPLOT_IOSTREAM_INCLUDE_DIRS GNUPLOT_IOSTREAM_INCLUDE_DIR)
