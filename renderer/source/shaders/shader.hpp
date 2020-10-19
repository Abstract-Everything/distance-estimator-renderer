#pragma once

#include "uniform.hpp"
#include "screen_vertex_array.hpp"

#include <GL/glew.h>

#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include <memory>

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
	std::unique_ptr<Screen_Vertex_Array> screen_vertices = nullptr;

	void print_shaders (
		std::string const& vertex_shader,
		std::string const& fragment_shader);
};

} // namespace renderer
