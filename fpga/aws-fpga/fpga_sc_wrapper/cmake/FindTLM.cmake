# Find TLM
# This module finds if TLM is installed and determines where the
# include files and libraries are. This code sets the following
# variables: (from tlm_version.h)
#
#  TLM_VERSION_MAJOR      = The major version of the package found.
#  TLM_VERSION_MINOR      = The minor version of the package found.
#  TLM_VERSION_PATCH      = The patch version of the package found.
#  TLM_VERSION            = The full version string of the package found.
#
# The minimum required version of TLM can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(TLM 2.2)
#
# For these components the following variables are set:
#
#  TLM_FOUND                    - TRUE if all components are found.
#  TLM_INCLUDE_DIRS             - Full paths to all include dirs.
#
# Example Usages:
#  FIND_PACKAGE(TLM)

#=============================================================================
# Copyright 2015 GreenSocs
#=============================================================================

set(_TLM_HINTS
    ${TLM_PREFIX}
    ${SYSTEMC_PREFIX}/include
    $ENV{TLM_HOME}
    $ENV{SYSTEMC_HOME}/include)

set(_TLM_PATHS
    /usr/local/include/tlm
    /usr/local/include/systemc
    /usr/include/tlm
    /usr/include/systemc)

find_file(_TLM_VERSION_FILE
          NAMES tlm_version.h
          HINTS ${_TLM_HINTS}
          PATHS ${_TLM_PATHS}
          PATH_SUFFIXES tlm_h
                        tlm/tlm_h
                        tlm_core/tlm_2)

if(EXISTS ${_TLM_VERSION_FILE})
    file (READ ${_TLM_VERSION_FILE} _TLM_VERSION_FILE_CONTENTS)

    string (REGEX MATCH "TLM_VERSION_MAJOR[ \t]+([0-9]+)" 
            TLM_VERSION_MAJOR ${_TLM_VERSION_FILE_CONTENTS})       
    string (REGEX MATCH "([0-9]+)" TLM_VERSION_MAJOR 
            ${TLM_VERSION_MAJOR})       
    string (REGEX MATCH "TLM_VERSION_MINOR[ \t]+([0-9]+)" 
            TLM_VERSION_MINOR ${_TLM_VERSION_FILE_CONTENTS})
    string (REGEX MATCH "([0-9]+)" TLM_VERSION_MINOR 
            ${TLM_VERSION_MINOR})
    string (REGEX MATCH "TLM_VERSION_PATCH[ \t]+([0-9]+)" 
            TLM_VERSION_PATCH ${_TLM_VERSION_FILE_CONTENTS})
    string (REGEX MATCH "([0-9]+)" TLM_VERSION_PATCH 
            ${TLM_VERSION_PATCH})

    set(TLM_VERSION "${TLM_VERSION_MAJOR}.${TLM_VERSION_MINOR}.${TLM_VERSION_PATCH}")
    
    find_path(TLM_INCLUDE_DIRS
              NAMES tlm tlm.h
              HINTS ${_TLM_HINTS}
              PATHS ${_TLM_PATHS}
              PATH_SUFFIXES tlm)

endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TLM
                                  REQUIRED_VARS TLM_INCLUDE_DIRS
                                  VERSION_VAR TLM_VERSION)