add_library (renderer STATIC
	"${CMAKE_CURRENT_LIST_DIR}/renderer"

	"${CMAKE_CURRENT_LIST_DIR}/shaders/shader"

	"${CMAKE_CURRENT_LIST_DIR}/preprocessor/lexer"
	"${CMAKE_CURRENT_LIST_DIR}/preprocessor/parser"

	"${CMAKE_CURRENT_LIST_DIR}/interfaces/gl_interface"

	"${CMAKE_CURRENT_LIST_DIR}/utilities/file_loader"
)

target_include_directories (renderer
	PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/../include/"

	PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}/../include/renderer"

	"${CMAKE_CURRENT_LIST_DIR}/"
	"${CMAKE_CURRENT_LIST_DIR}/shaders"
	"${CMAKE_CURRENT_LIST_DIR}/preprocessor"
	"${CMAKE_CURRENT_LIST_DIR}/interfaces"
	"${CMAKE_CURRENT_LIST_DIR}/utilities"
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

set (shaders
	"glsl/2d/colouring.frag"
	"glsl/2d/complex_plane.frag"
	"glsl/2d/complex_plane.vert"
	"glsl/2d/signed_distance_functions/flat_background.frag"
	"glsl/2d/signed_distance_functions/julia_set.frag"
	"glsl/2d/signed_distance_functions/mandelbrot.frag"
	"glsl/3d/camera_frame.vert"
	"glsl/3d/colouring.frag"
	"glsl/3d/ray_march.frag"
	"glsl/3d/signed_distance_functions/cube.frag"
	"glsl/3d/signed_distance_functions/sphere.frag"
	"glsl/constants.glsl"
	"glsl/structures.glsl"
	"glsl/utility.frag"
)

foreach(file IN LISTS shaders)
	configure_file (
		"${CMAKE_CURRENT_LIST_DIR}/${file}"
		"${CMAKE_BINARY_DIR}/${file}"
		COPYONLY
	)
endforeach()
