#include "camera.hpp"

#include "constants.hpp"
#include "renderer.hpp"

#include <QDebug>
#include <QKeyEvent>
#include <QQuickWindow>
#include <QtMath>

Camera::Camera() : QQuickItem()
{
	connect (
		&Singletons::renderer,
		&Singletons::Renderer::update_shader,
		this,
		&Camera::shader_updated);
}

void Camera::initialise_signals()
{
	connect (
		window(),
		&QQuickWindow::beforeRendering,
		this,
		&Camera::update_uniforms);
}

void Camera::set_event_filter (QObject* mouse_area)
{
	if (mouse_area == nullptr)
	{
		qDebug() << "Camera could not initialize mouse area";
		return;
	}

	mouse_area->installEventFilter (this);
}

void Camera::shader_updated()
{
	Uniform position = Singletons::renderer.get_uniform ("camera.position");
	if (position.size() == 2)
	{
		dimensions = Dimensions::Two;
	}
	else if (position.size() == 3)
	{
		dimensions = Dimensions::Three;
	}
	else
	{
		qDebug() << "Camera position size is neither 2 nor 3";
	}

	move_direction.setX (0);
	move_direction.setY (0);
	move_direction.setZ (0);
}

bool Camera::eventFilter (QObject* watched, QEvent* event)
{
	switch (event->type())
	{
	case QEvent::HoverEnter:
		watched->setProperty ("focus", true);
		return true;

	case QEvent::HoverLeave:
		watched->setProperty ("focus", false);
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
		update_zoom_offset (*static_cast<QWheelEvent*> (event));
		return true;
	}

	default:
		return QObject::eventFilter (watched, event);
	}
}

float Camera::get_zoom()
{
	Uniform zoom = Singletons::renderer.get_uniform ("camera.zoom");
	return zoom.value (0).toFloat();
}

float Camera::get_zoom_factor()
{
	const float zoom = get_zoom();
	return qBound (0.0f, zoom + 1.0f, 2.0f);
}

void Camera::update_uniforms()
{
	float delta_time = static_cast<float> (m_timer.elapsed()) / 1000.0f;
	update_zoom (delta_time);
	update_position (delta_time);
	update_view (delta_time);
	m_timer.restart();
}

void Camera::update_zoom(float delta_time)
{
	if (qAbs (zoom_offset) < cnst::min_ui_float)
	{
		return;
	}

	Uniform zoom = Singletons::renderer.get_uniform ("camera.zoom");
	const float zoom_value  = zoom.value (0).toFloat();
	const float zoom_change = zoom_delta * delta_time;
	zoom.set_value (qBound (-1.0f, zoom_value + zoom_change, 1.0f), 0);
	Singletons::renderer.set_uniform (zoom);
	zoom_offset -= zoom_change;
	if (qAbs (zoom_offset) < qAbs (zoom_change))
	{
		zoom_offset = 0.0f;
	}
}

void Camera::update_position (float delta_time)
{
	const QVector3D move_offset
		= move_direction * delta_time * get_zoom_factor();
	Uniform position = Singletons::renderer.get_uniform ("camera.position");
	if (dimensions == Dimensions::Two)
	{
		update_position_2d (move_offset, position);
	}
	else if (dimensions == Dimensions::Three)
	{
		update_position_3d (move_offset, position);
	}
	Singletons::renderer.set_uniform (position);
}

void Camera::update_position_2d (
	QVector3D const& move_offset,
	Uniform& position)
{
	position.set_value (position.value (0).toFloat() + move_offset.x(), 0);
	position.set_value (position.value (1).toFloat() + move_offset.y(), 1);
}

void Camera::update_position_3d (
	QVector3D const& move_offset,
	Uniform&         position)
{
	position.set_value (position.value (0).toFloat() + move_offset.x(), 0);
	position.set_value (position.value (1).toFloat() + move_offset.y(), 1);
	position.set_value (position.value (2).toFloat() + move_offset.z(), 2);
}

void Camera::update_view (float /* delta_time */)
{
	const QVector2D offset
		= get_zoom_factor() * QVector2D (-1.0f, 1.0f) * pan_direction;
	pan_direction          = {0.0f, 0.0f};
	if (dimensions == Dimensions::Two)
	{
		update_view_2d (offset);
	}
	else if (dimensions == Dimensions::Three)
	{
		update_view_3d (offset);
	}
}

void Camera::update_view_2d(QVector2D const& offset)
{
	Uniform position = Singletons::renderer.get_uniform ("camera.position");
	position.set_value (position.value (0).toFloat() + offset.x(), 0);
	position.set_value (position.value (1).toFloat() + offset.y(), 1);
	Singletons::renderer.set_uniform (position);
}

void Camera::update_view_3d (QVector2D const& offset)
{
	Uniform yaw       = Singletons::renderer.get_uniform ("camera.yaw");
	float yaw_value = (yaw.value (0).toFloat() + offset.x() * cnst::pi);
	yaw.set_value (yaw_value, 0);
	Singletons::renderer.set_uniform (yaw);

	Uniform pitch = Singletons::renderer.get_uniform ("camera.pitch");
	const float pitch_value = qBound (
		-cnst::pi_2,
		pitch.value (0).toFloat() + offset.y() * cnst::pi,
		cnst::pi_2);
	pitch.set_value (pitch_value, 0);
	Singletons::renderer.set_uniform (pitch);
}

void Camera::update_move_direction (QKeyEvent const& key_event)
{
	if (dimensions == Dimensions::Two)
	{
		update_move_direction_2d (key_event);
	}
	else if (dimensions == Dimensions::Three)
	{
		update_move_direction_3d (key_event);
	}
}

void Camera::update_move_direction_2d (QKeyEvent const& key_event)
{
	if (key_event.type() == QEvent::KeyPress)
	{
		switch (key_event.key())
		{
		case Qt::Key_A: move_direction.setX (-1); break;
		case Qt::Key_D: move_direction.setX (1); break;
		case Qt::Key_W: move_direction.setY (1); break;
		case Qt::Key_S: move_direction.setY (-1); break;
		}
	}
	else if (key_event.type() == QEvent::KeyRelease)
	{
		switch (key_event.key())
		{
		case Qt::Key_A:
		case Qt::Key_D: move_direction.setX (0); break;

		case Qt::Key_W:
		case Qt::Key_S: move_direction.setY (0); break;
		}
	}
}

void Camera::update_move_direction_3d (QKeyEvent const& key_event)
{
	if (key_event.type() == QEvent::KeyPress)
	{
		switch (key_event.key())
		{
		case Qt::Key_A:       move_direction.setX (-1); break;
		case Qt::Key_D:       move_direction.setX (1); break;
		case Qt::Key_Space:   move_direction.setY (1); break;
		case Qt::Key_Control: move_direction.setY (-1); break;
		case Qt::Key_W:       move_direction.setZ (1); break;
		case Qt::Key_S:       move_direction.setZ (-1); break;
		}
	}
	else if (key_event.type() == QEvent::KeyRelease)
	{
		switch (key_event.key())
		{
		case Qt::Key_A:
		case Qt::Key_D: move_direction.setX (0); break;

		case Qt::Key_Space:
		case Qt::Key_Control: move_direction.setY (0); break;

		case Qt::Key_W:
		case Qt::Key_S: move_direction.setZ (0); break;
		}
	}
}

void Camera::update_pan_direction (QMouseEvent const& mouse_event)
{
	QVector2D mouse_position{
		static_cast<float> (mouse_event.pos().x() / width()),
		static_cast<float> (mouse_event.pos().y() / height())};

	switch (mouse_event.type())
	{
	case QEvent::MouseButtonPress: last_mouse_position = mouse_position; break;

	case QEvent::MouseMove:
		pan_direction       = QVector2D (mouse_position - last_mouse_position);
		last_mouse_position = mouse_position;
		break;

	case QEvent::MouseButtonRelease: pan_direction = {0.0f, 0.0f}; break;

	default: break;
	}
}

void Camera::update_zoom_offset (QWheelEvent const& wheel_event)
{
	if (qAbs (wheel_event.angleDelta().y()) == 0)
	{
		return;
	}
	const float offset    = wheel_event.angleDelta().y();
	const float direction = offset / qAbs (offset);

	const float zoom        = qAbs (get_zoom() + zoom_offset);
	const float zoom_change = 1.0f - zoom;
	zoom_offset             = direction * qMax (zoom_change, cnst::min_ui_float);
	zoom_delta              = zoom_offset;
}