set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (CMAKE_AUTOUIC ON)
qt5_add_resources (QT_RESOURCES qml.qrc)

add_executable (viewer
	"${CMAKE_CURRENT_LIST_DIR}/main"

	"${CMAKE_CURRENT_LIST_DIR}/Wrapper/uniform"
	"${CMAKE_CURRENT_LIST_DIR}/Wrapper/renderer"

	"${CMAKE_CURRENT_LIST_DIR}/UI/main_window"
	"${CMAKE_CURRENT_LIST_DIR}/UI/viewport"
	"${CMAKE_CURRENT_LIST_DIR}/UI/inspector"
	"${CMAKE_CURRENT_LIST_DIR}/UI/camera"

	${QT_RESOURCES}
)

target_include_directories (viewer PRIVATE
	${QtQml_INCLUDE_DIRS}

	"${CMAKE_CURRENT_LIST_DIR}/"
	"${CMAKE_CURRENT_LIST_DIR}/Wrapper"
	"${CMAKE_CURRENT_LIST_DIR}/UI"
)

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
