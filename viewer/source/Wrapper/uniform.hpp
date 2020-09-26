#pragma once

#include <renderer/uniform.hpp>

#include <QMetaType>
#include <QString>
#include <QVariant>

#include <memory>

class Uniform
{
public:
	Uniform() = default;
	Uniform (QString const& name, QList<QVariant> const& values);
	Uniform (renderer::Uniform const& uniform);

	operator std::unique_ptr<renderer::Uniform>() const;

	QString         name() const;
	QMetaType::Type type() const;
	size_t          size() const;

	void     update (Uniform const& uniform);
	QVariant value (unsigned int index) const;
	void     set_value (QVariant const& value, unsigned int index);

private:
	QString         m_name;
	QMetaType::Type m_type = QMetaType::Type::UnknownType;
	QList<QVariant> m_values{};

	bool is_type_compatabile (QVariant const& value);

	template <typename T>
	void fill_values (renderer::Uniform const& p_uniform);

	template <typename T>
	std::unique_ptr<renderer::Typed_Uniform<T>> create_typed_uniform (
		std::string const&                 uniform_name,
		std::function<T (QVariant)> const& convert) const;
};