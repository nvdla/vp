# - Find GreenLib
# This module finds if GreenLib is installed and determines where the
# include files and libraries are.
#

#=============================================================================
# Copyright 2014 GreenSocs
#
# KONRAD Frederic <fred.konrad@greensocs.com>
#=============================================================================

MESSAGE(STATUS "Searching for GreenLib")

# The HINTS option should only be used for values computed from the system.
SET(_GREENLIB_HINTS
    ${GREENLIB_PREFIX}/include
    ${GREENLIB_PREFIX}/lib
    ${GREENLIB_PREFIX}/lib-linux
    ${GREENLIB_PREFIX}/lib-linux64
    ${GREENLIB_PREFIX}/lib-macos
    $ENV{GREENLIB_PREFIX}/include
    $ENV{GREENLIB_PREFIX}/lib
    $ENV{GREENLIB_PREFIX}/lib-linux
    $ENV{GREENLIB_PREFIX}/lib-linux64
    $ENV{GREENLIB_PREFIX}/lib-macos
    ${CMAKE_INSTALL_PREFIX}/include
    ${CMAKE_INSTALL_PREFIX}/lib
    ${CMAKE_INSTALL_PREFIX}/lib-linux
    ${CMAKE_INSTALL_PREFIX}/lib-linux64
    ${CMAKE_INSTALL_PREFIX}/lib-macos
)

# Hard-coded guesses should still go in PATHS. This ensures that the user
# environment can always override hard guesses.
SET(_GREENLIB_PATHS
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

FIND_PATH(GREENLIB_INCLUDE_DIRS
    NAMES greencontrol/config.h
    HINTS ${_GREENLIB_HINTS}
    PATHS ${_GREENLIB_PATHS}
)

FIND_PATH(GREENLIB_LIBRARY_DIRS
    NAMES libgreenreg.a
    HINTS ${_GREENLIB_HINTS}
    PATHS ${_GREENLIB_PATHS}
)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

if("${GREENLIB_INCLUDE_DIRS}" MATCHES "GREENLIB_INCLUDE_DIRS-NOTFOUND")
    SET(GREENLIB_FOUND FALSE)
else("${GREENLIB_INCLUDE_DIRS}" MATCHES "GREENLIB_INCLUDE_DIRS-NOTFOUND")
    SET(GREENLIB_FOUND TRUE)
    MESSAGE(STATUS "GreenLib include directory = ${GREENLIB_INCLUDE_DIRS}")
    MESSAGE(STATUS "GreenLib library directory = ${GREENLIB_LIBRARY_DIRS}")
    SET(GREENLIB_LIBRARIES greenreg)
endif("${GREENLIB_INCLUDE_DIRS}" MATCHES "GREENLIB_INCLUDE_DIRS-NOTFOUND")

