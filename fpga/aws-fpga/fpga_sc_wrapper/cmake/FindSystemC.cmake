# Find SystemC
# This module finds if SystemC is installed and determines where the
# include files and libraries are. This code sets the following
# variables: (from kernel/sc_ver.h)
#
#  SystemC_VERSION_MAJOR      = The major version of the package found
#  SystemC_VERSION_MINOR      = The minor version of the package found
#  SystemC_VERSION_PATCH      = The patch version of the package found
#  SystemC_VERSION            = The full version string of the package found
#
# For these components the following variables are set:
#
#  SystemC_INCLUDE_DIRS             - Full paths to all include dirs
#  SystemC_LIBRARY_DIRS             - Link directories for SystemC libraries
#  SystemC::SystemC                 - Full path to SystemC library
#  SystemC::SystemC_STATIC          - Full path to SystemC static library
#
# Example Usages:
#  find_package(SystemC)

#=============================================================================
# Copyright 2015 GreenSocs
#=============================================================================

function(SYSTEMC_APPEND_TARGET var prefix)
    set(listVar "")
    foreach(f ${ARGN})
        list(APPEND listVar "${prefix}${f}")
    endforeach(f)
    set(${var} "${listVar}" PARENT_SCOPE)
endfunction(SYSTEMC_APPEND_TARGET)

set(_SYSTEMC_TARGET_SUFFIXES linux
                             linux64
                             bsd
                             bsd64
                             macosx
                             macosx64
                             macosxppc
                             macosxppc64
                             mingw
                             mingw64
                             gccsparcOS5
                             sparcOS5)

SYSTEMC_APPEND_TARGET(_SYSTEMC_PREFIX_SUFFIXES ${SYSTEMC_PREFIX}/lib- ${_SYSTEMC_TARGET_SUFFIXES})
SYSTEMC_APPEND_TARGET(_SYSTEMC_HOME_SUFFIXES $ENV{SYSTEMC_HOME}/lib- ${_SYSTEMC_TARGET_SUFFIXES})
SYSTEMC_APPEND_TARGET(_USR_SUFFIXES /usr/lib- ${_SYSTEMC_TARGET_SUFFIXES})
SYSTEMC_APPEND_TARGET(_USR_LOCAL_SUFFIXES /usr/local/lib- ${_SYSTEMC_TARGET_SUFFIXES})

set(_SYSTEMC_HINTS
    ${SYSTEMC_PREFIX}/include
    ${SYSTEMC_PREFIX}/lib
    ${_SYSTEMC_PREFIX_SUFFIXES}
    $ENV{SYSTEMC_HOME}/lib
    $ENV{SYSTEMC_HOME}/include
    ${_SYSTEMC_HOME_SUFFIXES})

set(_SYSTEMC_PATHS
    /usr/local/lib
    ${_USR_LOCAL_SUFFIXES}
    /usr/local/lib64
    /usr/local/include
    /usr/local/systemc
    /usr/local/systemc/include
    /usr/lib
    ${_USR_SUFFIXES}
    /usr/lib64
    /usr/include
    /usr/systemc
    /usr/systemc/include)

find_file(_SYSTEMC_VERSION_FILE
          NAMES sc_ver.h
          HINTS ${_SYSTEMC_HINTS}
          PATHS ${_SYSTEMC_PATHS}
          PATH_SUFFIXES sysc/kernel)

if(EXISTS ${_SYSTEMC_VERSION_FILE})
    file (READ ${_SYSTEMC_VERSION_FILE} _SYSTEMC_VERSION_FILE_CONTENTS)
    string (REGEX MATCH
        "SC_API_VERSION_STRING[ \t]+sc_api_version_([0-9]+)_([0-9]+)_([0-9]+)"
        SC_API_VERSION_STRING ${_SYSTEMC_VERSION_FILE_CONTENTS})

    if(NOT "${SC_API_VERSION_STRING}" MATCHES "")
    	# SystemC < 2.3.1
    	string (REGEX MATCHALL "([0-9]+)" _SystemC_VERSION
            ${SC_API_VERSION_STRING})
    	list(GET _SystemC_VERSION 0 SystemC_VERSION_MAJOR)
    	list(GET _SystemC_VERSION 1 SystemC_VERSION_MINOR)
    	list(GET _SystemC_VERSION 2 SystemC_VERSION_PATCH)
    else()
    	# SystemC >= 2.3.1
    	string (REGEX MATCH "SC_VERSION_MAJOR[ \t]+([0-9]+)"
            SystemC_VERSION_MAJOR ${_SYSTEMC_VERSION_FILE_CONTENTS})
    	string (REGEX MATCH "([0-9]+)" SystemC_VERSION_MAJOR
            ${SystemC_VERSION_MAJOR})
    	string (REGEX MATCH "SC_VERSION_MINOR[ \t]+([0-9]+)"
            SystemC_VERSION_MINOR ${_SYSTEMC_VERSION_FILE_CONTENTS})
    	string (REGEX MATCH "([0-9]+)" SystemC_VERSION_MINOR
            ${SystemC_VERSION_MINOR})
    	string (REGEX MATCH "SC_VERSION_PATCH[ \t]+([0-9]+)"
            SystemC_VERSION_PATCH ${_SYSTEMC_VERSION_FILE_CONTENTS})
    	string (REGEX MATCH "([0-9]+)" SystemC_VERSION_PATCH
            ${SystemC_VERSION_PATCH})
    endif()

    set(SystemC_VERSION "${SystemC_VERSION_MAJOR}.${SystemC_VERSION_MINOR}.${SystemC_VERSION_PATCH}")

    # Compatibility variable
    set(SystemC_VERSION_STRING ${SystemC_VERSION})

    find_path(SystemC_INCLUDE_DIRS
              NAMES systemc systemc.h
              HINTS ${_SYSTEMC_HINTS}
              PATHS ${_SYSTEMC_PATHS})

    # Static
    set(_SystemC_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    if(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
    endif()

    find_library(SystemC_LIBRARIES_STATIC
                 NAMES systemc SystemC
                 HINTS ${_SYSTEMC_HINTS}
                 PATHS ${_SYSTEMC_PATHS})

    # Restore original CMAKE_FIND_LIBRARY_SUFFIXES
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_SystemC_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})

    # If SystemC_USE_STATIC_LIBS set to ON, force the use of the static libraries
    if(SystemC_USE_STATIC_LIBS)
        set(SystemC_LIBRARIES SystemC_LIBRARIES_STATIC)
    else()
        find_library(SystemC_LIBRARIES
                     NAMES systemc SystemC
                     HINTS ${_SYSTEMC_HINTS}
                     PATHS ${_SYSTEMC_PATHS})
    endif()

    # Default
    if(NOT TARGET SystemC::SystemC)
        add_library(SystemC::SystemC UNKNOWN IMPORTED)
        set_target_properties(SystemC::SystemC PROPERTIES
                              IMPORTED_LOCATION "${SystemC_LIBRARIES}")
    endif()

    # Static
    if(NOT TARGET SystemC::SystemC_STATIC)
        add_library(SystemC::SystemC_STATIC STATIC IMPORTED)
        set_target_properties(SystemC::SystemC_STATIC PROPERTIES
                              IMPORTED_LOCATION "${SystemC_LIBRARIES_STATIC}")
    endif()

    if("${CMAKE_VERSION}" VERSION_GREATER 2.8.12)
        get_filename_component(SystemC_LIBRARY_DIRS ${SystemC_LIBRARIES}
            DIRECTORY)
    else("${CMAKE_VERSION}" VERSION_GREATER 2.8.12)
        get_filename_component(SystemC_LIBRARY_DIRS ${SystemC_LIBRARIES}
            PATH)
    endif()
endif()

include(FindPackageHandleStandardArgs)
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.10)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(SystemC
                                      FOUND_VAR SystemC_FOUND
                                      REQUIRED_VARS SystemC_LIBRARIES SystemC_LIBRARY_DIRS SystemC_INCLUDE_DIRS
                                      VERSION_VAR SystemC_VERSION)
else()
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(SystemC
                                      REQUIRED_VARS SystemC_LIBRARIES SystemC_LIBRARY_DIRS SystemC_INCLUDE_DIRS
                                      VERSION_VAR SystemC_VERSION)
    set(SystemC_FOUND ${SYSTEMC_FOUND})
endif()
mark_as_advanced(SystemC_LIBRARIES SystemC_LIBRARY_DIRS SystemC_INCLUDE_DIRS)
