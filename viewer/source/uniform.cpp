#include "uniform.hpp"

#include <QDebug>

#include <iostream>

Uniform::Uniform () : type (QMetaType::Type::UnknownType) {}

Uniform::Uniform (renderer::Uniform const &p_uniform)
{
  name = QString::fromStdString (p_uniform.get_name ());

  if (typeid (p_uniform).hash_code() == typeid (renderer::Typed_Uniform<bool>).hash_code())
  {
    type = QMetaType::Type::Bool;
    fill_values <bool> (p_uniform);
  }

  else if (typeid (p_uniform).hash_code() == typeid (renderer::Typed_Uniform<int>).hash_code())
  {
    type = QMetaType::Type::Int;
    fill_values <int> (p_uniform);
  }

  else if (typeid (p_uniform).hash_code() == typeid (renderer::Typed_Uniform<unsigned int>).hash_code())
  {
    type = QMetaType::Type::UInt;
    fill_values <unsigned int> (p_uniform);
  }

  else if (typeid (p_uniform).hash_code() == typeid (renderer::Typed_Uniform<float>).hash_code())
  {
    type = QMetaType::Type::Float;
    fill_values <float> (p_uniform);
  }

  else if (typeid (p_uniform).hash_code() == typeid (renderer::Typed_Uniform<double>).hash_code())
  {
    type = QMetaType::Type::Double;
    fill_values <double> (p_uniform);
  }
}

std::unique_ptr <renderer::Uniform> Uniform::get_as_renderer_uniform ()
{
  std::string uniform_name = name.toStdString();

  const auto to_bool   = [](QVariant const &value) { return value.toBool();   };
  const auto to_int    = [](QVariant const &value) { return value.toInt();    };
  const auto to_uint   = [](QVariant const &value) { return value.toUInt();   };
  const auto to_float  = [](QVariant const &value) { return value.toFloat();  };
  const auto to_double = [](QVariant const &value) { return value.toDouble(); };

  if (type == QMetaType::Type::Bool)   return get_typed_uniform <bool> (uniform_name, to_bool);
  if (type == QMetaType::Type::Int)    return get_typed_uniform <int> (uniform_name, to_int);
  if (type == QMetaType::Type::UInt)   return get_typed_uniform <unsigned int> (uniform_name, to_uint);
  if (type == QMetaType::Type::Float)  return get_typed_uniform <float> (uniform_name, to_float);
  if (type == QMetaType::Type::Double) return get_typed_uniform <double> (uniform_name, to_double);

  assert (false && "Uniforms should only be one of: bool, int, uint, float, double\n");
  return nullptr;
}

QString Uniform::get_name () const
{
  return name;
}

QMetaType::Type Uniform::get_type () const
{
  return type;
}

unsigned int Uniform::size () const
{
  return values.size();
}

QVariant Uniform::get_value (unsigned int index) const
{
  return values[index];
}

void Uniform::set_value (QVariant const &value, unsigned int index)
{
  if (!is_type_compatabile (value)) return;
  values[index] = value;
}

void Uniform::offset_value (QVariant const &value, unsigned int index)
{
  if (!is_type_compatabile (value)) return;

  QVariant &variant = values[index];
  switch (type)
  {
    case QMetaType::Type::Int:    variant = variant.toInt() + value.toInt();       break;
    case QMetaType::Type::UInt:   variant = variant.toUInt() + value.toUInt();     break;
    case QMetaType::Type::Float:  variant = variant.toFloat() + value.toFloat();   break;
    case QMetaType::Type::Double: variant = variant.toDouble() + value.toDouble(); break;
    default: qDebug() << "Cannot offset a boolean value.";
  }
}

template <typename T>
void Uniform::fill_values (renderer::Uniform const &p_uniform)
{
  const renderer::Typed_Uniform<T> *uniform =
    dynamic_cast<const renderer::Typed_Uniform<T> *> (&p_uniform);

  for (const T value: uniform->get_values())
    values.push_back (value);
}

template <typename T>
std::unique_ptr <renderer::Typed_Uniform<T>> Uniform::get_typed_uniform (
    std::string const &uniform_name,
    std::function<T(QVariant)> const &convert
) {
  std::vector<T> uniform_values;
  for (const QVariant value: values) uniform_values.push_back (convert(value));
  return std::make_unique <renderer::Typed_Uniform<T>> (uniform_name, uniform_values);
}

bool Uniform::is_type_compatabile (QVariant const &value)
{
  bool compatabile = static_cast <int> (type) == static_cast <int> (value.type());
  if (!compatabile)
    qDebug() << "Tried setting a " << get_name() << "'s value with a different type.\n"
             << "setting: " << values << "by: " << value << "\n";
  return compatabile;
}
