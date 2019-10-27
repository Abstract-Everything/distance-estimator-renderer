#include "inspector.hpp"

#include <QtMath>

#include <iostream>
#include <limits>

const int decimal_points = 6;

void Inspector::set_shader_list (QStringList const &p_shader_names)
{
  shader_names = p_shader_names;
  emit shader_list_changed ();
}

void Inspector::set_value_bool (QString const &variable_name, unsigned int index, bool value)
{
  change_uniform_value (variable_name, index, value);
}

void Inspector::set_value_int (QString const &variable_name, unsigned int index, int value)
{
  change_uniform_value (variable_name, index, value);
}

void Inspector::set_value_uint (QString const &variable_name, unsigned int index, uint value)
{
  change_uniform_value (variable_name, index, value);
}

void Inspector::set_value_float (QString const &variable_name, unsigned int index, float value)
{
  change_uniform_value (variable_name, index, value);
}

void Inspector::set_value_double (QString const &variable_name, unsigned int index, double value)
{
  change_uniform_value (variable_name, index, value);
}

void Inspector::change_uniform_value (QString const &variable_name, unsigned int index, QVariant value)
{
  if (uniforms.find (variable_name) == uniforms.end())
  {
    std::cerr << "Tried setting a non existing uniform variable.";
    return;
  }

  Uniform &uniform = uniforms[variable_name];
  uniform.set_value (value, index);
  emit update_uniform (uniform);
}

void Inspector::uniforms_updated (QMap <QString, Uniform> const &p_uniforms)
{
  uniforms = p_uniforms;
  emit uniforms_changed ();
}

void Inspector::uniform_updated (Uniform const &uniform)
{
  if (!uniforms.contains (uniform.get_name())) return;
  for (unsigned int i = 0; i < uniform.size(); ++i)
  {
    QObject *object = findChild<QObject *>(create_id (uniform, i));
    switch (uniform.get_type())
    {
      case QMetaType::Type::Bool:
        object->setProperty ("checked",
            uniform.get_value(i).toBool() ? "true" : "false"); break;

      case QMetaType::Type::Int:
      case QMetaType::Type::UInt:
      case QMetaType::Type::Float:
      case QMetaType::Type::Double:
        object->setProperty ("value", get_number_settings (uniform, i).value); break;

      default: qDebug() << "Uniform: " << uniform.get_name() << " should not have a non basic type\n"; break;
    }
  }
}

QMap <QString, QList <Uniform>> Inspector::group_uniforms (QMap <QString, Uniform> const &p_uniforms)
{
  QMap <QString, QList <Uniform>> uniforms;
  for (QString const &uniform : p_uniforms.keys())
  {
    QString tab_name = uniform.contains ('.') ?
      uniform.section ('.', 0, 0) : "Unnamed";
    uniforms[tab_name].push_back (p_uniforms[uniform]);
  }

  return uniforms;
}

QStringList Inspector::get_uniforms_source ()
{
  QMap <QString, QList <Uniform>> grouped_uniforms = group_uniforms (uniforms);
  
  QStringList tabs_source;
  for (QString const &tab_name : grouped_uniforms.keys())
    tabs_source.push_back (create_tab (tab_name, grouped_uniforms[tab_name]));

  return tabs_source;
}

QString Inspector::create_tab (QString const &name, QList <Uniform> const & uniforms) 
{
  unsigned int most_columns = 0;
  QString variable_fields;
  for (int i = 0; i < uniforms.size(); ++i)
  {
    most_columns = qMax (most_columns, uniforms[i].size());
    variable_fields += create_variable (uniforms[i], i);
  }

  return QStringLiteral (R"QML(
    import QtQml 2.0
    import QtQuick 2.4
    import QtQuick.Controls 1.4
    import QtQuick.Controls 2.4
    import QtQuick.Layouts 1.0

    Tab
    {
      title: "%1"

      GroupBox
      {
        GridLayout
        {
          columns: %2

          %3
        }
      }
    })QML")
  .arg (name)
  .arg (QString::number(most_columns + 1))
  .arg (variable_fields);
}

QString Inspector::create_variable (Uniform const &uniform, int row) 
{
  QString variable_source;

  for (unsigned int i = 0; i < uniform.size(); ++i)
  {
    QString const id = create_id (uniform, i);
    QString source;
    switch (uniform.get_type())
    {
      case QMetaType::Type::Bool: source = create_input_boolean (uniform, i); break;

      case QMetaType::Type::UInt:
      case QMetaType::Type::Int:
      case QMetaType::Type::Float:
      case QMetaType::Type::Double: source = create_input_number (uniform, i); break;

      default: qDebug() << "Uniform: " << uniform.get_name() << " should not have a non basic type\n"; break;
    }

    variable_source.append (source);
  }

  QString display_name = QString (uniform.get_name().contains ('.') ?
    uniform.get_name().section ('.', 1) : uniform.get_name())
    .replace ('_', ' ');

  return QString (R"QML(
        Text
        {
          Layout.row: %1
          text: "%2"
        }

        %3
  )QML")
  .arg (QString::number (row))
  .arg (display_name)
  .arg (variable_source);
}

QString Inspector::create_id (Uniform const &uniform, const unsigned int index) 
{
  return uniform.get_name().replace ('.', '_') + "_" + QString::number (index);
}
   
Inspector::number_settings Inspector::get_number_settings (Uniform const &uniform, unsigned int index)
{
  QVariant variant = uniform.get_value (index);
  int power = qPow (10, decimal_points);

  switch (uniform.get_type())
  {
    case QMetaType::Type::Int:
      return { variant.toInt(), true, 0, "set_value_int"};

    case QMetaType::Type::UInt:
      return { static_cast <int> (variant.toUInt()), false, 0, "set_value_uint"};

    case QMetaType::Type::Float:
      return { static_cast <int> (variant.toFloat() * power), true, decimal_points, "set_value_float"};

    case QMetaType::Type::Double:
      return { static_cast <int> (variant.toDouble() * power), true, decimal_points, "set_value_double"};

    default: assert (false && "Get display number was called for invalid types\n");
  }

  return {};
}

QString Inspector::create_input_boolean (Uniform const &uniform, int index)
{
  return QString (R"QML(
    CheckBox
    {
      id: %1
      objectName: "%1"

      checked: %4

      onClicked: inspector.set_value_bool ("%2", %3, checked);
    }
  )QML")
  .arg (create_id (uniform, index))
  .arg (uniform.get_name())
  .arg (index)
  .arg (uniform.get_value (index).toBool() ? "true" : "false");
}

QString Inspector::create_input_number (Uniform const &uniform, int index)
{
  auto [value, is_negative, decimals, function_name] = get_number_settings (uniform, index);
  return QString (R"QML(
    SpinBox
    {
      id: %1
      objectName: "%1"

      from:  %7
      to:    %8
      stepSize: Math.sqrt (float_factor)

      value: %5
      editable: true

      property int decimals: %6
      property int float_factor: Math.pow (10, decimals)

      validator: %9
      {
        bottom: %1.from
        top:  %1.to
      }

      textFromValue: function (value, locale)
      {
        return Number (value / float_factor).toLocaleString (locale, 'f', decimals);
      }

      valueFromText: function (text, locale)
      {
        return Number.fromLocaleString (locale, text) * float_factor;
      }

      onValueChanged: inspector.%2 ("%3", %4, value / float_factor);
    }
  )QML")
  .arg (create_id (uniform, index))
  .arg (function_name)
  .arg (uniform.get_name())
  .arg (index)
  .arg (value)
  .arg (decimals)
  .arg (is_negative ? std::numeric_limits<int>::min() : 0)
  .arg (std::numeric_limits<int>::max())
  .arg (decimals != 0 ? "DoubleValidator" : "IntValidator");
}

QStringList Inspector::get_shader_names ()
{
  return shader_names;
}
