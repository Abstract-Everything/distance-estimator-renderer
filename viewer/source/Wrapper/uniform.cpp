#include "uniform.hpp"

#include <QDebug>

Uniform::Uniform (QString const& name, Type type, QList<QVariant> const& values)
	: m_name (name)
	, m_type (type)
{
	assert (type != Type::Invalid && "Invalid uniform initailization");
	assert (values.size() > 0);
	for (int i = 0; i < values.size(); ++i)
	{
		switch (type)
		{
		case Type::Int:    m_values.push_back (values[i].toInt());    break;
		case Type::UInt:   m_values.push_back (values[i].toUInt());   break;
		case Type::Float:  m_values.push_back (values[i].toFloat());  break;
		case Type::Double: m_values.push_back (values[i].toDouble()); break;

		case Type::Invalid: assert(false); break;
		}
	}
}

Uniform::Uniform (renderer::Uniform const& uniform)
{
	m_name = QString::fromStdString (uniform.get_name());

	if (typeid (uniform).hash_code()
		== typeid (renderer::Typed_Uniform<int>).hash_code())
	{
		m_type = Type::Int;
		fill_values<int> (uniform);
	}
	else if (
		typeid (uniform).hash_code()
		== typeid (renderer::Typed_Uniform<unsigned int>).hash_code())
	{
		m_type = Type::UInt;
		fill_values<unsigned int> (uniform);
	}
	else if (
		typeid (uniform).hash_code()
		== typeid (renderer::Typed_Uniform<float>).hash_code())
	{
		m_type = Type::Float;
		fill_values<float> (uniform);
	}
	else if (
		typeid (uniform).hash_code()
		== typeid (renderer::Typed_Uniform<double>).hash_code())
	{
		m_type = Type::Double;
		fill_values<double> (uniform);
	}
	else
	{
		m_type = Type::Invalid;
		assert (false && "Uniform has an unsupported type.");
	}
}

Uniform::operator std::unique_ptr<renderer::Uniform>() const
{
	std::string uniform_name = m_name.toStdString();

	switch (m_type)
	{
	case Type::Int:
		return create_typed_uniform<int> (
			uniform_name,
			[] (QVariant const& value) { return value.toInt(); });

	case Type::UInt:
		return create_typed_uniform<unsigned int> (
			uniform_name,
			[] (QVariant const& value) { return value.toUInt(); });

	case Type::Float:
		return create_typed_uniform<float> (
			uniform_name,
			[] (QVariant const& value) { return value.toFloat(); });

	case Type::Double:
		return create_typed_uniform<double> (
			uniform_name,
			[] (QVariant const& value) { return value.toDouble(); });

	case Type::Invalid:
		assert (
			false
			&& "Uniforms should only be one of: int, uint, float, double\n");
		return nullptr;
	}
	return nullptr;
}

QString Uniform::name() const
{
	return m_name;
}

Uniform::Type Uniform::type() const
{
	return m_type;
}

int Uniform::size() const
{
	return m_values.size();
}

void Uniform::update (Uniform const& uniform)
{
	for (int i = 0; i < uniform.size(); ++i)
	{
		set_value (uniform, i);
	}
}

QVariant Uniform::value (int index) const
{
	return m_values[index];
}

void Uniform::set_value (Uniform const& uniform, unsigned int index)
{
	assert (
		uniform.type() == type()
		&& "Setting uniform value with a different type.");
	m_values[index] = uniform.m_values[index];
}

void Uniform::set_value (int value, unsigned int index)
{
	assert (type() == Type::Int);
	m_values[index] = value;
}

void Uniform::set_value (unsigned int value, unsigned int index)
{
	assert (type() == Type::UInt);
	m_values[index] = value;
}

void Uniform::set_value (float value, unsigned int index)
{
	assert (type() == Type::Float);
	m_values[index] = value;
}

void Uniform::set_value (double value, unsigned int index)
{
	assert (type() == Type::Double);
	m_values[index] = value;
}

bool Uniform::is_type_compatabile (QVariant const& value)
{
	const bool compatabile
		= (static_cast<int> (m_type) == static_cast<int> (value.type()));
	if (!compatabile)
	{
		qDebug() << "Tried setting a " << m_name
				 << "'s value with a different type.\n"
				 << "setting: " << m_values << "by: " << value << "\n";
	}
	return compatabile;
}

template <typename T>
void Uniform::fill_values (renderer::Uniform const& p_uniform)
{
	const renderer::Typed_Uniform<T>* uniform
		= dynamic_cast<const renderer::Typed_Uniform<T>*> (&p_uniform);

	for (const T value : uniform->get_values())
	{
		m_values.push_back (value);
	}
}

template <typename T>
std::unique_ptr<renderer::Typed_Uniform<T>> Uniform::create_typed_uniform (
	std::string const&                 uniform_name,
	std::function<T (QVariant)> const& convert) const
{
	std::vector<T> uniform_values;
	for (const QVariant& value : m_values)
	{
		uniform_values.push_back (convert (value));
	}
	return std::make_unique<renderer::Typed_Uniform<T>> (
		uniform_name,
		uniform_values);
}
