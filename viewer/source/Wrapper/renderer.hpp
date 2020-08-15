#pragma once

#include "uniform.hpp"

#include <renderer/renderer.hpp>

#include <QMap>
#include <QList>
#include <QMutex>

#include <filesystem>

namespace Singletons
{
	class Renderer : public QObject
	{
		Q_OBJECT

	public:
		void init_renderer();
		void init_shaders();

		QList<QString> get_shaders();
		void set_shader(QString const& shader_name);

		QList<Uniform> get_uniforms();
		Uniform get_uniform(QString const& name);
		void set_uniform(Uniform const& uniform);

		void render();

	signals:
		void update_shader();
		void update_uniform(QString const& uniform_name);

	private:
		std::filesystem::path glsl;

		QMutex m_mutex;

		QMap<QString, std::filesystem::path> m_shaders;
		QMap<QString, Uniform> m_uniforms;
		renderer::Renderer m_renderer_wrapper;
	};

	extern Renderer renderer;
}