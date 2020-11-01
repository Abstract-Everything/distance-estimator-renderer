#pragma once

#include "uniform.hpp"

#include <renderer/renderer.hpp>

#include <QList>
#include <QMap>
#include <QMutex>

#include <filesystem>

class Renderer : public QObject
{
	Q_OBJECT

public:
	Renderer();
	void initialise();

	QList<QString> get_shaders();
	void           set_shader (QString const& shader_name);

	bool           exists_uniform (QString const& name);
	QList<Uniform> get_uniforms();
	Uniform        get_uniform (QString const& name);
	void           set_uniform (Uniform const& uniform);

	void update_shader_settings();
	void render (QPoint const& resolution);

signals:
	void update_shader_list();
	void update_shader();
	void update_uniform (QString const& uniform_name);

private:
	const std::filesystem::path glsl;
	QString                     shader_name_to_set;

	void init_shaders();

	QMutex m_mutex;

	QMap<QString, std::filesystem::path> m_shaders;
	QMap<QString, Uniform>               m_uniforms;
	std::unique_ptr<renderer::Renderer>  m_renderer_wrapper = nullptr;

	void set_new_shader();
	void update_uniforms();
};