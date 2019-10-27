#pragma once

#include "uniform.hpp"

#include <filesystem>
#include <map>
#include <vector>

namespace renderer
{

class Renderer
{
public:
  Renderer (std::filesystem::path const &glsl_path);

  std::vector <std::filesystem::path> get_shaders ();
  std::vector <std::unique_ptr <Uniform>> set_shader (std::filesystem::path const &shader_path);
  void set_uniform (Uniform const &uniform_data);

  void render();
};

}
