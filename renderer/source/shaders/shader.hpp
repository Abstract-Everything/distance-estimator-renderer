#pragma once

#include "uniform.hpp"

#include <GL/glew.h>

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace renderer
{

class Shader
{
public:
	void initialise_vertex_data();

	void draw();
	void set_uniform (Uniform const& uniform);

	std::vector<std::unique_ptr<Uniform>> change_shader (
		std::filesystem::path const& include_path,
		std::filesystem::path const& shader_path);

private:
	bool                     valid      = false;
	bool                     new_shader = false;
	std::vector<std::string> errors;

	std::string vertex_shader_code;
	std::string fragment_shader_code;

	GLuint program_id   = 0;
	GLuint vertex_array = 0;

	void print_shaders (
		std::string const& vertex_shader,
		std::string const& fragment_shader);
};

} // namespace renderer
