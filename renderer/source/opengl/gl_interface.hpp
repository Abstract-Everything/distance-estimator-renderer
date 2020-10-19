#pragma once

#include "uniform.hpp"

#include <GL/glew.h>

#include <iostream>
#include <string>
#include <vector>

namespace renderer::gl
{

enum class Shader_Type
{
	Vertex   = GL_VERTEX_SHADER,
	Fragment = GL_FRAGMENT_SHADER
};

void init();

std::pair<bool, GLuint>
compile_shader (std::string const& shader_source, Shader_Type shader_type);

std::pair<bool, GLuint> create_program (
	const std::string& vertex_shader_code,
	const std::string& fragment_shader_code);

void set_uniform (GLuint program_id, renderer::Uniform const& uniform);

} // namespace renderer::gl