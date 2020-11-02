#include "screen_input.hpp"

Screen_Input::Screen_Input (QQuickItem* parent) : QQuickItem (parent)
{
	reset_move_direction();
}

void Screen_Input::set_event_filter (QObject* mouse_area)
{
	assert (
		mouse_area != nullptr && "Screen was initialized with null mouse area");
	mouse_area->installEventFilter (this);
}

QMap<Qt::Key, bool> Screen_Input::move_keys() const
{
	return m_move_keys_pressed;
}

QVector2D Screen_Input::pan_direction() const
{
	return m_pan_direction;
}

float Screen_Input::zoom_direction() const
{
	return m_zoom_direction;
}

void Screen_Input::reset_input()
{
	m_pan_direction = QVector2D();
	m_zoom_direction = 0.0f;
}

bool Screen_Input::eventFilter (QObject* watched, QEvent* event)
{
	switch (event->type())
	{
	case QEvent::HoverEnter:
		watched->setProperty ("focus", true);
		break;

	case QEvent::HoverLeave:
		watched->setProperty ("focus", false);
		reset_move_direction();
		break;

	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	{
		update_move_direction (*static_cast<QKeyEvent*> (event));
		break;
	}

	case QEvent::MouseButtonPress:
	case QEvent::MouseMove:
	case QEvent::MouseButtonRelease:
	{
		update_pan_direction (*static_cast<QMouseEvent*> (event));
		break;
	}

	case QEvent::Wheel:
	{
		update_zoom_direction (*static_cast<QWheelEvent*> (event));
		break;
	}

	default: return QObject::eventFilter (watched, event);
	}

	emit input_updated();
	return true;
}

void Screen_Input::reset_move_direction()
{
	m_move_keys_pressed[Qt::Key_A]       = false;
	m_move_keys_pressed[Qt::Key_D]       = false;
	m_move_keys_pressed[Qt::Key_Space]   = false;
	m_move_keys_pressed[Qt::Key_Control] = false;
	m_move_keys_pressed[Qt::Key_W]       = false;
	m_move_keys_pressed[Qt::Key_S]       = false;
}

void Screen_Input::update_move_direction (QKeyEvent const& key_event)
{
	Qt::Key key            = static_cast<Qt::Key> (key_event.key());
	m_move_keys_pressed[key] = (key_event.type() == QEvent::KeyPress);
}

void Screen_Input::update_pan_direction (QMouseEvent const& mouse_event)
{
	QVector2D mouse_position{mouse_event.pos()};

	switch (mouse_event.type())
	{
	case QEvent::MouseButtonPress:
		m_last_mouse_position = mouse_position;
		break;

	case QEvent::MouseMove:
		m_pan_direction += QVector2D (mouse_position - m_last_mouse_position);
		m_last_mouse_position = mouse_position;
		break;

	case QEvent::MouseButtonRelease:
		m_pan_direction = {0.0f, 0.0f};
		break;

	default: break;
	}
}

void Screen_Input::update_zoom_direction (QWheelEvent const& wheel_event)
{
	const float offset = wheel_event.delta();
	if (offset == 0)
	{
		return;
	}
	m_zoom_direction += qAbs(offset) / offset;
}