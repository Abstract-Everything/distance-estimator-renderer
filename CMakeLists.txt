cmake_minimum_required (VERSION 3.16)
project (
	Distance_Estimation_Renderer
	VERSION 0.1
	DESCRIPTION "A Renderer which uses distance estimation to draw objects."
	LANGUAGES CXX
)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_EXTENSIONS OFF)

include (cmake/header_directories.cmake)
include (cmake/group_sources.cmake)

add_subdirectory (renderer)
add_subdirectory (viewer)
