#include "main_window.hpp"
#include "viewport.hpp"
#include "inspector.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick/QQuickView>
#include <QQmlContext>
#include <QQmlComponent>
#include <QOpenGLContextGroup>

int main (int argc, char** argv)
{
  QGuiApplication application (argc, argv);
  qmlRegisterType<Main_Window> ("renderer.main_window", 1, 0, "Main_Window_Class");
  qmlRegisterType<Viewport> ("renderer.viewport", 1, 0, "Viewport_Class");
  qmlRegisterType<Inspector> ("renderer.inspector", 1, 0, "Inspector_Class");

  QQmlApplicationEngine engine (QUrl ("qrc:/main.qml"));
  return application.exec();
}
