#include "screen_input.hpp"

namespace
{

template <typename T>
T get_and_reset_value(T& value)
{
	T old_value = T{};
	std::swap (old_value, value);
	return old_value;
}

}

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
	return move_keys_pressed;
}

QVector2D Screen_Input::get_and_reset_pan_direction()
{
	return get_and_reset_value (pan_direction);
}

float Screen_Input::get_and_reset_zoom_direction()
{
	return get_and_reset_value (zoom_direction);
}

bool Screen_Input::eventFilter (QObject* watched, QEvent* event)
{
	switch (event->type())
	{
	case QEvent::HoverEnter:
		watched->setProperty ("focus", true);
		return true;

	case QEvent::HoverLeave:
		watched->setProperty ("focus", false);
		reset_move_direction();
		return true;

	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	{
		update_move_direction (*static_cast<QKeyEvent*> (event));
		return true;
	}

	case QEvent::MouseButtonPress:
	case QEvent::MouseMove:
	case QEvent::MouseButtonRelease:
	{
		update_pan_direction (*static_cast<QMouseEvent*> (event));
		return true;
	}

	case QEvent::Wheel:
	{
		update_zoom_direction (*static_cast<QWheelEvent*> (event));
		return true;
	}

	default: return QObject::eventFilter (watched, event);
	}
}

void Screen_Input::reset_move_direction()
{
	move_keys_pressed[Qt::Key_A]       = false;
	move_keys_pressed[Qt::Key_D]       = false;
	move_keys_pressed[Qt::Key_Space]   = false;
	move_keys_pressed[Qt::Key_Control] = false;
	move_keys_pressed[Qt::Key_W]       = false;
	move_keys_pressed[Qt::Key_S]       = false;
}

void Screen_Input::update_move_direction (QKeyEvent const& key_event)
{
	Qt::Key key            = static_cast<Qt::Key> (key_event.key());
	move_keys_pressed[key] = (key_event.type() == QEvent::KeyPress);
}

void Screen_Input::update_pan_direction (QMouseEvent const& mouse_event)
{
	QVector2D mouse_position{mouse_event.pos()};

	switch (mouse_event.type())
	{
	case QEvent::MouseButtonPress:
		last_mouse_position = mouse_position;
		break;

	case QEvent::MouseMove:
		pan_direction += QVector2D (mouse_position - last_mouse_position);
		last_mouse_position = mouse_position;
		break;

	case QEvent::MouseButtonRelease:
		pan_direction = {0.0f, 0.0f};
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
	zoom_direction += qAbs(offset) / offset;
}