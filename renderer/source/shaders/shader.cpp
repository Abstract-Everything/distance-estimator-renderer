#include "shader.hpp"

#include "gl_interface.hpp"

#include <iostream>

namespace renderer
{

std::vector<std::unique_ptr<Uniform>> Shader::change_shader (
	std::filesystem::path const& include_path,
	std::filesystem::path const& shader_path)
{
	valid = false;
	if (shader_path.empty())
	{
		return {};
	}

	preprocessor::Parser parser (include_path, shader_path);
	if (!parser.is_valid())
	{
		print_parser_errors (parser);
		return {};
	}

	auto [success, new_program_id] = gl::create_program (
		parser.get_vertex_shader_code(),
		parser.get_fragment_shader_code());

	if (!success)
	{
		std::cout << "Failed to create opengl program.\n";
		print_parser_errors (parser);
		return {};
	}

	valid = true;
	glDeleteProgram (program_id);
	program_id = new_program_id;

	return parser.get_uniforms();
}

void Shader::render()
{
	if (valid)
	{
		glClearColor (0.7f, 0.7f, 0.7f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram (program_id);
		screen_vertices.render();
		glUseProgram (0);
	}
	else
	{
		glClearColor (1.0f, 0.0f, 0.0f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}
}

void Shader::set_uniform (Uniform const& uniform)
{
	if (!valid)
	{
		return;
	}

	try
	{
		gl::set_uniform (program_id, uniform);
	}
	catch (std::bad_cast const& /* e */)
	{
		std::cerr << "Tried to write to " << uniform.get_name()
				  << " with a different type.\n";
	}
}

void Shader::print_parser_errors (preprocessor::Parser const& parser)
{
	if (!parser.get_errors().empty())
	{
		std::cout
			<< "------------------------------------------------------------"
			<< "\nParser Errors:\n";

		for (std::string const& error : parser.get_errors())
		{
			std::cout << error << "\n";
		}
	}

	std::cout << "------------------------------------------------------------"
			  << "\nVertex Shader:\n"
			  << parser.get_vertex_shader_code()

			  << "------------------------------------------------------------"
			  << "\nFragment_shader:\n"
			  << parser.get_fragment_shader_code()

			  << "------------------------------------------------------------"
			  << "\n";
}

} // namespace renderer
