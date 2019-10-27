add_library (renderer
  "${CMAKE_CURRENT_LIST_DIR}/renderer"

  "${CMAKE_CURRENT_LIST_DIR}/shaders/shader"

  "${CMAKE_CURRENT_LIST_DIR}/preprocessor/lexer"
  "${CMAKE_CURRENT_LIST_DIR}/preprocessor/parser"

  "${CMAKE_CURRENT_LIST_DIR}/interfaces/gl_interface"

  "${CMAKE_CURRENT_LIST_DIR}/utilities/file_loader"
)

target_include_directories(renderer
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

target_compile_options (renderer PRIVATE
  -Wall
  -Wextra
  -Werror
  -fstrict-aliasing
  -pedantic-errors
  -pedantic
)

target_compile_options (renderer PRIVATE
  $<$<CONFIG:RELEASE>:
    -O3
    -DNDEBUG>

  $<$<CONFIG:DEBUG>:
    -g
    -DDEBUG
    $<$<CXX_COMPILER_ID:Clang>:-D_GLIBCXX_DEBUG>>
)

target_link_libraries (renderer PUBLIC
  GL
  GLEW
)

add_custom_command (
  TARGET renderer
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_LIST_DIR}/glsl ${CMAKE_BINARY_DIR}/resources/glsl/
  COMMENT "Copying shaders." VERBATIM)

