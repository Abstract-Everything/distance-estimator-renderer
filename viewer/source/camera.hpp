#pragma once

#include "uniform.hpp"

#include <QObject>
#include <QMap>
#include <QString>
#include <QKeyEvent>
#include <QVector3D>
#include <QVector2D>

class Camera : public QObject
{
  Q_OBJECT

public:
  void before_rendering ();

signals:
  void update_uniform (Uniform const &uniform);

public slots:
  void uniforms_updated (QMap <QString, Uniform> const &p_uniforms);
  void uniform_updated (Uniform const &uniform);

protected:
  bool eventFilter (QObject *watched, QEvent *event) override;

private:
  enum class Dimension { Zero, Two, Three };
  QMap <QString, Uniform> uniforms;

  double camera_speed = 0.001;
  Dimension dimension = Dimension::Zero;

  float zoom_offset = 0;
  QVector3D move_direction;
  QPoint last_mouse_position;
  QVector2D pan_direction;

  void update_pan (QMouseEvent const &event);
  void update_zoom (QWheelEvent const &event);
  void update_camera_speed (QKeyEvent const &event);
  void update_move_direction_2d (QKeyEvent const &event);
  void update_move_direction_3d (QKeyEvent const &event);

  void move_camera ();
  void pan_camera ();
  void zoom_camera ();

  Uniform offset_uniform (QString const &name, QVector3D const &values);
  Uniform set_uniform (QString const &name, QVector3D const &values);

  float get_zoom_factor ();
  QVector3D get_forward_vector ();
  std::tuple <QVector3D, QVector3D, QVector3D> calculate_basis ();
};
