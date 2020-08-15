#include "main_window.hpp"

#include "uniform.hpp"
#include "viewport.hpp"
#include "inspector.hpp"
#include "camera.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick/QQuickView>
#include <QQmlContext>
#include <QQmlComponent>
#include <QOpenGLContextGroup>

int main (int argc, char** argv)
{
  QGuiApplication application (argc, argv);

  // QML Types
  qmlRegisterType<Main_Window> ("renderer.main_window", 1, 0, "Main_Window_");
  qmlRegisterType<Viewport> ("renderer.viewport", 1, 0, "Viewport_");
  qmlRegisterType<Inspector> ("renderer.inspector", 1, 0, "Inspector_");
  qmlRegisterType<Camera> ("renderer.camera", 1, 0, "Camera_");

  // QTypes
  qRegisterMetaType<Uniform> ("Uniform");

  QQmlApplicationEngine engine (QUrl ("qrc:/UI/main.qml"));
  return application.exec();
}