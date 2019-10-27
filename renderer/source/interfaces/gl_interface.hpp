#pragma once

#include "uniform.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

#include <vector>
#include <string>
#include <iostream>

namespace renderer::gl
{

enum class Shader_Type {
  Vertex   = GL_VERTEX_SHADER,
  Fragment = GL_FRAGMENT_SHADER
};

void init ();

GLuint compile_shader (std::string const &shader_source, Shader_Type shader_type);
GLuint create_program(std::vector<unsigned int> const &shaders);
void set_uniform (GLuint program_id, renderer::Uniform const &uniform);

}

