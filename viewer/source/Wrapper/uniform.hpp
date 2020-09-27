#pragma once

#include <renderer/uniform.hpp>

#include <QString>
#include <QVariant>

#include <memory>

class Uniform
{
public:
	enum class Type
	{
		Invalid,

		Int,
		UInt,
		Float,
		Double
	};

	Uniform() = default;
	Uniform (QString const& name, Type type, QList<QVariant> const& values);
	Uniform (renderer::Uniform const& uniform);

	operator std::unique_ptr<renderer::Uniform>() const;

	QString name() const;
	Type    type() const;
	int     size() const;

	void     update (Uniform const& uniform);
	QVariant value (int index) const;
	void     set_value (Uniform const& uniform, unsigned int index);
	void     set_value (int value, unsigned int index);
	void     set_value (unsigned int value, unsigned int index);
	void     set_value (float value, unsigned int index);
	void     set_value (double value, unsigned int index);

private:
	QString         m_name;
	Type            m_type = Type::Invalid;
	QList<QVariant> m_values{};

	bool is_type_compatabile (QVariant const& value);

	template <typename T>
	void fill_values (renderer::Uniform const& p_uniform);

	template <typename T>
	std::unique_ptr<renderer::Typed_Uniform<T>> create_typed_uniform (
		std::string const&                 uniform_name,
		std::function<T (QVariant)> const& convert) const;
};