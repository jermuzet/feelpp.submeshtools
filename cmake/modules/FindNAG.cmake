# - Find NAG
# Find the native NAG includes and library
#
#  GSL_INCLUDES    - where to find gsl/gsl_*.h, etc.
#  GSL_LIBRARIES   - List of libraries when using GSL.
#  GSL_FOUND       - True if GSL found.

# from https://github.com/jedbrown/cmake-modules/blob/master/FindGSL.cmake


find_library (NAG_LIB NAMES nag_nag
  HINTS ENV NAG_DIR /opt/NAG/fll6a23dfl
)
# if (GSL_LIB)
#    MESSAGE(STATUS "Found gsl : ${GSL_LIB}")
# endif()


set (NAG_LIBRARIES "${NAG_LIB}" )

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (NAG DEFAULT_MSG NAG_LIBRARIES)

mark_as_advanced (NAG_LIBRARIES)
