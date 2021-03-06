#include "renderer.hpp"

#include <renderer/renderer.hpp>

#include <QCoreApplication>
#include <QDebug>
#include <QMutexLocker>

namespace fs = std::filesystem;

namespace
{
fs::path find_glsl_path()
{
	fs::path build;
	fs::path current = QCoreApplication::applicationDirPath().toStdString();
	for (const fs::path& subpath : current)
	{
		std::string folder = subpath.string();

		std::transform (
			folder.begin(),
			folder.end(),
			folder.begin(),
			::tolower);

		if (folder == "bin")
		{
			break;
		}

		build.append (subpath.string());
	}

	return build / "glsl";
}
} // namespace

Renderer::Renderer() : glsl(find_glsl_path()) {}

void Renderer::initialise()
{
	QMutexLocker lock (&m_mutex);
	m_renderer_wrapper = std::make_unique<renderer::Renderer>();
	init_shaders();
}

void Renderer::init_shaders()
{
	const std::vector<fs::path> search_paths
		= {fs::path{glsl / "2d" / "signed_distance_functions"},
		   fs::path{glsl / "3d" / "signed_distance_functions"}};

	for (fs::path const& shader_path :
		 m_renderer_wrapper->get_shaders (glsl, search_paths))
	{
		std::string   std_name = shader_path.stem().string();
		const QString name     = QString::fromStdString (std_name);
		m_shaders[name]        = shader_path;
	}
	emit update_shader_list();
}

QList<QString> Renderer::get_shaders()
{
	QMutexLocker lock (&m_mutex);
	return m_shaders.keys();
}

void Renderer::set_shader (QString const& shader_name)
{
	{
		QMutexLocker lock (&m_mutex);
		shader_name_to_set = shader_name;
	}
}

Uniform Renderer::get_uniform (QString const& name)
{
	QMutexLocker lock (&m_mutex);
	return m_uniforms[name];
}

QList<Uniform> Renderer::get_uniforms()
{
	QMutexLocker lock (&m_mutex);
	return m_uniforms.values();
}

bool Renderer::exists_uniform(QString const& name)
{
	return m_uniforms.contains (name);
}

void Renderer::set_uniform (Uniform const& uniform)
{
	{
		QMutexLocker lock (&m_mutex);
		if (!m_uniforms.contains (uniform.name()))
		{
			qDebug() << "Tried setting a non existent uniform";
			return;
		}

		m_uniforms[uniform.name()].update (uniform);
		m_uniforms_to_update.insert (uniform.name());
	}
	emit update_uniform (uniform.name());
}

bool Renderer::do_shader_settings_need_updating()
{
	bool new_shader = !shader_name_to_set.isEmpty();
	bool update_uniform = !m_uniforms_to_update.empty();
	return new_shader || update_uniform;
}

void Renderer::update_shader_settings()
{
	QMutexLocker lock (&m_mutex);
	set_new_shader();
	update_uniforms();
}

void Renderer::render (QPoint const& resolution)
{
	QMutexLocker lock (&m_mutex);
	m_renderer_wrapper->render (resolution.x(), resolution.y());
}

void Renderer::set_new_shader()
{
	if (shader_name_to_set.isEmpty())
	{
		return;
	}

	m_uniforms.clear();
	m_uniforms_to_update.clear();
	const fs::path shader = m_shaders[shader_name_to_set];
	shader_name_to_set    = "";

	for (std::unique_ptr<renderer::Uniform>& uniform :
		 m_renderer_wrapper->set_shader (glsl, shader))
	{
		Uniform qt_uniform (*uniform);
		m_uniforms[qt_uniform.name()] = qt_uniform;
		m_uniforms_to_update.insert (qt_uniform.name());
	}
	emit update_shader();
}

void Renderer::update_uniforms()
{
	for (QString const& uniform_name : m_uniforms_to_update)
	{
		Uniform&                           uniform = m_uniforms[uniform_name];
		std::unique_ptr<renderer::Uniform> renderer_uniform{uniform};
		m_renderer_wrapper->set_uniform (*renderer_uniform);
	}
	m_uniforms_to_update.clear();
}
