#include "viewport.hpp"

#include "renderer/uniform.hpp"
#include "uniform.hpp"

#include <QString>
#include <QStringList>

#include <iostream>
#include <filesystem>

Viewport::Viewport (QQuickItem *object) : QQuickFramebufferObject (object)
{
  setMirrorVertically (true);
}

QQuickFramebufferObject::Renderer * Viewport::createRenderer() const
{
  return new Viewport_Renderer;
}

void Viewport::connected ()
{
  update_flags += Renderer_Update_Flags::Update_Shaders_Paths;
  update ();
}

void Viewport::per_frame ()
{
  if (shader_set)
  {
    time += 0.001;
    update_globals (true, false);
  }
}

void Viewport::set_resolution (uint x, uint y)
{
  if (resolution_x != x || resolution_y != y)
  {
    resolution_x = x;
    resolution_y = y;
    update_globals (false, true);
  }
}

void Viewport::update_globals (bool update_time, bool update_resolution)
{
  if (update_time) 
    update_uniform (renderer::Typed_Uniform<float> ("globals.time", { static_cast<float> (time) }));

  if (update_resolution)
    update_uniform (renderer::Typed_Uniform<unsigned int> ("globals.resolution", { resolution_x,  resolution_y }));
}

std::vector <std::filesystem::path> Viewport::get_shader_paths ()
{
  return shader_paths;
}

void Viewport::set_shader_paths (std::vector <std::filesystem::path> const &p_shader_paths)
{
  shader_paths = p_shader_paths;

  QStringList shader_names;
  shader_names.push_back ("");

  for (std::filesystem::path const &shader_path : shader_paths)
    shader_names.push_back (QString::fromStdString (shader_path.stem().string()));

  emit shader_paths_updated (shader_names);
}

std::filesystem::path Viewport::get_selected_shader ()
{
  return selected_shader;
}

void Viewport::set_selected_shader (std::filesystem::path const &shader_path)
{
  time = 0;
  update_globals (true, true);
  shader_set = true;

  selected_shader = shader_path;
  update_flags |= Renderer_Update_Flags::Selected_Shader;
  update ();
}

void Viewport::update_selected_shader (QString const &shader_name)
{
  if (shader_name.isEmpty()) return;

  for (std::filesystem::path const &shader_path : shader_paths)
  {
    if (shader_path.stem().string() == shader_name.toStdString())
    {
      set_selected_shader (shader_path);
      return;
    }
  }

  std::cerr << "Set shader not found.";
}

void Viewport::update_uniform (Uniform const &uniform)
{
  updated_uniforms.push_back (uniform);
  update ();
  emit uniform_updated (uniform);
}

void Viewport::update_uniforms (std::vector <std::unique_ptr <renderer::Uniform>> const &p_uniforms)
{
  QMap <QString, Uniform> uniforms;
  for (std::unique_ptr <renderer::Uniform> const &uniform : p_uniforms)
    uniforms.insert (QString::fromStdString (uniform->get_name()), *uniform);

  emit uniforms_updated (uniforms);
}

bool Viewport::more_update_uniforms ()
{
  return updated_uniforms.size() > 0;
}

Uniform Viewport::get_next_uniform ()
{
  Uniform uniform = updated_uniforms.front();
  updated_uniforms.pop_front();
  return uniform;
}

int Viewport::get_update_flags ()
{
  int current_update_flags = update_flags;
  update_flags = 0;
  return current_update_flags;
}

Viewport_Renderer::Viewport_Renderer () : QQuickFramebufferObject::Renderer ()
{
  const std::filesystem::path executable_path (QCoreApplication::applicationDirPath ().toStdString ());
  const std::filesystem::path glsl_path = executable_path.parent_path () / "resources/glsl";
  renderer = std::make_unique <renderer::Renderer> (glsl_path);
}

QOpenGLFramebufferObject *Viewport_Renderer::createFramebufferObject (QSize const &size)
{
  QOpenGLFramebufferObjectFormat format;
  format.setAttachment (QOpenGLFramebufferObject::CombinedDepthStencil);
  return new QOpenGLFramebufferObject (size, format);
}

void Viewport_Renderer::render ()
{
  renderer->render ();

  if (framebuffer)
  {
    framebuffer->window()->resetOpenGLState();
    update ();
  }
}

void Viewport_Renderer::synchronize (QQuickFramebufferObject *p_viewport)
{
  framebuffer = p_viewport;
  Viewport *viewport = static_cast <Viewport *> (p_viewport);

  viewport->set_resolution (framebuffer->width(), framebuffer->height());

  int update_flags = viewport->get_update_flags();
  if (update_flags & Renderer_Update_Flags::Update_Shaders_Paths)
    viewport->set_shader_paths (renderer->get_shaders());

  if (update_flags & Renderer_Update_Flags::Selected_Shader)
  {
    std::filesystem::path path = viewport->get_selected_shader();
    std::vector <std::unique_ptr <renderer::Uniform>> uniforms = renderer->set_shader (path);
    viewport->update_uniforms (uniforms);
  }

  while (viewport->more_update_uniforms())
    renderer->set_uniform (*viewport->get_next_uniform().get_as_renderer_uniform());
}
