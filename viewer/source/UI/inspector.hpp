#pragma once

#include "uniform.hpp"

#include <QtQuick/QQuickItem>
#include <QStringList>

#include <vector>
#include <filesystem>

class Inspector : public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY (QStringList shaders READ shaders)

public:
	Inspector();
	QStringList shaders();

	Q_INVOKABLE void update_shader(int index);
	Q_INVOKABLE QStringList create_uniforms_qml_source();
	Q_INVOKABLE void set_uniform_value(QString const& name, double value, quint32 index);

signals:
	void uniforms_changed();

public slots:
	void shader_updated();
	void uniform_updated(QString const& uniform_name);

private:
	QList<QString> shader_names;

	QMap<QString, QList<Uniform>> group_uniforms();

	QString create_tab(QString const &tab_name, QList<Uniform> const &uniforms);
	QString create_uniform_source(Uniform const& uniform, size_t row);
	QString create_uniform_field(Uniform const& uniform, size_t column);

	QString create_number_input(
		Uniform const& uniform,
		size_t index,
		size_t decimal_points,
		int minimum = std::numeric_limits<int>::min(),
		int maximum = std::numeric_limits<int>::max()
	);

	int convert_value_for_input_field(
		QVariant const& value,
		size_t decimal_points
	);
	size_t get_decimal_points(QMetaType::Type type);

	QString create_id(Uniform const& uniform, const unsigned int index);
};