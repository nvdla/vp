# - Find AWSSDK
# This module finds if AWS SDK is built and determines where the
# include files and libraries are.
#

MESSAGE(STATUS "Searching for AWS SDK")

# The HINTS option should only be used for values computed from the system.
SET(_AWSSDK_HINTS
    ${AWS_SDK_PREFIX}/userspace/include
    ${AWS_SDK_PREFIX}/userspace/lib/so
    $ENV{AWS_SDK_PREFIX}/userspace/include
    $ENV{AWS_SDK_PREFIX}/userspace/lib/so
    $ENV{SDK_DIR}/userspace/include
    $ENV{SDK_DIR}/userspace/lib/so
    ${CMAKE_INSTALL_PREFIX}/include
    ${CMAKE_INSTALL_PREFIX}/lib
    ${CMAKE_INSTALL_PREFIX}/lib-linux
    ${CMAKE_INSTALL_PREFIX}/lib-linux64
    ${CMAKE_INSTALL_PREFIX}/lib-macos
)

# Hard-coded guesses should still go in PATHS. This ensures that the user
# environment can always override hard guesses.
SET(_AWSSDK_PATHS
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

FIND_PATH(AWSSDK_INCLUDE_DIRS
  NAMES fpga_pci.h
  HINTS ${_AWSSDK_HINTS}
  PATHS ${_AWSSDK_PATHS}
)

find_library(AWSSDK_LIBRARIES
  NAMES fpga_mgmt
  HINTS ${_AWSSDK_HINTS}
  PATHS ${_AWSSDK_PATHS}
) 

FIND_PATH(AWSSDK_LIBRARY_DIRS
  NAMES ${CMAKE_SHARED_LIBRARY_PREFIX}fpga_mgmt${CMAKE_SHARED_LIBRARY_SUFFIX}
  HINTS ${_AWSSDK_HINTS}
  PATHS ${_AWSSDK_PATHS}
)

if("${AWSSDK_INCLUDE_DIRS}" MATCHES "AWSSDK_INCLUDE_DIRS-NOTFOUND")
    SET(AWSSDK_FOUND FALSE)
else("${AWSSDK_INCLUDE_DIRS}" MATCHES "AWSSDK_INCLUDE_DIRS-NOTFOUND")
    SET(AWSSDK_FOUND TRUE)
    MESSAGE(STATUS "AWS SDK include directory = ${AWSSDK_INCLUDE_DIRS}")
    MESSAGE(STATUS "AWS SDK library directory = ${AWSSDK_LIBRARY_DIRS}")
    MESSAGE(STATUS "AWS SDK library = ${AWSSDK_LIBRARIES}")
endif("${AWSSDK_INCLUDE_DIRS}" MATCHES "AWSSDK_INCLUDE_DIRS-NOTFOUND")
