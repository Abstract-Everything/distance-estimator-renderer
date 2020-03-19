cmake_minimum_required (VERSION 3.17)
project (Distance_Estimation_Renderer
  VERSION 0.1
  DESCRIPTION "A Renderer which uses distance estimation to draw objects."
  LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message (STATUS "Setting build type to release")
  set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
  set_property (CACHE CMAKE_BUILD_TYPE PROPERTY
    STRINGS "Debug" "Release")
endif()

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory (renderer)
add_subdirectory (viewer)
