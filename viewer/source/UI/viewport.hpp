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

	Camera_Screen_Input get_and_reset_camera_screen_input();

private:
	Screen_Input* screen_input = nullptr;
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

	QElapsedTimer     m_timer;
	Camera_Controller camera;
};
