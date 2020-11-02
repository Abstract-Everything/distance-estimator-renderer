#pragma once

#include "camera_controller.hpp"
#include "screen_input.hpp"

#include <QOpenGLFramebufferObjectFormat>
#include <QtQuick/QQuickFramebufferObject>
#include <QtQuick/QQuickWindow>

#include <map>
#include <string>
#include <vector>

class Viewport : public QQuickFramebufferObject
{
	Q_OBJECT

public:
	Viewport (QQuickItem* object = nullptr);
	QQuickFramebufferObject::Renderer* createRenderer() const override;

	Q_INVOKABLE void set_screen_input (QObject* qobject);

private:
	Screen_Input*     screen_input = nullptr;
	Camera_Controller camera;

	void                render();
	Camera_Screen_Input camera_screen_input();
};

class Viewport_Renderer : public QQuickFramebufferObject::Renderer
{
public:
	Viewport_Renderer (QQuickWindow* window);

protected:
	QOpenGLFramebufferObject*
	createFramebufferObject (QSize const& size) override;

	void synchronize (QQuickFramebufferObject* quick_fbo) override;

	void render() override;

private:
	QQuickWindow* window;
};
