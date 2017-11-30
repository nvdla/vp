# - Find TLM
# This module finds if TLM is installed and determines where the
# include files and libraries are. This code sets the following
# variables: (from kernel/sc_ver.h)
#
#  TLM_VERSION_STRING     = Version of the package found, eg. "2.2.0"
#  TLM_VERSION_MAJOR      = The major version of the package found.
#  TLM_VERSION_MINOR      = The minor version of the package found.
#  TLM_VERSION_PATCH      = The patch version of the package found.
#  TLM_VERSION_DATE       = The date of release (from TLM_VERSION)
#  TLM_VERSION            = This is set to: $major.$minor.$patch
#
# The minimum required version of TLM can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(TLM 2.2)
#
# For these components the following variables are set:
#
#  TLM_FOUND                    - TRUE if all components are found.
#  TLM_INCLUDE_DIRS             - Full paths to all include dirs.
#  TLM_LIBRARIES                - Full paths to all libraries.
#  TLM_<component>_FOUND        - TRUE if <component> is found.
#
# Example Usages:
#  FIND_PACKAGE(TLM)
#  FIND_PACKAGE(TLM 2.3)
#

#=============================================================================
# Copyright 2012 GreenSocs
#
#=============================================================================

message(STATUS "Searching for TLM")

# The HINTS option should only be used for values computed from the system.
SET(_TLM_HINTS
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\TLM\\2.2;TLMHome]/include"
  $ENV{TLM_HOME}
  ${SYSTEMC_PREFIX}/include
  ${SYSTEMC_PREFIX}/lib
  ${SYSTEMC_PREFIX}/lib-linux
  ${SYSTEMC_PREFIX}/lib-linux64
  ${SYSTEMC_PREFIX}/lib-macos
  $ENV{SYSTEMC_PREFIX}/include
  $ENV{SYSTEMC_PREFIX}/lib
  $ENV{SYSTEMC_PREFIX}/lib-linux
  $ENV{SYSTEMC_PREFIX}/lib-linux64
  $ENV{SYSTEMC_PREFIX}/lib-macos
  ${CMAKE_INSTALL_PREFIX}/include
  ${CMAKE_INSTALL_PREFIX}/lib
  ${CMAKE_INSTALL_PREFIX}/lib-linux
  ${CMAKE_INSTALL_PREFIX}/lib-linux64
  ${CMAKE_INSTALL_PREFIX}/lib-macos
  )
# Hard-coded guesses should still go in PATHS. This ensures that the user
# environment can always override hard guesses.
SET(_TLM_PATHS
  /usr/include/TLM
  /usr/include/systemc
  /usr/lib
  /usr/lib-linux
  /usr/lib-linux64
  /usr/lib-macos
  /usr/local/lib
  /usr/local/lib-linux
  /usr/local/lib-linux64
  /usr/local/lib-macos
  )

FIND_PATH(TLM_INCLUDE_DIRS
  NAMES tlm.h
  HINTS ${_TLM_HINTS}
  PATHS ${_TLM_PATHS}
  PATH_SUFFIXES include/tlm
)

EXEC_PROGRAM("ls ${TLM_INCLUDE_DIRS}/tlm.h"
             RETURN_VALUE ret)

if("${ret}" MATCHES "0")
  set(TLM_FOUND TRUE)
endif("${ret}" MATCHES "0")

message(STATUS "TLM library = ${TLM_INCLUDE_DIRS}/tlm.h")

