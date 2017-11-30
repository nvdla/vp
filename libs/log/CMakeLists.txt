#
# CMakeLists.txt
#
# Copyright (C) 2014, GreenSocs ltd.
#
# Developped by Konrad Frederic <fred.konrad@greensocs.com>
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
PROJECT(LOG)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

FIND_PACKAGE(SystemC)
if(SystemC_FOUND)
    INCLUDE_DIRECTORIES(${SystemC_INCLUDE_DIRS})
else()
    MESSAGE(FATAL_ERROR "SystemC library not found.")
endif()

set (INCLUDE_DIRS include)

INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-Wall -Werror -pedantic -O2 -DSC_INCLUDE_DYNAMIC_PROCESSES -Wp,-w -std=c++11")

if(NOT MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

add_library(log SHARED
  src/log.cpp
)

# Installation paths
INSTALL(DIRECTORY include/log
        DESTINATION include
        PATTERN "*~" EXCLUDE
)
# Not sure it's the right destination? Maybe we want lib64 in case of 64bits?
INSTALL(TARGETS log DESTINATION lib)

# Make tests
# ENABLE_TESTING()
# ADD_SUBDIRECTORY(tests)

