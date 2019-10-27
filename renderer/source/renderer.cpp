#include "renderer.hpp"

#include "parser.hpp"
#include "shader.hpp"
#include "gl_interface.hpp"
#include "file_loader.hpp"

#include <iostream>

namespace
{
  std::unique_ptr<renderer::Shader> shader = nullptr;
}

namespace renderer
{

Renderer::Renderer (std::filesystem::path const &glsl_path)
{
  gl::init ();
  io::init (glsl_path);
  shader = std::make_unique <renderer::Shader> ();
}

std::vector <std::unique_ptr <Uniform>> Renderer::set_shader (std::filesystem::path const &shader_path)
{
  return shader->change_shader (io::get_glsl_path() / shader_path);
}

std::vector <std::filesystem::path> Renderer::get_shaders ()
{
  std::vector <std::filesystem::path> shaders;
  auto glsl_paths = { io::get_glsl_path() / "2d/signed_distance_functions",
                      io::get_glsl_path() / "3d/signed_distance_functions"};
  for (io::file_query <std::filesystem::path> const &path :
       io::load_recursive (glsl_paths, "frag"))
  {
    if (!path.exists)
    {
      std::cerr << path.error;
      continue;
    }
    
    preprocessor::Parser parser (path.contents);
    if (parser.is_valid())
      shaders.push_back (path.contents);
    else
      std::cerr << parser.get_errors()[0];
  }
  return shaders;
}

void Renderer::set_uniform (Uniform const &uniform_data)
{
  shader->set_uniform (uniform_data);
}

void Renderer::render()
{
  shader->draw();
}

}
