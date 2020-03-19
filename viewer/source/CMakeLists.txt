set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (CMAKE_AUTOUIC ON)
qt5_add_resources (QT_RESOURCES qml.qrc)

add_executable (viewer
  "${CMAKE_CURRENT_LIST_DIR}/main"
  "${CMAKE_CURRENT_LIST_DIR}/main_window"
  "${CMAKE_CURRENT_LIST_DIR}/viewport"
  "${CMAKE_CURRENT_LIST_DIR}/inspector"
  "${CMAKE_CURRENT_LIST_DIR}/camera"
  "${CMAKE_CURRENT_LIST_DIR}/uniform"

  ${QT_RESOURCES}
)

target_include_directories (viewer PRIVATE
  ${QtQml_INCLUDE_DIRS}

  "${CMAKE_CURRENT_LIST_DIR}/"
)

set_target_properties (viewer
  PROPERTIES CXX_STANDARD 17
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_compile_definitions (viewer PRIVATE
  ${QtQml_DEFINITIONS}
  ${${Qt5Quick_DEFINITIONS}}
)

if (<$CXX_COMPILER_ID:"GNU, Clang">)
	target_compile_options (viewer PRIVATE
	  -Wall
	  -Wextra
	  -Werror
	  -fstrict-aliasing
	  -pedantic-errors
	  -pedantic
	  
	  $<$<CONFIG:RELEASE>:
		-O3>

	  $<$<CONFIG:DEBUG>:
		-g
		$<$<CXX_COMPILER_ID:Clang>:-D_GLIBCXX_DEBUG>>
	)
	
	target_link_libraries (viewer
		-lstdc++fs
	)

elseif(<$CXX_COMPILER_ID:"MSVC">)
	target_compile_options (viewer PRIVATE
	  /W4
	  /WX
	  
	  $<$<CONFIG:RELEASE>:
		/O2>

	  $<$<CONFIG:DEBUG>:
		/Zi>
	)
endif()

target_compile_definitions (viewer PRIVATE
	  $<$<CONFIG:RELEASE>:
		NDEBUG>

	  $<$<CONFIG:DEBUG>:
		DEBUG>
)

target_link_libraries (viewer
  Qt5::Qml
  Qt5::Quick
  renderer
)
