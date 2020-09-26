#include "viewport.hpp"

#include "renderer.hpp"

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
	return new Viewport_Renderer (window());
}

Viewport_Renderer::Viewport_Renderer (QQuickWindow* window)
	: QQuickFramebufferObject::Renderer()
	, window (window)
{
	Singletons::renderer.init_renderer();
}

QOpenGLFramebufferObject*
Viewport_Renderer::createFramebufferObject (QSize const& size)
{
	QOpenGLFramebufferObjectFormat format;
	format.setAttachment (QOpenGLFramebufferObject::CombinedDepthStencil);
	return new QOpenGLFramebufferObject (size, format);
}

void Viewport_Renderer::render()
{
	const unsigned int width
		= static_cast<unsigned int> (framebufferObject()->width());
	const unsigned int height
		= static_cast<unsigned int> (framebufferObject()->height());
	Singletons::renderer.set_uniform (
		Uniform ("globals.resolution", {width, height}));

	Singletons::renderer.render();
	window->resetOpenGLState();
	update();
}
