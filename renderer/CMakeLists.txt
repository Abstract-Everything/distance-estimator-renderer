cmake_minimum_required (VERSION 3.10)
project (renderer VERSION 0.1
                  DESCRIPTION "Distance estimation rendering engine."
                  LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message (STATUS "Setting build type to release")
  set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  set_property (CACHE CMAKE_BUILD_TYPE PROPERTY
    STRINGS "Debug" "Release")
endif()

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory (source)
