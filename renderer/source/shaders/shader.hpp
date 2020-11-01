#pragma once

#include "parser.hpp"
#include "screen_vertex_array.hpp"
#include "uniform.hpp"

#include <GL/glew.h>

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace renderer
{

class Shader
{
public:
	void render();
	void set_uniform (Uniform const& uniform);

	std::vector<std::unique_ptr<Uniform>> change_shader (
		std::filesystem::path const& include_path,
		std::filesystem::path const& shader_path);

private:
	bool valid = false;

	GLuint              program_id = 0;
	Screen_Vertex_Array screen_vertices;

	void print_parser_errors (preprocessor::Parser const& parser);
};

} // namespace renderer
