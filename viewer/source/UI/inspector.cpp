#include "inspector.hpp"

#include "renderer.hpp"

#include <QtMath>
#include <QList>

Inspector::Inspector()
{
	Singletons::renderer.init_shaders();
	connect(&Singletons::renderer, &Singletons::Renderer::update_shader, this, &Inspector::shader_updated);
	connect(&Singletons::renderer, &Singletons::Renderer::update_uniform, this, &Inspector::uniform_updated);
}

QStringList Inspector::shaders()
{
	shader_names = Singletons::renderer.get_shaders();

	int index = shader_names.indexOf("flat_background");
	if (index != -1)
	{
		QString& first_shader = shader_names[0];
		shader_names[0] = shader_names[index];
		shader_names[index] = first_shader;
		update_shader(0);
	}

	return shader_names;
}

void Inspector::update_shader(int index)
{
	if (index >= shader_names.size() || index < 0)
	{
		qDebug() << "Inspector shader selection index out of range";
		return;
	}

	Singletons::renderer.set_shader(shader_names[index]);
}

QStringList Inspector::create_uniforms_qml_source()
{
    QStringList tabs_source;
    QMap<QString, QList<Uniform>> grouped_uniforms = group_uniforms();
    for (QString const &tab_name : grouped_uniforms.keys())
    {
        QString tab_source = create_tab(tab_name, grouped_uniforms[tab_name]);
        tabs_source.push_back(tab_source);
    }
    return tabs_source;
}

void Inspector::set_uniform_value(
    QString const &name,
    double value,
    quint32 index
)
{
	Uniform uniform = Singletons::renderer.get_uniform(name);
	uniform.set_value(value, index);
    Singletons::renderer.set_uniform(uniform);
}

void Inspector::shader_updated()
{
    emit uniforms_changed();
}

void Inspector::uniform_updated(QString const& uniform_name)
{
	Uniform const& uniform = Singletons::renderer.get_uniform(uniform_name);
	for (unsigned int i = 0; i < uniform.size(); ++i)
	{
		QObject* object = findChild<QObject*>(create_id(uniform, i));
		if (object == nullptr)
		{
			return;
		}

		const size_t decimal_points = get_decimal_points(uniform.type());
		int value = convert_value_for_input_field(uniform.value(i), decimal_points);
		object->setProperty("value", value);
	}
}

QMap<QString, QList<Uniform>> Inspector::group_uniforms()
{
    QMap<QString, QList<Uniform>> grouped_uniforms;
    for (Uniform const& uniform : Singletons::renderer.get_uniforms())
    {
		const QString tab_name = uniform.name().contains ('.') ?
		  uniform.name().section ('.', 0, 0) : "Unnamed";

		const QString formatted_tab_name = QString(tab_name).replace('_', ' ');
		grouped_uniforms[formatted_tab_name].push_back(uniform);
    }
    return grouped_uniforms;
}

QString Inspector::create_tab(QString const &tab_name, QList<Uniform> const &uniforms)
{
    size_t max_column = 0;
    QString uniform_fields;
    for (int i = 0; i < uniforms.size(); ++i)
    {
        max_column = qMax(max_column, uniforms[i].size());
        uniform_fields += create_uniform_source(uniforms[i], i);
    }

    return QStringLiteral(R"QML(
	import QtQuick 2.4
    import QtQuick.Controls 1.4
    import QtQuick.Controls 2.4
	import QtQuick.Layouts 1.0

    Tab
    {
		title: "%1"
        width: implicitWidth

		GroupBox
		{
			GridLayout
			{
                width: parent.width

				columns: %2
				
				%3
			}
		}
    })QML")
        .arg(tab_name)
        .arg(QString::number(max_column + 1))
        .arg(uniform_fields);
}

QString Inspector::create_uniform_source(Uniform const& uniform, size_t row)
{
	QString uniform_source;
	for (size_t i = 0; i < uniform.size(); ++i)
	{
		uniform_source.append(create_uniform_field(uniform, i));
	}

    const QString uniform_name = uniform.name().contains('.') ?
        uniform.name().section('.', 1) :
        uniform.name();

	QString display_name = QString(uniform_name).replace('_', ' ');

    return QString(R"QML(
        Text
        {
            Layout.row: %1
            text: "%2"
        }

		%3
	)QML")
        .arg(QString::number(row))
        .arg(display_name)
        .arg(uniform_source);
}
   
QString Inspector::create_uniform_field(Uniform const &uniform, size_t column)
{
	QString source;
	if (uniform.type() == QMetaType::Type::Int)
	{
		source = create_number_input(uniform, column, 0);
	}
	else if (uniform.type() == QMetaType::Type::UInt)
	{
		source = create_number_input(uniform, column, 0, 0);
	}
	else if (uniform.type() == QMetaType::Type::Float
		|| uniform.type() == QMetaType::Type::Double)
	{
		const size_t decimal_points = get_decimal_points(uniform.type());
		source = create_number_input(uniform, column, decimal_points);
	}
	return source;
}

QString Inspector::create_number_input(
    Uniform const &uniform,
    size_t index,
    size_t decimal_points,
    int minimum,
    int maximum
)
{
	return QString(R"QML(
		SpinBox
		{
			id: %1
			objectName: "%1"

			from:  %6
			to:    %7
			stepSize: Math.sqrt (float_factor) * 10

			value: %4
			editable: true

			property int decimals: %5
			property int float_factor: Math.pow (10, decimals)

			validator: %8
			{
				bottom: %1.from
				top:  %1.to
			}

			textFromValue: function (value, locale)
			{
				return Number(value / float_factor).toLocaleString (locale, 'f', decimals);
			}

			valueFromText: function (text, locale)
			{
				return Number.fromLocaleString (locale, text) * float_factor;
			}

			onValueChanged: inspector.set_uniform_value("%2", value / float_factor, %3);
		}
  )QML")
		.arg(create_id(uniform, index))
		.arg(uniform.name())
		.arg(index)
		.arg(convert_value_for_input_field(uniform.value(index), decimal_points))
		.arg(decimal_points)
		.arg(minimum)
		.arg(maximum)
		.arg(decimal_points != 0 ? "DoubleValidator" : "IntValidator");
}

QString Inspector::create_id(Uniform const &uniform, const unsigned int index)
{
	QString id_name = uniform.name().replace('.', '_');
	return id_name + "_" + QString::number(index);
}

int Inspector::convert_value_for_input_field(
	QVariant const& value,
	size_t decimal_points
)
{
	const double exponent = qPow(10, decimal_points);
	if (value.type() == QMetaType::Type::Int)
	{
		return value.toInt();
	}
	else if (value.type() == QMetaType::Type::UInt)
	{
		return static_cast<int>(value.toUInt());
	}
	else if (value.type() == QMetaType::Type::Float)
	{
		return static_cast<int>(value.toFloat() * exponent);
	}
	else if (value.type() == QMetaType::Type::Double)
	{
		return static_cast<int>(value.toDouble() * exponent);
	}
	else
	{
		qDebug() << "Number type is not supported";
		const double double_value = value.toDouble();
		return static_cast<int>(double_value * exponent);
	}
}

size_t Inspector::get_decimal_points(QMetaType::Type type)
{
	if (type == QMetaType::Type::Float
		|| type == QMetaType::Type::Double)
	{
		return 6;
	}
	return 0;
}