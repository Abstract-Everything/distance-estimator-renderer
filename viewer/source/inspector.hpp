#pragma once

#include "uniform.hpp"

#include <QtQuick/QQuickItem>
#include <QStringList>

class Inspector : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY (QStringList shaders READ get_shader_names NOTIFY shader_list_changed)
  Q_PROPERTY (QStringList uniforms_source READ get_uniforms_source NOTIFY uniforms_changed)

public:
  QStringList get_shader_names ();
  QStringList get_uniforms_source ();
  Q_INVOKABLE void set_value_bool   (QString const &variable_name, unsigned int index, bool value);
  Q_INVOKABLE void set_value_int    (QString const &variable_name, unsigned int index, int value);
  Q_INVOKABLE void set_value_uint   (QString const &variable_name, unsigned int index, uint value);
  Q_INVOKABLE void set_value_float  (QString const &variable_name, unsigned int index, float value);
  Q_INVOKABLE void set_value_double (QString const &variable_name, unsigned int index, double value);

signals:
  void shader_list_changed ();
  void uniforms_changed ();

  void shader_selection_changed (QString shader_path);
  void update_uniform (Uniform const &uniform);

public slots:
  void set_shader_list (QStringList const &shader_paths);
  void uniforms_updated (QMap <QString, Uniform> const &p_uniforms);
  void uniform_updated (Uniform const &uniform);

private:
  struct number_settings;

  QStringList shader_names;
  QMap <QString, Uniform> uniforms;

  QMap <QString, QList <Uniform>> group_uniforms (QMap <QString, Uniform> const &p_uniforms);

  QString create_tab (QString const &name, QList <Uniform> const & uniforms);

  QString create_id (Uniform const &uniform, const unsigned int index);
  number_settings get_number_settings (Uniform const &uniform, unsigned int index);

  QString create_variable (Uniform const &uniform, int row);
  QString create_input_boolean (Uniform const &uniform, int index);
  QString create_input_number (Uniform const &uniform, int index);

  void change_uniform_value (QString const &variable_name, unsigned int index, QVariant value);
};

struct Inspector::number_settings
{
  int value = 0;
  bool is_negative = false;
  int decimals = 0;
  QString function_name;
};
