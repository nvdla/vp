# - Find NvdlaCmod
# This module finds if NVDLA CMOD is built and determines where the
# include files and libraries are.
#

MESSAGE(STATUS "Searching for NVDLA CMOD")

# The HINTS option should only be used for values computed from the system.
SET(_NVDLA_CMOD_HINTS
    ${NVDLA_HW_PREFIX}/outdir/${NVDLA_HW_PROJECT}/cmod/release/include
    ${NVDLA_HW_PREFIX}/outdir/${NVDLA_HW_PROJECT}/cmod/release/lib
    ${NVDLA_HW_PREFIX}/outdir/${NVDLA_HW_PROJECT}/cmod/release/lib-linux
    ${NVDLA_HW_PREFIX}/outdir/${NVDLA_HW_PROJECT}/cmod/release/lib-linux64
    ${NVDLA_HW_PREFIX}/outdir/${NVDLA_HW_PROJECT}/cmod/release/lib-macos
    $ENV{NVDLA_HW_PREFIX}/outdir/$ENV{NVDLA_HW_PROJECT}/cmod/release/include
    $ENV{NVDLA_HW_PREFIX}/outdir/$ENV{NVDLA_HW_PROJECT}/cmod/release/lib
    $ENV{NVDLA_HW_PREFIX}/outdir/$ENV{NVDLA_HW_PROJECT}/cmod/release/lib-linux
    $ENV{NVDLA_HW_PREFIX}/outdir/$ENV{NVDLA_HW_PROJECT}/cmod/release/lib-linux64
    $ENV{NVDLA_HW_PREFIX}/outdir/$ENV{NVDLA_HW_PROJECT}/cmod/release/lib-macos
    ${CMAKE_INSTALL_PREFIX}/include
    ${CMAKE_INSTALL_PREFIX}/lib
    ${CMAKE_INSTALL_PREFIX}/lib-linux
    ${CMAKE_INSTALL_PREFIX}/lib-linux64
    ${CMAKE_INSTALL_PREFIX}/lib-macos
)

# Hard-coded guesses should still go in PATHS. This ensures that the user
# environment can always override hard guesses.
SET(_NVDLA_CMOD_PATHS
    /usr/include
    /usr/lib
    /usr/lib-linux
    /usr/lib-linux64
    /usr/lib-macos
    /usr/local/lib
    /usr/local/lib-linux
    /usr/local/lib-linux64
    /usr/local/lib-macos
)

FIND_FILE(NVDLA_HW_VERSION_FILE
  NAMES VERSION
  HINTS ${_NVDLA_CMOD_HINTS}
  PATHS ${_NVDLA_CMOD_PATHS}
)

FILE(READ ${NVDLA_HW_VERSION_FILE} NVDLA_HW_VERSION)

FIND_PATH(NVDLA_CMOD_INCLUDE_DIR
  NAMES NV_nvdla.h
  HINTS ${_NVDLA_CMOD_HINTS}
  PATHS ${_NVDLA_CMOD_PATHS}
)

find_library(NVDLA_CMOD_LIBRARY
  NAMES nvdla_cmod
  HINTS ${_NVDLA_CMOD_HINTS}
  PATHS ${_NVDLA_CMOD_PATHS}
) 

FIND_PATH(NVDLA_CMOD_LIBRARY_DIR
  NAMES ${CMAKE_SHARED_LIBRARY_PREFIX}nvdla_cmod${CMAKE_SHARED_LIBRARY_SUFFIX}
  HINTS ${_NVDLA_CMOD_HINTS}
  PATHS ${_NVDLA_CMOD_PATHS}
)

if("${NVDLA_CMOD_INCLUDE_DIR}" MATCHES "NVDLA_CMOD_INCLUDE_DIR-NOTFOUND")
    SET(NVDLA_CMOD_FOUND FALSE)
else("${NVDLA_CMOD_INCLUDE_DIR}" MATCHES "NVDLA_CMOD_INCLUDE_DIR-NOTFOUND")
    SET(NVDLA_CMOD_FOUND TRUE)
    MESSAGE(STATUS "NVDLA HW VERSION = ${NVDLA_HW_VERSION}")
    MESSAGE(STATUS "NVDLA CMOD include directory = ${NVDLA_CMOD_INCLUDE_DIR}")
    MESSAGE(STATUS "NVDLA CMOD library directory = ${NVDLA_CMOD_LIBRARY_DIR}")
    MESSAGE(STATUS "NVDLA CMOD library = ${NVDLA_CMOD_LIBRARY}")
endif("${NVDLA_CMOD_INCLUDE_DIR}" MATCHES "NVDLA_CMOD_INCLUDE_DIR-NOTFOUND")
