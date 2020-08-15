#pragma once

#include <QtQuick/QQuickWindow>
#include <QtQuick/QQuickFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>

#include <vector>
#include <map>
#include <string>

class Viewport : public QQuickFramebufferObject
{
  Q_OBJECT

public:
  Viewport (QQuickItem *object = nullptr);
  QQuickFramebufferObject::Renderer *createRenderer() const override;
};

class Viewport_Renderer : public QQuickFramebufferObject::Renderer
{
public:
  Viewport_Renderer (QQuickWindow* window);

protected:
  QOpenGLFramebufferObject *createFramebufferObject (QSize const &size) override;
  
  void render () override;

private:
    QQuickWindow* window;
};
