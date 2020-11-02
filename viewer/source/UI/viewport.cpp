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
	connect (window(), &QQuickWindow::beforeRendering, this, &Viewport::render);
	return new Viewport_Renderer (window());
}

void Viewport::set_screen_input(QObject* qobject)
{
	Screen_Input* screen_input_ = dynamic_cast<Screen_Input*> (qobject);
	assert (
		screen_input_ != nullptr
		&& "Tried setting screen input with an invalid object");
	screen_input = screen_input_;

	connect (
		screen_input,
		&Screen_Input::input_updated,
		this,
		&Viewport::render);
}

void Viewport::render()
{
	Camera_Screen_Input input = camera_screen_input();
	screen_input->reset_input();
	camera.update_uniforms (input);

	if (Singletons::renderer().do_shader_settings_need_updating())
	{
		update();
	}
}

Camera_Screen_Input Viewport::camera_screen_input()
{
	Camera_Screen_Input input = Camera_Screen_Input (
		screen_input->move_keys(),
		screen_input->pan_direction(),
		screen_input->zoom_direction(),
		static_cast<float> (width()),
		static_cast<float> (height()));
	return input;
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
	Singletons::renderer().update_shader_settings();
}

void Viewport_Renderer::render()
{
	QPoint resolution{
		static_cast<int>(framebufferObject()->width()),
		static_cast<int>(framebufferObject()->height())};

	Singletons::renderer().render (resolution);
	window->resetOpenGLState();
}
