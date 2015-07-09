# Copyright (c) 2015 Alireza Kheirkhahan
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


# We require at least CMake V2.8.10
cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)

################################################################################
# project metadata
################################################################################
project(hpxio CXX)

set(HPXIO_MAJOR_VERSION 0)
set(HPXIO_MINOR_VERSION 1)
set(HPXIO_PATCH_LEVEL   0)
set(HPXIO_VERSION "${HPXIO_MAJOR_VERSION}.${HPXIO_MINOR_VERSION}.${HPXIO_PATCH_LEVEL}")
set(HPXIO_LIBRARY_VERSION "${HPXIO_VERSION}")


################################################################################
# Find and Include HPX
################################################################################
if(NOT HPX_ROOT)
   message(FATAL_ERROR "HPX_ROOT not set, unable to find HPX!")
endif()

if(HPX_ROOT AND NOT HPX_DIR)
   set(HPX_DIR "${HPX_ROOT}/lib/cmake/hpx")
endif()

# Instruct cmake to find the HPX settings, it looks for HPXConfig.cmake in ${HPX_DIR}
find_package(HPX)

include_directories(${HPX_INCLUDE_DIR})
link_directories(${HPX_LIBRARY_DIR})

# Adjust auxiliary build settings for HPX to work properly
if(NOT MSVC)
  set(HPX_RPATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}${HPX_RPATH})
else()
  # enable solution folders for MSVC
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  if(HPX_NO_INSTALL)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/$(Configuration)" CACHE PATH
      "Prefix prepended to install directories." FORCE)
  endif()
endif()

################################################################################
# Additional CMake configuration
################################################################################
set(CMAKE_MODULE_PATH "${hpxio_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )



################################################################################
# Build and Create Executable
################################################################################
include_directories("${hpxio_SOURCE_DIR}" "${CMAKE_BINARY_DIR}")
add_subdirectory(src)
add_subdirectory(examples)

################################################################################
# Installation
################################################################################
install(
  DIRECTORY hpxio
  DESTINATION include/hpxio
  FILES_MATCHING PATTERN "*.hpp"
  PATTERN ".git" EXCLUDE
  PATTERN "CMakeFiles" EXCLUDE
  PATTERN "CTestFiles" EXCLUDE)

