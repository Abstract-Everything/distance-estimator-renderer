#include "main_window.hpp"

#include <QSurfaceFormat>

Main_Window::Main_Window (QWindow* parent) : QQuickWindow (parent)
{
	QSurfaceFormat format;
	format.setDepthBufferSize (3);
	format.setStencilBufferSize (3);
	format.setProfile (QSurfaceFormat::CoreProfile);

#ifdef DEBUG
	format.setOption (QSurfaceFormat::DebugContext);
#endif

	setFormat (format);
	create();
}