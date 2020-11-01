#pragma once

#include "uniform.hpp"

#include <filesystem>
#include <map>
#include <vector>

namespace renderer
{

class Shader;

class Renderer
{
public:
	Renderer();
	~Renderer();

	std::vector<std::filesystem::path> get_shaders (
		std::filesystem::path const&              include_path,
		std::vector<std::filesystem::path> const& paths);

	std::vector<std::unique_ptr<Uniform>> set_shader (
		std::filesystem::path const& include_path,
		std::filesystem::path const& shader_path);

	void set_uniform (Uniform const& uniform_data);

	void render (unsigned int width, unsigned int height);

private:
	// Using a pointer in order to not include shader.hpp which would need to
	//  to be accessible outside of the library.
	renderer::Shader* shader = nullptr;
};

} // namespace renderer
