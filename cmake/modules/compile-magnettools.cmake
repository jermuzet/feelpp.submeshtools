# Specify cmake variable "DMAGNETTOOLS_CXX_COMPILER" to change the compiler.

message(STATUS "---------- MAGNETTOOLS AUTOCONF BEGIN ----------")
execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/MagnetTools-compile)
set( AUTORECONF_FORCE $ENV{HIFIMAGNET_AUTORECONF_FORCE} )
# To force autoreconf, just export HIFHIMAGNET_AUTORECONF_FORCE=TRUE
if ( NOT EXISTS ${HIFIMAGNET_HOME_DIR}/MagnetTools/configure OR AUTORECONF_FORCE )
    message(STATUS "Autoreconf MagnetTools in ${HIFIMAGNET_HOME_DIR}/MagnetTools...")
    execute_process( COMMAND ${HIFIMAGNET_HOME_DIR}/MagnetTools/autogen.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/MagnetTools
        OUTPUT_FILE "MagnetTools-autoreconf")
endif()

if(NOT MAGNETTOOLS_CXX_COMPILER)
    set( MAGNETTOOLS_CXX_COMPILER ${CMAKE_CXX_COMPILER})
endif()
if( NOT MAGNETTOOLS_CXX_COMPILER STREQUAL CMAKE_CXX_COMPILER )
    message(STATUS "MagnetTools set to use different compiler from Hifimagnet.")
endif()
set(MAGNETTOOLS_CONF_OPTS "")

#--------------------------------------------------------------------------------
# BOOST
#--------------------------------------------------------------------------------
find_package(Boost REQUIRED)
if(BOOST_FOUND)
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-boost-lib=${BOOST_LIBRARY_DIRS}")
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-boost-include=${BOOST_INCLUDE_DIRS}")
endif()

#--------------------------------------------------------------------------------
# GSL
#--------------------------------------------------------------------------------
Find_Package(GSL REQUIRED)
if(GSL_FOUND)
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-gsl-lib=${GSL_LIBRARY_DIRS}")
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-gsl-include=${GSL_INCLUDE_DIRS}")
endif()

#--------------------------------------------------------------------------------
# FREESTEAM
#--------------------------------------------------------------------------------
Find_Package(freesteam REQUIRED)
if (FREESTEAM_FOUND)
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-freesteam-lib=${FREESTEAM_LIBRARY_DIRS}")
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-freesteam-include=${FREESTEAM_INCLUDE_DIRS}")
else()
    include(compile-freesteam)
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-freesteam=${CMAKE_BINARY_DIR}/Freesteam")
endif()

#--------------------------------------------------------------------------------
# POPT
#--------------------------------------------------------------------------------
Find_Package(popt REQUIRED)
if(popt_FOUND)
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-popt-lib=${POPT_LIBRARY_DIRS}")
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-popt-include=${POPT_INCLUDE_DIRS}")
endif()

#--------------------------------------------------------------------------------
# FADBAD++
#--------------------------------------------------------------------------------
if (FADBAD_DIR)
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-fadbad=${FADBAD_DIR}")
else()
    Find_Package(fadbad++)
    if ( NOT FADBAD_FOUND)
        MESSAGE(STATUS "Installing Fadbad++ in ${CMAKE_BINARY_DIR}/fadbad++")
        execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/fadbad++/include/fadbad++)
        execute_process(COMMAND cp ${CMAKE_SOURCE_DIR}/MagnetTools/contrib/FADBAD++/*.h  ${CMAKE_BINARY_DIR}/fadbad++/include/fadbad++)
        set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-fadbad=${CMAKE_BINARY_DIR}/fadbad++")
    endif()
endif()

#--------------------------------------------------------------------------------
# SHPEREPACK
#--------------------------------------------------------------------------------
if (SPHEREPACK_DIR)
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-spherepack=${SPHEREPACK_DIR}")
else()
    Find_Package(SpherePack)
endif()

#--------------------------------------------------------------------------------
# EXPOKIT
#--------------------------------------------------------------------------------
if (EXPOKIT_DIR)
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-expokit=${EXPOKIT_DIR}")
else()
    Find_Package(expokit)
endif()

#--------------------------------------------------------------------------------
# JSON_SPIRIT
#--------------------------------------------------------------------------------
if (JSON_SPIRIT_DIR)
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-json-spirit=${JSON_SPIRIT_DIR}")
else()
    Find_Package(jsonspirit)
    if ( NOT Json_Spirit_FOUND)
        MESSAGE(STATUS "Installing Json_spirit in ${CMAKE_BINARY_DIR}/json_spirit")
        execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/json_spirit)
        execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/json_spirit-compile)
        if(DEFINED ENV{BOOST_ROOT})
            execute_process(COMMAND cmake ${CMAKE_SOURCE_DIR}/MagnetTools/contrib/json-spirit-4.08 -DCMAKE_CXX_Compiler=${CMAKE_CXX_COMPILER} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/json_spirit -DBOOST_ROOT=${BOOST_ROOT}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/json_spirit-compile
                OUTPUT_FILE cmake.log
                ERROR_FILE cmake.log
                RESULT_VARIABLE ERROR_CODE
                )
        else()
            execute_process(COMMAND cmake ${CMAKE_SOURCE_DIR}/MagnetTools/contrib/json-spirit-4.08 -DCMAKE_CXX_Compiler=${CMAKE_CXX_COMPILER} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/json_spirit
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/json_spirit-compile
                OUTPUT_FILE cmake.log
                ERROR_FILE cmake.log
                RESULT_VARIABLE ERROR_CODE
                )
        endif()
        IF (ERROR_CODE)
            message(FATAL_ERROR "MagnetTools build freesteam failed see ${CMAKE_BINARY_DIR}/json_spirit-compile/cmake.log for details")
        ENDIF (ERROR_CODE)
        execute_process(COMMAND make WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/json_spirit-compile RESULT_VARIABLE ERROR_CODE)
        execute_process(COMMAND make install WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/json_spirit-compile RESULT_VARIABLE ERROR_CODE)
        set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-json-spirit=${CMAKE_BINARY_DIR}/json_spirit")
    endif()
endif()

#--------------------------------------------------------------------------------
# YAMLCPP
#--------------------------------------------------------------------------------
if (YAMLCPP_DIR)
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-yaml-cpp=${YAMLCPP_DIR}")
else()
    Find_Package(YamlCpp)
    if ( NOT YAMLCPP_FOUND)
        MESSAGE(FATAL_ERROR "YAMLCPP is mandatory - try apt-get installlibyaml-cpp-dev ")
    endif()
endif()

#--------------------------------------------------------------------------------
# SUNDIALS
#--------------------------------------------------------------------------------
if( SUNDIALS_DIR )
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-sundials=${SUNDIALS_DIR}")
else()
    Find_Package(SUNDIALS REQUIRED)
endif()

#--------------------------------------------------------------------------------
# MATHEVAL
#--------------------------------------------------------------------------------
if( MATHEVAL_DIR )
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--with-matheval=${MATHEVAL_DIR}")
else()
    Find_Package(MATHEVAL)
endif()

#--------------------------------------------------------------------------------
# GNUPLOT-IOSTREAM
#--------------------------------------------------------------------------------
Find_Package(GnuplotIostream)
if (GNUPLOT_IOSTREAM_INCLUDE_DIR)
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};--enable-gnuplot_iostream")
endif(GNUPLOT_IOSTREAM_INCLUDE_DIR)


#--------------------------------------------------------------------------------
# EIGEN3
#--------------------------------------------------------------------------------
if ( FEELPP_ENABLE_SYSTEM_EIGEN3 )
    find_package(Eigen3 REQUIRED)
    MESSAGE(STATUS "Eigen: ${EIGEN3_INCLUDE_DIR}")
    set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};EIGEN3_CFLAGS=-I${EIGEN3_INCLUDE_DIR}")
else ( FEELPP_ENABLE_SYSTEM_EIGEN3 )
    if (DEFINED ENV{FEELPP_HOME})
        set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};EIGEN3_CFLAGS=-I$ENV{FEELPP_HOME}/include/feelpp/eigen")
    else ()
        set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};EIGEN3_CFLAGS=-I${FEELPP_SOURCE_DIR}/contrib/eigen")
    endif ()
endif ( FEELPP_ENABLE_SYSTEM_EIGEN3 )

#--------------------------------------------------------------------------------
# MAGNETTOOLS

set(MAGNETTOOLS_CONF_OPTS "${MAGNETTOOLS_CONF_OPTS};CXX=${MAGNETTOOLS_CXX_COMPILER}")
set(MAGNETTOOLS_CONF_SCRIPT "${HIFIMAGNET_HOME_DIR}/MagnetTools/configure   --prefix=${CMAKE_BINARY_DIR}/MagnetTools ${MAGNETTOOLS_CONF_OPTS}")

# generate ${HIFIMAGNET_HOME_DIR}/MagnetTools/configure
if(NOT EXISTS "${HIFIMAGNET_HOME_DIR}/MagnetTools/configure")
    message(STATUS "Generating ${HIFIMAGNET_HOME_DIR}/MagnetTools/configure")
    execute_process(
        COMMAND ${HIFIMAGNET_HOME_DIR}/MagnetTools/autogen.sh
        WORKING_DIRECTORY ${HIFIMAGNET_HOME_DIR}/MagnetTools
        OUTPUT_FILE autogen.log
        ERROR_FILE auotgen.log
        RESULT_VARIABLE ERROR_CODE
        )
    IF (ERROR_CODE)
        message(FATAL_ERROR "MagnetTools failed to create ${HIFIMAGNET_HOME_DIR}/MagnetTools/configure see ${HIFIMAGNET_HOME_DIR}/MagnetTools/${ERROR_FILE} for details")
    ENDIF (ERROR_CODE)
endif()

if(${HIFIMAGNET_HOME_DIR}/MagnetTools/configure IS_NEWER_THAN ${CMAKE_BINARY_DIR}/MagnetTools-compile/Makefile)
    message(STATUS "Configuring MagnetTools in ${CMAKE_BINARY_DIR}/MagnetTools-compile...")
    if (FEELPP_USE_STATIC_LINKAGE )
        set(MAGNETTOOLS_CONF_OPTS "--disable-shared;--enable-static;${MAGNETTOOLS_CONF_OPTS}")
    endif()

    message(STATUS "Configure ${MAGNETTOOLS_CONF_SCRIPT}")#  OUTPUT_FILE "MagnetTools-configure" (add --datadir= --pythondir= )
    execute_process(
        COMMAND ${HIFIMAGNET_HOME_DIR}/MagnetTools/configure  --prefix=${CMAKE_BINARY_DIR}/MagnetTools ${MAGNETTOOLS_CONF_OPTS}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/MagnetTools-compile
        OUTPUT_FILE configure.log
        ERROR_FILE configure.log
        RESULT_VARIABLE ERROR_CODE
        )
    IF (ERROR_CODE)
        message(FATAL_ERROR "MagnetTools configure failed see ${CMAKE_BINARY_DIR}/MagnetTools-compile/configure.log for details")
    ENDIF (ERROR_CODE)
endif()

if(${CMAKE_BINARY_DIR}/MagnetTools-compile/Makefile IS_NEWER_THAN ${CMAKE_SOURCE_DIR}/MagnetTools/Makefile.am)
    message(STATUS "Building MagnetTools in ${CMAKE_BINARY_DIR}/MagnetTools-compile (this may take a while)...")
    execute_process(
        COMMAND make -j${NProcs2}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/MagnetTools-compile
        OUTPUT_FILE build.log
        ERROR_FILE build.log
        RESULT_VARIABLE ERROR_CODE
        )
    IF (ERROR_CODE)
        message(FATAL_ERROR "MagnetTools build failed see ${CMAKE_BINARY_DIR}/MagnetTools-compile/build.log for details")
    ENDIF (ERROR_CODE)

    message(STATUS "Installing MagnetTools in ${CMAKE_BINARY_DIR}/MagnetTools...")
    execute_process(
        COMMAND make install
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/MagnetTools-compile
        OUTPUT_FILE install.log
        ERROR_FILE install.log
        RESULT_VARIABLE ERROR_CODE
        )
    IF (ERROR_CODE)
        message(FATAL_ERROR "MagnetTools install failed see ${CMAKE_BINARY_DIR}/MagnetTools-compile/install.log for details")
    ENDIF (ERROR_CODE)
endif()
message(STATUS "---------- MAGNETTOOLS AUTOCONF END ----------")
