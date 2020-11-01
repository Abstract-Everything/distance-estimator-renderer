#include "renderer.hpp"

#include "file_loader.hpp"
#include "gl_interface.hpp"
#include "parser.hpp"
#include "shader.hpp"

#include <iostream>

namespace renderer
{

Renderer::Renderer()
{
	gl::init();
	shader = new Shader();
}

Renderer::~Renderer()
{
	delete shader;
}

std::vector<std::filesystem::path> Renderer::get_shaders (
	std::filesystem::path const&              include_path,
	std::vector<std::filesystem::path> const& paths)
{
	std::vector<std::filesystem::path> shaders;
	for (io::file_query<std::filesystem::path> const& file :
		 io::load_recursive (paths, "frag"))
	{
		if (!file.exists)
		{
			std::cerr << file.error;
			continue;
		}

		preprocessor::Parser parser (include_path, file.contents);
		if (parser.is_valid())
			shaders.push_back (file.contents);
		else
			std::cerr << parser.get_errors()[0];
	}
	return shaders;
}

std::vector<std::unique_ptr<Uniform>> Renderer::set_shader (
	std::filesystem::path const& include_path,
	std::filesystem::path const& shader_path)
{
	return shader->change_shader (include_path, shader_path);
}

void Renderer::set_uniform (Uniform const& uniform)
{
	shader->set_uniform (uniform);
}

void Renderer::render()
{
	shader->render();
}

} // namespace renderer
