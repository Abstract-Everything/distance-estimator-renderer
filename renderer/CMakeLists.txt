cmake_minimum_required (VERSION 3.16)
project (
	renderer
	VERSION 0.1
	DESCRIPTION "Distance estimation rendering engine."
	LANGUAGES CXX
)

find_package (GLEW REQUIRED)

find_package (OpenGL REQUIRED)
if (NOT OpenGL_OpenGL_FOUND)
	message (FATAL_ERROR "OpenGL library was not found.")
endif()

add_subdirectory (source)
