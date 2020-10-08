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

file (
	GLOB_RECURSE
	renderer_sources
	CONFIGURE_DEPENDS
	"*.cpp"
)

file (
	GLOB_RECURSE
	renderer_headers
	CONFIGURE_DEPENDS
	"*.hpp"
)
header_directories (renderer_directories ${renderer_headers})

file (
	GLOB_RECURSE
	renderer_shaders
	RELATIVE "${CMAKE_CURRENT_LIST_DIR}/source"
	"*.glsl" "*.vert" "*.frag"
)

add_library (renderer STATIC ${renderer_sources} ${renderer_headers})

target_include_directories (renderer
	PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/"
	PRIVATE "${renderer_directories}"
)

set_target_properties (renderer
	PROPERTIES CXX_STANDARD 17
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if (MSVC)
	target_compile_options (renderer PRIVATE
		/W4
		/WX

		$<$<CONFIG:RELEASE>:
			/O2
		>

		$<$<CONFIG:DEBUG>:
			/Zi
		>
	)
else()
	target_compile_options (renderer PRIVATE
		-Wall
		-Wextra
		-Werror
		-fstrict-aliasing

		$<$<CONFIG:RELEASE>:
			-O3
		>

		$<$<CONFIG:DEBUG>:
			-g
			$<$<CXX_COMPILER_ID:Clang>:
				-D_GLIBCXX_DEBUG
			>
		>
	)

endif()

target_compile_definitions (renderer PRIVATE
	$<$<CONFIG:RELEASE>:NDEBUG>
	$<$<CONFIG:DEBUG>:DEBUG>
)

target_link_libraries (renderer PUBLIC
	OpenGL::GL
	GLEW::GLEW
)

foreach(shader IN LISTS renderer_shaders)
	configure_file (
		"${CMAKE_CURRENT_LIST_DIR}/source/${shader}"
		"${CMAKE_BINARY_DIR}/${shader}"
		COPYONLY
	)
endforeach()

group_sources(
	"${CMAKE_CURRENT_LIST_DIR}"
	"${renderer_sources}" "${renderer_headers}"
)
