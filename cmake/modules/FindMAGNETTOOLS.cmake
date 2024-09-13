###  TEMPLATE.txt.tpl; coding: utf-8 ---
# To force autoreconf just export this environment variable
#
# export HIFIMAGNET_AUTORECONF_FORCE=TRUE
#
# This module provides these variables:
#
# MAGNETTOOLS_FOUND		MAGNETTOOLS has been successfully found
# MAGNETTOOLS_INCLUDE_DIR       the include directories
# MAGNETTOOLS_LIBRARIES         MAGNETTOOLS library and its dependencies (if any)
#

set (MAGNETTOOLS_LIBS mesh postprocessing utils numinteg fe_equation magnetfield)
set( MAGNETTOOLS_SUBDIR "BoundaryConditions" "Cooling" "Equations" "Integration" "MagnetField" "Material" "Matrix" "Mesh" "Optimization" "PostProcessing" "Python" "Scripts" "Shapes" "Tests" "Utils" )

MESSAGE(STATUS "MAGNETTOOLS_DIR=${MAGNETTOOLS_DIR}")

FIND_PATH(MAGNETTOOLS_BIN_DIR B_Map
  PATHS ${MAGNETTOOLS_DIR}/bin
  )
MESSAGE(STATUS "MAGNETTOOLS_BIN_DIR=${MAGNETTOOLS_BIN_DIR}")

FIND_PATH(MAGNETTOOLS_INCLUDE_DIR_PREFIX Load_BField.h
  PATHS /usr/include/MagnetTools ${MAGNETTOOLS_DIR}/include ${MAGNETTOOLS_DIR}/include/MagnetTools
  )
MESSAGE(STATUS "MAGNETTOOLS_INCLUDE_DIR_PREFIX=${MAGNETTOOLS_INCLUDE_DIR_PREFIX}")
if ( NOT MAGNETTOOLS_INCLUDE_DIR_PREFIX )
	MESSAGE(FATAL_ERROR "[HiFiMagnet] cannot find MagnetTools (Load_BField.h)")
endif()

set(MAGNETTOOLS_INCLUDE_DIR ${MAGNETTOOLS_INCLUDE_DIR_PREFIX})
foreach( _SUBDIR ${MAGNETTOOLS_SUBDIR} )
  list(APPEND MAGNETTOOLS_INCLUDE_DIR ${MAGNETTOOLS_INCLUDE_DIR_PREFIX}/${_SUBDIR} )
endforeach()
MESSAGE(STATUS "MAGNETTOOLS_INCLUDE_DIR=${MAGNETTOOLS_INCLUDE_DIR}")

foreach(_l ${MAGNETTOOLS_LIBS})
  #SET(TMP_${_l})
  FIND_LIBRARY(TMP_${_l} ${_l}
    PATHS /usr/lib/x86_64-linux-gnu/MagnetTools ${MAGNETTOOLS_DIR}/lib ${MAGNETTOOLS_DIR}/lib/MagnetTools
    )
  if(TMP_${_l})
    list(APPEND MAGNETTOOLS_LIBRARY "${TMP_${_l}}")
  else()
	  message(FATAL_ERROR "[MagnetTools] ${_l} not found : ${MAGNETTOOLS_DIR}/lib" )
  endif()
endforeach()

set( MAGNETTOOLS_INCLUDE_DIRS ${MAGNETTOOLS_INCLUDE_DIR}) #+contrib
set( MAGNETTOOLS_LIBRARIES ${MAGNETTOOLS_LIBRARY}) #+contrib

if (MAGNETTOOLS_INCLUDE_DIR AND MAGNETTOOLS_LIBRARIES)
  set(MAGNETTOOLS_FIND_QUIETLY TRUE)
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MAGNETTOOLS REQUIRED_VARS MAGNETTOOLS_LIBRARIES MAGNETTOOLS_INCLUDE_DIR)
if( MAGNETTOOLS_FOUND )
  message(STATUS "[MagnetTools] \nincludes: ${MAGNETTOOLS_INCLUDE_DIR} \nlibraries: ${MAGNETTOOLS_LIBRARY}")
endif()
