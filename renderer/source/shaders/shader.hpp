#pragma once

#include "uniform.hpp"

#include <GL/glew.h>

#include <map>
#include <vector>
#include <string>
#include <filesystem>

namespace renderer {

class Shader
{
public:
  void draw();
  void set_uniform (Uniform const &uniform);

  Shader ();
  std::vector <std::unique_ptr <Uniform>> change_shader (std::filesystem::path const &shader_path);

private:
  bool valid = false;
  std::vector <std::string> errors;

  GLuint program_id;
  GLuint vertex_array;

  void create_vertex_data();
};

}
