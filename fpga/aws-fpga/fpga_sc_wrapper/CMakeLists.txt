# Copyright (C) 2014, GreenSocs ltd.
#
# CMakeLists.txt
#
# KONRAD Frederic <fred.konrad@greensocs.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses>.
#
# Linking GreenSocs code, statically or dynamically with other modules
# is making a combined work based on GreenSocs code. Thus, the terms and
# conditions of the GNU General Public License cover the whole
# combination.
#
# In addition, as a special exception, the copyright holders, GreenSocs
# Ltd, give you permission to combine GreenSocs code with free software
# programs or libraries that are released under the GNU LGPL, under the
# OSCI license, under the OCP TLM Kit Research License Agreement or
# under the OVP evaluation license.You may copy and distribute such a
# system following the terms of the GNU GPL and the licenses of the
# other code concerned.
#
# Note that people who make modified versions of GreenSocs code are not
# obligated to grant this special exception for their modified versions;
# it is their choice whether to do so. The GNU General Public License
# gives permission to release a modified version without this exception;
# this exception also makes it possible to release a modified version
# which carries forward this exception.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(NVDLA)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

FIND_PACKAGE(SystemC)
if(SystemC_FOUND)
    INCLUDE_DIRECTORIES(${SystemC_INCLUDE_DIRS})
else()
    MESSAGE(FATAL_ERROR "SystemC library not found.")
endif()

FIND_PACKAGE(TLM)
if(TLM_FOUND)
    INCLUDE_DIRECTORIES(${TLM_INCLUDE_DIRS})
else()
    MESSAGE( FATAL_ERROR "TLM library not found." )
endif()

FIND_PACKAGE(GREENLIB)
if(GREENLIB_FOUND)
    INCLUDE_DIRECTORIES(${GREENLIB_INCLUDE_DIRS})
    LINK_DIRECTORIES(${GREENLIB_LIBRARY_DIRS})
else()
    MESSAGE(FATAL_ERROR "GreenLib not found.")
endif()

FIND_PACKAGE(SIMPLECPU)
if(SIMPLECPU_FOUND)
    INCLUDE_DIRECTORIES(${SIMPLECPU_INCLUDE_DIRS})
    LINK_DIRECTORIES(${SIMPLECPU_LIBRARY_DIRS})
else()
    MESSAGE(FATAL_ERROR "SimpleCpu not found.")
endif()

FIND_PACKAGE(AWSSDK)
if(AWSSDK_FOUND)
    INCLUDE_DIRECTORIES(${AWSSDK_INCLUDE_DIRS})
    LINK_DIRECTORIES(${AWSSDK_LIBRARY_DIRS})
else()
    MESSAGE(FATAL_ERROR "AwsSdk not found.")
endif()

### AWSSDK ###
INSTALL(DIRECTORY ${AWSSDK_INCLUDE_DIRS}/ DESTINATION include)
INSTALL(DIRECTORY ${AWSSDK_LIBRARY_DIRS}/ DESTINATION lib)

set (INCLUDE_DIRS ${INCLUDE_DIRS} include)

INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSC_INCLUDE_DYNAMIC_PROCESSES -Wall -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSC_INCLUDE_DYNAMIC_PROCESSES -Wall -Werror -DDEBUG_LOG=1")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSC_INCLUDE_DYNAMIC_PROCESSES -Wall -Werror")

set(FPGA_SC_WRAPPER_FILES 
  src/nvdla_fpga_sc_wrapper.cpp
  src/extmem_fpga_sc_wrapper.cpp
  src/transactors/thread_safe_event.cpp
  src/transactors/gsgp2fpga.cpp
  src/transactors/msi2gss.cpp
  )

if(MINGW)
    add_library(fpga_sc_wrapper STATIC ${FPGA_SC_WRAPPER_FILES})
else()
    add_library(fpga_sc_wrapper SHARED ${FPGA_SC_WRAPPER_FILES})
endif()  

TARGET_LINK_LIBRARIES(fpga_sc_wrapper ${GREENLIB_LIBRARIES} ${AWSSDK_LIBRARIES} pthread)

# Installation paths
INSTALL(DIRECTORY include/fpga_sc_wrapper
        DESTINATION include
)

INSTALL(TARGETS fpga_sc_wrapper DESTINATION lib)
