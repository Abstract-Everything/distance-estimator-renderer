#include "shader.hpp"

#include "gl_interface.hpp"
#include "parser.hpp"

#include <iostream>

namespace renderer
{

void Shader::initialise_vertex_data()
{
	const std::vector<float> vertices{
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
	glBufferData (
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof (float),
		&vertices[0],
		GL_STATIC_DRAW);

	glEnableVertexAttribArray (0);
	glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, sizeof (float) * 2, 0);

	glBindVertexArray (0);
}

std::vector<std::unique_ptr<Uniform>> Shader::change_shader (
	std::filesystem::path const& include_path,
	std::filesystem::path const& shader_path)
{
	if (shader_path.empty())
	{
		valid = false;
		return {};
	}

	preprocessor::Parser parser (include_path, shader_path);
	valid = parser.is_valid();
	if (!valid)
	{
		print_shaders (
			parser.get_vertex_shader_code(),
			parser.get_fragment_shader_code());
		errors = parser.get_errors();
		return {};
	}

	vertex_shader_code   = parser.get_vertex_shader_code();
	fragment_shader_code = parser.get_fragment_shader_code();
	new_shader           = true;

	return parser.get_uniforms();
}

void Shader::draw()
{
	if (new_shader)
	{
		glDeleteProgram (program_id);
		auto [success, new_program_id]
			= gl::create_program (vertex_shader_code, fragment_shader_code);

		valid      = success;
		program_id = new_program_id;
		new_shader = false;

		if (!valid)
		{
			print_shaders (vertex_shader_code, fragment_shader_code);
		}
	}

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

void Shader::set_uniform (Uniform const& uniform)
{
	if (!valid || new_shader)
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

void Shader::print_shaders (
	std::string const& vertex_shader,
	std::string const& fragment_shader)
{
	std::cout << "-------------------------------------------------------------"
				 "-------------------"
			  << "\nVertex Shader:\n"
			  << vertex_shader

			  << "-------------------------------------------------------------"
				 "-------------------"
			  << "\nFragment_shader:\n"
			  << fragment_shader

			  << "-------------------------------------------------------------"
				 "-------------------\n"
			  << std::flush;
}

} // namespace renderer
