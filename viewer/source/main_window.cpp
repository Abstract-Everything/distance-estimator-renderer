#include "main_window.hpp"

#include "uniform.hpp"

#include "viewport.hpp"
#include "inspector.hpp"
#include "camera.hpp"

#include <QMetaType>
#include <QSurfaceFormat>

Q_DECLARE_METATYPE (Uniform)

Main_Window::Main_Window (QWindow *parent) : QQuickWindow (parent)
{
  QSurfaceFormat format;
  format.setDepthBufferSize (3);
  format.setStencilBufferSize (3);
  format.setProfile (QSurfaceFormat::CoreProfile);
  format.setOption (QSurfaceFormat::DebugContext);
  setFormat (format);
  create ();

  qRegisterMetaType<Uniform> ("Uniform");
  qRegisterMetaType<QMap <QString, Uniform>> ("QMap <QString, Uniform>>");

  connect (this, &Main_Window::sceneGraphInitialized, this, &Main_Window::init);
}

void Main_Window::init ()
{
  Viewport *viewport = findChild <Viewport *> ("viewport");
  QObject *mouse_area = findChild <QObject *> ("mouse_area");
  Inspector *inspector = findChild <Inspector *> ("inspector");

  Camera *camera = new Camera;
  mouse_area->installEventFilter (camera);

  connect (this, &Main_Window::beforeRendering, camera, &Camera::before_rendering);
  connect (this, &Main_Window::beforeRendering, viewport, &Viewport::per_frame);

  connect (viewport, &Viewport::shader_paths_updated, inspector, &Inspector::set_shader_list);
  connect (inspector, &Inspector::shader_selection_changed, viewport, &Viewport::update_selected_shader);
  
  connect (viewport, &Viewport::uniforms_updated, inspector, &Inspector::uniforms_updated);
  connect (viewport, &Viewport::uniforms_updated, camera, &Camera::uniforms_updated);

  connect (camera, &Camera::update_uniform, viewport, &Viewport::update_uniform);
  connect (inspector, &Inspector::update_uniform, viewport, &Viewport::update_uniform);

  connect (viewport, &Viewport::uniform_updated, inspector, &Inspector::uniform_updated);
  connect (viewport, &Viewport::uniform_updated, camera, &Camera::uniform_updated);

  viewport->connected();
}
