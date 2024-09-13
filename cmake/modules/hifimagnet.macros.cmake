# - Specific macros for Hifimagnet project

INCLUDE(ParseArguments)

#
# Add macro for applications testsuite
macro(hifimagnet_add_test)
  PARSE_ARGUMENTS(FEELPP_TEST
    "EXEC;CFG;GEO;LABEL"
    "MPI;NO_TEST;EXCLUDE_FROM_ALL"
    ${ARGN}
    )
  CAR(FEELPP_TEST_NAME ${FEELPP_TEST_DEFAULT_ARGS})

  if ( NOT FEELPP_TEST_EXEC )
    set(execname feelpp_${FEELPP_TEST_NAME})
  else()
    set(excname feelpp_${FEELPP_TEST_EXEC})
  endif()

  if ( FEELPP_TEST_CFG )
    if ( FEELPP_TEST_MPI AND NProcs2 GREATER 1 )
      add_test(
        NAME test_${FEELPP_TEST_NAME}-np-${NProcs2}
        COMMAND  ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${NProcs2} ${MPIEXEC_PREFLAGS} ${CMAKE_CURRENT_BINARY_DIR}/${execname} --config-file=${FEELPP_TEST_CFG} --nochdir --exporter.export=0 --log_level=message
        )
    else(FEELPP_TEST_MPI AND NProcs2 GREATER 1)
      add_test(
        NAME test_${FEELPP_TEST_NAME}
        COMMAND ${execname} --config-file=${FEELPP_TEST_CFG} --nochdir --exporter.export=0 --log_level=message
        )
    endif(FEELPP_TEST_MPI AND NProcs2 GREATER 1)
  else(FEELPP_TEST_CFG)
    if ( FEELPP_TEST_MPI AND NProcs2 GREATER 1 )
      add_test(
        NAME test_${FEELPP_TEST_NAME}-np-${NProcs2}
        COMMAND  ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${NProcs2} ${MPIEXEC_PREFLAGS} ${CMAKE_CURRENT_BINARY_DIR}/${execname} --nochdir --exporter.export=0 --log_level=message
        )
    else(FEELPP_TEST_MPI AND NProcs2 GREATER 1)
      add_test(
        NAME test_${FEELPP_TEST_NAME}
        COMMAND ${execname} --nochdir --exporter.export=0 --log_level=message
        )
    endif(FEELPP_TEST_MPI AND NProcs2 GREATER 1)
  endif(FEELPP_TEST_CFG)

endmacro(hifimagnet_add_test)
