cmake_minimum_required (VERSION 3.16)
project (
	viewer
	VERSION 0.1
	DESCRIPTION "A gui for the renderer."
	LANGUAGES CXX
)

find_package (Qt5 COMPONENTS Qml Quick REQUIRED)


set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (CMAKE_AUTOUIC ON)
qt5_add_resources (QT_RESOURCES "${CMAKE_CURRENT_LIST_DIR}/source/qml.qrc")

file (
	GLOB_RECURSE
	viewer_sources
	CONFIGURE_DEPENDS
	"*.cpp"
)

file (
	GLOB_RECURSE
	viewer_headers
	CONFIGURE_DEPENDS
	"*.hpp"
)
header_directories (viewer_directories ${viewer_headers})

file (
	GLOB_RECURSE
	viewer_qtres
	CONFIGURE_DEPENDS
	"*.qml" "*.qrc"
)

add_executable (viewer
	${viewer_sources} ${viewer_headers} ${viewer_qtres} ${QT_RESOURCES})

target_include_directories (
	viewer PRIVATE ${QtQml_INCLUDE_DIRS} "${viewer_directories}")

set_target_properties (viewer
	PROPERTIES CXX_STANDARD 17
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_compile_definitions (viewer PRIVATE
	${QtQml_DEFINITIONS}
	${${Qt5Quick_DEFINITIONS}}
)

if (MSVC)
	target_compile_options (viewer PRIVATE
		/W4

		$<$<CONFIG:RELEASE>:
			/O2
		>

		$<$<CONFIG:DEBUG>:
			/Zi
		>
	)
else()
	target_compile_options (viewer PRIVATE
		-Wall
		-Wextra
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

target_compile_definitions (viewer PRIVATE
	$<$<CONFIG:RELEASE>:NDEBUG>
	$<$<CONFIG:DEBUG>:DEBUG>
)

target_link_libraries (viewer
	Qt5::Qml
	Qt5::Quick
	renderer
)

group_sources(
	"${CMAKE_CURRENT_LIST_DIR}"
	"${viewer_sources}" "${viewer_headers}" "${viewer_qtres}")
