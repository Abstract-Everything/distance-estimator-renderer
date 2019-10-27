#pragma once

#include "renderer/renderer.hpp"

#include "uniform.hpp"

#include <QtQuick/QQuickWindow>
#include <QtQuick/QQuickFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>

#include <vector>
#include <map>
#include <string>

enum Renderer_Update_Flags
{
  Selected_Shader      = 1 << 0,
  Update_Shaders_Paths = 1 << 1,
};

class Viewport : public QQuickFramebufferObject
{
  Q_OBJECT

public:
  void connected ();
  void per_frame ();

  void set_resolution (unsigned int x, unsigned int y);
  void update_globals (bool update_time, bool update_resolution);

  std::vector <std::filesystem::path> get_shader_paths ();
  void set_shader_paths (std::vector <std::filesystem::path> const &p_shader_paths);

  std::filesystem::path get_selected_shader ();
  void set_selected_shader (std::filesystem::path const &shader_path);

  void update_uniforms (std::vector <std::unique_ptr <renderer::Uniform>> const &p_uniforms);
  bool more_update_uniforms ();
  Uniform get_next_uniform ();

  int get_update_flags ();

  QQuickFramebufferObject::Renderer *createRenderer() const override;
  Viewport (QQuickItem *object = nullptr);

signals:
  void shader_paths_updated (QStringList shader_paths);
  void uniform_updated (Uniform uniform);
  void uniforms_updated (QMap <QString, Uniform> uniforms);

public slots:
  void update_selected_shader (QString const &shader_name);
  void update_uniform (Uniform const &uniform);

private:
  bool shader_set = false;
  qreal time = 0;
  uint resolution_x = 0u;
  uint resolution_y = 0u;

  int update_flags = 0;
  std::filesystem::path selected_shader;
  std::vector <std::filesystem::path> shader_paths;
  std::list <Uniform> updated_uniforms;
};

class Viewport_Renderer : public QQuickFramebufferObject::Renderer
{
public:
  Viewport_Renderer ();
  void render () override;
  void synchronize (QQuickFramebufferObject *p_viewport) override;
  QOpenGLFramebufferObject *createFramebufferObject (QSize const &size) override;

private:
  std::unique_ptr <renderer::Renderer> renderer = nullptr;
  QQuickFramebufferObject *framebuffer;
};
