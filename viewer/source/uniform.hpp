#pragma once

#include "renderer/uniform.hpp"

#include <QString>
#include <QVariant>
#include <QMetaType>

#include <memory>
#include <string>

class Uniform
{
public:
  Uniform ();
  Uniform (renderer::Uniform const &p_uniform);
  std::unique_ptr <renderer::Uniform> get_as_renderer_uniform ();

  QString get_name () const;
  QMetaType::Type get_type () const;
  unsigned int size () const;

  QVariant get_value (unsigned int index) const;
  void set_value (QVariant const &value, unsigned int index);
  void offset_value (QVariant const &value, unsigned int index);

private:
  QString name = "";
  QMetaType::Type type;
  QList <QVariant> values {};

  template <typename T>
  void fill_values (renderer::Uniform const &p_uniform);
  template <typename T>
  std::unique_ptr <renderer::Typed_Uniform<T>> get_typed_uniform (
      std::string const &uniform_name,
      std::function<T(QVariant)> const &convert);
  bool is_type_compatabile (QVariant const &value);
};
