#include "shader.hpp"

#include "parser.hpp"

#include "gl_interface.hpp"

#include <iostream>

namespace renderer {

Shader::Shader()
{
  create_vertex_data ();
}

std::vector <std::unique_ptr <Uniform>> Shader::change_shader (std::filesystem::path const &shader_path)
{
  if (shader_path.empty())
  {
    valid = false;
    return {};
  }

  preprocessor::Parser parser (shader_path);
  if (!(valid = parser.is_valid()))
  {
    #ifdef DEBUG
    std::cout <<
      "--------------------------------------------------------------------------------" <<
      "\nVertex Shader:\n" <<
      parser.get_vertex_shader_code() <<

      "--------------------------------------------------------------------------------" <<
      "\nFragment_shader:\n" <<
      parser.get_fragment_shader_code() <<

      "--------------------------------------------------------------------------------\n" <<
      std::flush;
    #endif
    errors = parser.get_errors();
    return {};
  }

  program_id = gl::create_program({ 
    compile_shader(parser.get_vertex_shader_code(), gl::Shader_Type::Vertex),
    compile_shader(parser.get_fragment_shader_code(), gl::Shader_Type::Fragment)
  });

  std::vector <std::unique_ptr <Uniform>> uniforms = parser.get_uniforms();
  for (std::unique_ptr <Uniform> const &uniform : uniforms)
    set_uniform (*uniform);
  return uniforms;
}

void Shader::draw()
{
  glClearColor (0.7f, 0.7f, 0.7f, 1.0f);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (valid)
  {
    glUseProgram (program_id);

    glBindVertexArray (vertex_array);
    glDrawArrays (GL_TRIANGLES, 0, 6);
    glBindVertexArray (0);

    glUseProgram (0);
  }
}

void Shader::set_uniform (Uniform const &uniform)
{ 
  if (!valid) return;

  try
  { gl::set_uniform (program_id, uniform); }

  catch (std::bad_cast const &e)
  { std::cerr << "Tried to write to " << uniform.get_name() << " with a different type.\n"; }
}

void Shader::create_vertex_data()
{
  const std::vector<float> vertices {
     1,  1,
     1, -1,
    -1, -1,
     1,  1,
    -1, -1,
    -1,  1
  };

  glGenVertexArrays (1, &vertex_array);
  glBindVertexArray (vertex_array);

  GLuint vertex_buffer;
  glGenBuffers (1, &vertex_buffer);
  glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData (GL_ARRAY_BUFFER, vertices.size() * sizeof (float), &vertices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray (0);
  glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, sizeof (float) * 2, 0);

  glBindVertexArray (0);
}

}
