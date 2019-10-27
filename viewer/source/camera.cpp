#include "camera.hpp"

#include <iostream>

void Camera::before_rendering ()
{
  if (dimension == Dimension::Zero) return;

  move_camera ();
  pan_camera ();
  if (dimension != Dimension::Three) zoom_camera ();
}

bool Camera::eventFilter (QObject *watched, QEvent *event)
{
  if (event->type() == QEvent::MouseButtonPress ||
      event->type() == QEvent::MouseMove ||
      event->type() == QEvent::MouseButtonRelease)
  {
    watched->setProperty ("focus", true);
    QMouseEvent *mouse_event = static_cast<QMouseEvent *>(event);
    update_pan (*mouse_event);
    return true;
  }

  if (event->type() == QEvent::Wheel)
  {
    QWheelEvent *wheel_event = static_cast<QWheelEvent *>(event);
    update_zoom (*wheel_event);
  }

  if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
  {
    QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
    update_camera_speed (*key_event);
    if (dimension == Dimension::Two)   update_move_direction_2d (*key_event);
    if (dimension == Dimension::Three) update_move_direction_3d (*key_event);
    return true;
  }

  return QObject::eventFilter (watched, event);
}

void Camera::update_pan (QMouseEvent const &event)
{
  if (event.type() == QEvent::MouseButtonPress)
    last_mouse_position = event.pos();

  if (event.type() == QEvent::MouseMove)
  {
    pan_direction += QVector2D (event.pos() - last_mouse_position);
    last_mouse_position = event.pos ();
  }

  if (event.type() == QEvent::MouseButtonRelease)
    pan_direction = { 0.0f, 0.0f };
}

void Camera::update_zoom (QWheelEvent const &event)
{
  if (qAbs (event.angleDelta().y()) == 0 || dimension == Dimension::Three)
    return;

  float direction = event.angleDelta().y() > 0 ? 1 : -1;
  float zoom_factor = get_zoom_factor();
  float new_zoom =  direction * qMin (zoom_factor / 10.0, 0.1);
  if (qAbs (new_zoom) < 1.0f)
    zoom_offset = new_zoom;
}

void Camera::update_camera_speed (QKeyEvent const &event)
{
  const double fast_multiplier = 3.0;
  const double slow_multiplier = 10.0;
  const double base_camera_speed = 0.01;
  if (event.type() == QEvent::KeyPress)
  {
    if (event.key() == Qt::Key_Shift) camera_speed = base_camera_speed * fast_multiplier;
    if (event.key() == Qt::Key_Alt)   camera_speed = base_camera_speed / slow_multiplier;
  }
  else if (event.type() == QEvent::KeyRelease)
  {
    if (event.key() == Qt::Key_Shift || event.key() == Qt::Key_Alt)
      camera_speed = base_camera_speed;
  }
}

void Camera::update_move_direction_2d (QKeyEvent const &event)
{
  if (event.type() == QEvent::KeyPress)
  {
    if (event.key() == Qt::Key_A) move_direction[0] = -1;
    if (event.key() == Qt::Key_D) move_direction[0] =  1;
    if (event.key() == Qt::Key_W) move_direction[1] =  1;
    if (event.key() == Qt::Key_S) move_direction[1] = -1;
  }
  else if (event.type() == QEvent::KeyRelease)
  {
    if (event.key() == Qt::Key_A || event.key() == Qt::Key_D) move_direction[0] = 0;
    if (event.key() == Qt::Key_W || event.key() == Qt::Key_S) move_direction[1] = 0;
  }
}

void Camera::update_move_direction_3d (QKeyEvent const &event)
{
  if (event.type() == QEvent::KeyPress)
  {
    if (event.key() == Qt::Key_A) move_direction[0] = -1;
    if (event.key() == Qt::Key_D) move_direction[0] =  1;
    if (event.key() == Qt::Key_Space)   move_direction[1] =  1;
    if (event.key() == Qt::Key_Control) move_direction[1] = -1;
    if (event.key() == Qt::Key_W) move_direction[2] =  1;
    if (event.key() == Qt::Key_S) move_direction[2] = -1;
  }
  else if (event.type() == QEvent::KeyRelease)
  {
    if (event.key() == Qt::Key_A || event.key() == Qt::Key_D) move_direction[0] = 0;
    if (event.key() == Qt::Key_Space || event.key() == Qt::Key_Control) move_direction[1] = 0;
    if (event.key() == Qt::Key_W || event.key() == Qt::Key_S) move_direction[2] = 0;
  }
}

void Camera::move_camera ()
{
  if (qAbs (move_direction.x()) + qAbs (move_direction.y()) + qAbs (move_direction.z()) < 0.5f)
    return;

  QVector3D move_distance = move_direction.normalized() * camera_speed;
  if (dimension == Dimension::Two)
    move_distance *= get_zoom_factor ();

  if (dimension == Dimension::Three)
  {
    auto [right, up, forward] = calculate_basis();
    move_distance = right * move_distance[0] + up * move_distance[1] + forward * move_distance[2];
  }

  emit update_uniform (offset_uniform ("camera.position", move_distance));
}

void Camera::pan_camera ()
{
  if (qAbs (pan_direction.x()) < 0.5 || qAbs (pan_direction.y()) < 0.5)
    return;

  QVector2D move_offset = pan_direction / 2048.0f *  QVector2D (-1, 1);

  if (dimension == Dimension::Two)
    emit update_uniform (offset_uniform ("camera.position", move_offset * get_zoom_factor ()));

  if (dimension == Dimension::Three)
  {
    move_offset *= -1;
    auto [right, up, forward] = calculate_basis();
    forward = (forward + right * move_offset.x() + up * move_offset.y()).normalized();
    emit update_uniform (set_uniform ("camera.forward", forward));
  }

  pan_direction = QVector2D (0.0f, 0.0f);
}

void Camera::zoom_camera ()
{
  if (qAbs (zoom_offset) == 0) return;

  float zoom_delta = zoom_offset / 10.0f;
  Uniform &uniform = uniforms["camera.zoom"];
  uniform.offset_value (zoom_delta, 0);

  if (qAbs (zoom_delta) < 0.0000001f) zoom_offset = 0;
  else zoom_offset -= zoom_delta;

  emit update_uniform (uniform);
}

void Camera::uniforms_updated (QMap <QString, Uniform> const &p_uniforms)
{
  dimension = Dimension::Zero;

  for (QString const &key : p_uniforms.keys ())
    if (key.contains ("camera"))
      uniforms[key] = p_uniforms[key];

  if (uniforms.contains ("camera.position"))
  {
    move_direction = {0.0, 0.0, 0.0};
    Uniform uniform = uniforms ["camera.position"];
    if (uniform.size () == 2) dimension = Dimension::Two;
    if (uniform.size () == 3) dimension = Dimension::Three;
  }
  else
  {
    std::cerr << "Camera position was not found.";
  }
}

void Camera::uniform_updated (Uniform const &uniform)
{
  if (uniforms.contains (uniform.get_name()))
    uniforms[uniform.get_name()] = uniform;
}

Uniform Camera::offset_uniform (QString const &name, QVector3D const &values)
{
  Uniform &uniform = uniforms[name];
  for (unsigned int i = 0; i < uniform.size(); ++i)
    uniform.offset_value (values[i], i);

  return uniform;
}

Uniform Camera::set_uniform (QString const &name, QVector3D const &values)
{
  Uniform &uniform = uniforms[name];
  for (unsigned int i = 0; i < uniform.size(); ++i)
    uniform.set_value (values[i], i);

  return uniform;
}

float Camera::get_zoom_factor ()
{
  float zoom = uniforms["camera.zoom"].get_value(0).toFloat();
  return 1.0f - qAbs(zoom);
}

QVector3D Camera::get_forward_vector ()
{
  QVector3D forward;
  Uniform &uniform = uniforms["camera.forward"];
  for (unsigned int i = 0; i < uniform.size(); ++i)
    forward[i] = uniform.get_value (i).toFloat();

  return forward;
}

std::tuple <QVector3D, QVector3D, QVector3D> Camera::calculate_basis ()
{
  QVector3D world_up = QVector3D (0.0f, 1.0f, 0.0);
  QVector3D forward = get_forward_vector().normalized();
  QVector3D right = QVector3D::crossProduct(world_up, forward).normalized();
  QVector3D up = QVector3D::crossProduct(forward, right).normalized();

  return { right, up, forward };
}
