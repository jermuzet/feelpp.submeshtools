MESSAGE(STATUS "Installing Freesteam in ${CMAKE_BINARY_DIR}/freesteam")
# May require GSL_LIBDIR=$GSL_DIR/lib, GSL_INCDIR=$GSL_DIR/include
execute_process(
    COMMAND scons  python INSTALL_PREFIX=${CMAKE_BINARY_DIR}/freesteam INSTALL_PYTHON=$MODULE/${CMAKE_BINARY_DIR}/freesteam
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/MagnetTools/contrib/freesteam-2.2
    OUTPUT_FILE build.log
    ERROR_FILE build.log
    RESULT_VARIABLE ERROR_CODE
    )
IF (ERROR_CODE)
    message(FATAL_ERROR "MagnetTools build freesteam failed see ${CMAKE_BINARY_DIR}/freesteam-compile/build.log for details")
ENDIF (ERROR_CODE)
