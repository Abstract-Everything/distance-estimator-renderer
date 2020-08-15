#include "uniform.hpp"

#include <QDebug>

Uniform::Uniform(QString const &name, QList<QVariant> const &values)
	: m_name(name)
	, m_values(values)
{
	assert(values.size() > 0);
	for (size_t i = 1; i < values.size(); ++i)
	{
		assert(values[i - 1].type() == values[i].type());
	}

	m_type = static_cast<QMetaType::Type>(values[0].type());
}

Uniform::Uniform(renderer::Uniform const& uniform)
{
	m_name = QString::fromStdString(uniform.get_name());

	if (typeid (uniform).hash_code() == typeid (renderer::Typed_Uniform<int>).hash_code())
	{
		m_type = QMetaType::Type::Int;
		fill_values <int>(uniform);
	}
	else if (typeid (uniform).hash_code() == typeid (renderer::Typed_Uniform<unsigned int>).hash_code())
	{
		m_type = QMetaType::Type::UInt;
		fill_values <unsigned int>(uniform);
	}
	else if (typeid (uniform).hash_code() == typeid (renderer::Typed_Uniform<float>).hash_code())
	{
		m_type = QMetaType::Type::Float;
		fill_values <float>(uniform);
	}
	else if (typeid (uniform).hash_code() == typeid (renderer::Typed_Uniform<double>).hash_code())
	{
		m_type = QMetaType::Type::Double;
		fill_values <double>(uniform);
	}
}

Uniform::operator std::unique_ptr<renderer::Uniform>() const
{
	std::string uniform_name = m_name.toStdString();

	if (m_type == QMetaType::Type::Int)
	{
		return create_typed_uniform<int>(
			uniform_name,
			[](QVariant const& value) { return value.toInt(); }
		);
	}
	else if (m_type == QMetaType::Type::UInt)
	{
		return create_typed_uniform<unsigned int>(
			uniform_name,
			[](QVariant const& value) { return value.toUInt(); }
		);
	}
	else if (m_type == QMetaType::Type::Float)
	{
		return create_typed_uniform<float>(
			uniform_name,
			[](QVariant const& value) { return value.toFloat(); }
		);
	}
	else if (m_type == QMetaType::Type::Double)
	{
		return create_typed_uniform<double>(
			uniform_name,
			[](QVariant const& value) { return value.toDouble(); }
		);
	}
	else
	{
		assert(false && "Uniforms should only be one of: bool, int, uint, float, double\n");
		return nullptr;
	}
}

QString Uniform::name() const
{
	return m_name;
}

QMetaType::Type Uniform::type() const
{
	return m_type;
}

size_t Uniform::size() const
{
	return m_values.size();
}

void Uniform::update(Uniform const& uniform)
{
	for (int i = 0; i < uniform.size(); ++i)
	{
		set_value(uniform.m_values[i], i);
	}
}

QVariant Uniform::value(unsigned int index) const
{
	return m_values[index];
}

void Uniform::set_value(QVariant const& value, unsigned int index)
{
	switch (value.type())
	{
	case QMetaType::Type::Int:
		m_values[index] = value.toInt();
		break;

	case QMetaType::Type::UInt:
		m_values[index] = value.toUInt();
		break;

	case QMetaType::Type::Float:
		m_values[index] = value.toFloat();
		break;

	case QMetaType::Type::Double:
		m_values[index] = value.toDouble();
		break;
	}
}

bool Uniform::is_type_compatabile(QVariant const &value)
{
	const bool compatabile = (static_cast<int>(m_type) == static_cast<int>(value.type()));
	if (!compatabile)
	{
		qDebug() << "Tried setting a " << m_name << "'s value with a different type.\n"
			<< "setting: " << m_values << "by: " << value << "\n";
	}
	return compatabile;
}

template <typename T>
void Uniform::fill_values(renderer::Uniform const& p_uniform)
{
	const renderer::Typed_Uniform<T>* uniform =
		dynamic_cast<const renderer::Typed_Uniform<T>*> (&p_uniform);

	for (const T value : uniform->get_values())
	{
		m_values.push_back(value);
	}
}

template <typename T>
std::unique_ptr <renderer::Typed_Uniform<T>> Uniform::create_typed_uniform(
	std::string const& uniform_name,
	std::function<T(QVariant)> const& convert
) const
{
	std::vector<T> uniform_values;
	for (const QVariant value : m_values)
	{
		uniform_values.push_back(convert(value));
	}
	return std::make_unique<renderer::Typed_Uniform<T>>(uniform_name, uniform_values);
}
