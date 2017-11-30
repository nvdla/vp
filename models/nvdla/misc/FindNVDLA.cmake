# This module looks for NVDLA module and determines where the header files and
# libraries are.

MESSAGE(STATUS "Searching for NVDLA model.")

# The HINTS option should only be used for values computed from the system.
SET(_NVDLA_HINTS
  ${NVDLA_PREFIX}/include
  ${NVDLA_PREFIX}/lib
  ${NVDLA_PREFIX}/lib64
  $ENV{NVDLA_PREFIX}/include
  $ENV{NVDLA_PREFIX}/lib
  $ENV{NVDLA_PREFIX}/lib64
  ${CMAKE_INSTALL_PREFIX}/include
  ${CMAKE_INSTALL_PREFIX}/lib
  ${CMAKE_INSTALL_PREFIX}/lib64
  )
# Hard-coded guesses should still go in PATHS. This ensures that the user
# environment can always override hard guesses.
SET(_NVDLA_PATHS
  /usr/include
  /usr/lib
  /usr/lib64
  /usr/local/lib
  /usr/local/lib64
  )

FIND_PATH(NVDLA_INCLUDE_DIRS
  NAMES NVDLA/NVDLA.h
  HINTS ${_NVDLA_HINTS}
  PATHS ${_NVDLA_PATHS}
)

FIND_PATH(NVDLA_LIBRARY_DIRS
  NAMES libnvdla.so
  HINTS ${_NVDLA_HINTS}
  PATHS ${_NVDLA_PATHS}
)

if("${NVDLA_INCLUDE_DIRS}" MATCHES "NVDLA_INCLUDE_DIRS-NOTFOUND")
    SET(NVDLA_FOUND FALSE)
else("${NVDLA_INCLUDE_DIRS}" MATCHES "NVDLA_INCLUDE_DIRS-NOTFOUND")
    SET(NVDLA_FOUND TRUE)
    MESSAGE(STATUS "NVDLA include directory = ${NVDLA_INCLUDE_DIRS}")
    MESSAGE(STATUS "NVDLA library directory = ${NVDLA_LIBRARY_DIRS}")
    SET(NVDLA_LIBRARIES nvdla)
endif("${NVDLA_INCLUDE_DIRS}" MATCHES "NVDLA_INCLUDE_DIRS-NOTFOUND")

