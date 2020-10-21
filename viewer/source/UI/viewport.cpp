#include "viewport.hpp"

#include "renderer.hpp"
#include "singletons.hpp"

#include <QString>
#include <QStringList>

#include <filesystem>
#include <iostream>

Viewport::Viewport (QQuickItem* object) : QQuickFramebufferObject (object)
{
	setMirrorVertically (true);
	setTextureFollowsItemSize (true);
}

QQuickFramebufferObject::Renderer* Viewport::createRenderer() const
{
	connect (window(), &QQuickWindow::beforeRendering, this, &Viewport::update);
	return new Viewport_Renderer (window());
}

void Viewport::set_screen_input(QObject* qobject)
{
	Screen_Input* screen_input_ = dynamic_cast<Screen_Input*> (qobject);
	assert (
		screen_input_ != nullptr
		&& "Tried setting screen input with an invalid object");
	screen_input = screen_input_;
}

Camera_Screen_Input Viewport::get_and_reset_camera_screen_input()
{
	return Camera_Screen_Input (
		screen_input->move_keys(),
		screen_input->get_and_reset_pan_direction(),
		screen_input->get_and_reset_zoom_direction(),
		static_cast<float> (width()),
		static_cast<float> (height()));
}

Viewport_Renderer::Viewport_Renderer (QQuickWindow* window)
	: QQuickFramebufferObject::Renderer()
	, window (window)
{
	// ToDo: Figure out how to move this call into main
	Singletons::renderer().initialise();
}

QOpenGLFramebufferObject*
Viewport_Renderer::createFramebufferObject (QSize const& size)
{
	QOpenGLFramebufferObjectFormat format;
	format.setAttachment (QOpenGLFramebufferObject::CombinedDepthStencil);
	return new QOpenGLFramebufferObject (size, format);
}

void Viewport_Renderer::synchronize (QQuickFramebufferObject* quick_fbo)
{
	Viewport* viewport = static_cast<Viewport*> (quick_fbo);

	float delta_time = static_cast<float> (m_timer.elapsed()) / 1000.0f;
	m_timer.restart();

	Camera_Screen_Input camera_screen_input
		= viewport->get_and_reset_camera_screen_input();

	camera.update_uniforms (delta_time, camera_screen_input);
	update();
}

void Viewport_Renderer::render()
{
	if (Singletons::renderer().exists_uniform ("globals.resolution"))
	{
		const unsigned int width
			= static_cast<unsigned int> (framebufferObject()->width());
		const unsigned int height
			= static_cast<unsigned int> (framebufferObject()->height());
		Singletons::renderer().set_uniform (Uniform (
			"globals.resolution",
			Uniform::Type::UInt,
			{width, height}));
	}

	Singletons::renderer().render();
	window->resetOpenGLState();
}
