# This module looks for SIMPLECPU module and determines where the header files
# and libraries are.

#=============================================================================
# Copyright 2014 GreenSocs
#
# KONRAD Frederic <fred.konrad@greensocs.com>
#
#=============================================================================

MESSAGE(STATUS "Searching for SimpleCPU model.")

# The HINTS option should only be used for values computed from the system.
SET(_SIMPLECPU_HINTS
  ${SIMPLECPU_PREFIX}/include
  ${SIMPLECPU_PREFIX}/lib
  ${SIMPLECPU_PREFIX}/lib64
  $ENV{SIMPLECPU_PREFIX}/include
  $ENV{SIMPLECPU_PREFIX}/lib
  $ENV{SIMPLECPU_PREFIX}/lib64
  ${CMAKE_INSTALL_PREFIX}/include
  ${CMAKE_INSTALL_PREFIX}/lib
  ${CMAKE_INSTALL_PREFIX}/lib64
  )
# Hard-coded guesses should still go in PATHS. This ensures that the user
# environment can always override hard guesses.
SET(_SIMPLECPU_PATHS
  /usr/include
  /usr/lib
  /usr/lib64
  /usr/local/lib
  /usr/local/lib64
  )

FIND_PATH(SIMPLECPU_INCLUDE_DIRS
  NAMES SimpleCPU/simpleCPU.h
  HINTS ${_SIMPLECPU_HINTS}
  PATHS ${_SIMPLECPU_PATHS}
)

FIND_PATH(SIMPLECPU_LIBRARY_DIRS
  NAMES libsimplecpu.so
  HINTS ${_SIMPLECPU_HINTS}
  PATHS ${_SIMPLECPU_PATHS}
)

if("${SIMPLECPU_INCLUDE_DIRS}" MATCHES "SIMPLECPU_INCLUDE_DIRS-NOTFOUND")
    SET(SIMPLECPU_FOUND FALSE)
else("${SIMPLECPU_INCLUDE_DIRS}" MATCHES "SIMPLECPU_INCLUDE_DIRS-NOTFOUND")
    SET(SIMPLECPU_FOUND TRUE)
    MESSAGE(STATUS "SimpleCPU include directory = ${SIMPLECPU_INCLUDE_DIRS}")
    MESSAGE(STATUS "SimpleCPU library directory = ${SIMPLECPU_LIBRARY_DIRS}")
    SET(SIMPLECPU_LIBRARIES simplecpu)
endif("${SIMPLECPU_INCLUDE_DIRS}" MATCHES "SIMPLECPU_INCLUDE_DIRS-NOTFOUND")

