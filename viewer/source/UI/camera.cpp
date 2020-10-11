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

	reset_move_direction();
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
	reset_move_direction();
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

void Camera::set_zoom(float zoom_value)
{
	zoom_value = qBound (
		-1.0f + cnst::min_ui_float,
		zoom_value,
		1.0f - cnst::min_ui_float);
	Uniform zoom = Singletons::renderer.get_uniform ("camera.zoom");
	zoom.set_value (zoom_value, 0);
	Singletons::renderer.set_uniform (zoom);
}

float Camera::get_zoom_factor()
{
	const float zoom = get_zoom();
	return qBound (cnst::min_ui_float, zoom * -1.0f + 1.0f, 2.0f);
}

std::tuple<QVector3D, QVector3D, QVector3D> Camera::get_basis()
{
	const float pitch
		= Singletons::renderer.get_uniform ("camera.pitch").value (0).toFloat();
	const float yaw
		= Singletons::renderer.get_uniform ("camera.yaw").value (0).toFloat();

	
	QVector3D forward
		= QVector3D (qSin (yaw), qSin (pitch), qCos (yaw)).normalized();

	QVector3D right
		= QVector3D::crossProduct (QVector3D (0.0f, 1.0f, 0.0f), forward)
			  .normalized();

	QVector3D up = QVector3D::crossProduct(forward, right).normalized();
	return {right, up, forward};
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
	const float zoom_delta_remaining = qAbs(get_zoom() - zoom_before);
	const float zoom_delta           = zoom_after - zoom_before;
	const float zoom_delta_abs       = qAbs (zoom_delta);
	if (qAbs (zoom_delta_remaining - zoom_delta_abs)
		< zoom_delta_abs / cnst::zoom_factor + cnst::min_ui_float)
	{
		return;
	}
	const float direction = zoom_delta / zoom_delta_abs;

	const float zoom_change
		= direction
		  * qMax (zoom_delta_abs * delta_time * 2.0f, cnst::min_ui_float);
	set_zoom (get_zoom() + zoom_change);
}

void Camera::update_position (float delta_time)
{
	const float multiplier = delta_time * get_zoom_factor();
	Uniform position = Singletons::renderer.get_uniform ("camera.position");
	if (dimensions == Dimensions::Two)
	{
		update_position_2d (multiplier, position);
	}
	else if (dimensions == Dimensions::Three)
	{
		update_position_3d (multiplier, position);
	}
	Singletons::renderer.set_uniform (position);
}

void Camera::update_position_2d (float multiplier, Uniform& position)
{
	const QVector2D move_offset (
		determine_move_direction (Qt::Key_D, Qt::Key_A) * multiplier,
		determine_move_direction (Qt::Key_W, Qt::Key_S) * multiplier);
	
	position.set_value (position.value (0).toFloat() + move_offset.x(), 0);
	position.set_value (position.value (1).toFloat() + move_offset.y(), 1);
}

void Camera::update_position_3d (float multiplier, Uniform& position)
{
	const QVector3D move_offset (
		determine_move_direction (Qt::Key_D, Qt::Key_A) * multiplier,
		determine_move_direction (Qt::Key_Space, Qt::Key_Control) * multiplier,
		determine_move_direction (Qt::Key_W, Qt::Key_S) * multiplier);

	auto [right, up, forward] = get_basis();
	QVector3D position_value  = move_offset.x() * right + move_offset.y() * up
							   + move_offset.z() * forward;
	
	position.set_value (position.value (0).toFloat() + position_value.x(), 0);
	position.set_value (position.value (1).toFloat() + position_value.y(), 1);
	position.set_value (position.value (2).toFloat() + position_value.z(), 2);
}

float Camera::determine_move_direction (Qt::Key forward, Qt::Key backwards)
{
	float direction = 0.0f;
	if (move_keys_pressed[forward])
	{
		direction += 1.0f;
	}

	if (move_keys_pressed[backwards])
	{
		direction -= 1.0f;
	}
	return direction * move_speed;
}

void Camera::update_view (float /* delta_time */)
{
	const QVector2D offset
		= get_zoom_factor() * QVector2D (-1.0f, 1.0f) * pan_direction;
	pan_direction = {0.0f, 0.0f};
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
	const float aspect
		= static_cast<float> (height()) / static_cast<float> (width());
	QVector2D view_offset = QVector2D (offset.x(), offset.y() * aspect)
							/ cnst::screen_in_pixels_2d;

	Uniform position = Singletons::renderer.get_uniform ("camera.position");
	position.set_value (position.value (0).toFloat() + view_offset.x(), 0);
	position.set_value (position.value (1).toFloat() + view_offset.y(), 1);
	Singletons::renderer.set_uniform (position);
}

void Camera::update_view_3d (QVector2D const& offset)
{
	QVector2D view_offset
		= QVector2D (offset.x() / width(), offset.y() / height()) * cnst::pi_2;

	Uniform yaw       = Singletons::renderer.get_uniform ("camera.yaw");
	float   yaw_value = (yaw.value (0).toFloat() + view_offset.x());
	yaw.set_value (yaw_value, 0);
	Singletons::renderer.set_uniform (yaw);

	Uniform pitch = Singletons::renderer.get_uniform ("camera.pitch");
	const float pitch_value = qBound (
		-cnst::pi_2,
		pitch.value (0).toFloat() + view_offset.y(),
		cnst::pi_2);
	pitch.set_value (pitch_value, 0);
	Singletons::renderer.set_uniform (pitch);
}

void Camera::reset_move_direction()
{
	move_keys_pressed[Qt::Key_A] = false;
	move_keys_pressed[Qt::Key_D] = false;
	move_keys_pressed[Qt::Key_Space] = false;
	move_keys_pressed[Qt::Key_Control] = false;
	move_keys_pressed[Qt::Key_W] = false;
	move_keys_pressed[Qt::Key_S] = false;
}

void Camera::update_move_direction (QKeyEvent const& key_event)
{
	Qt::Key key            = static_cast<Qt::Key> (key_event.key());
	move_keys_pressed[key] = (key_event.type() == QEvent::KeyPress);
}

void Camera::update_pan_direction (QMouseEvent const& mouse_event)
{
	QVector2D mouse_position{mouse_event.pos()};

	switch (mouse_event.type())
	{
	case QEvent::MouseButtonPress: last_mouse_position = mouse_position; break;

	case QEvent::MouseMove:
		pan_direction       += QVector2D (mouse_position - last_mouse_position);
		last_mouse_position = mouse_position;
		break;

	case QEvent::MouseButtonRelease: pan_direction = {0.0f, 0.0f}; break;

	default: break;
	}
}

void Camera::update_zoom_offset (QWheelEvent const& wheel_event)
{
	const float offset = wheel_event.angleDelta().y();
	if (qAbs (offset) == 0)
	{
		return;
	}
	const float direction  = offset / qAbs (offset);
	const float remaining  = 1.0f - qAbs (zoom_after);
	const float zoom_delta = direction * remaining / cnst::zoom_factor;

	zoom_before = get_zoom();
	zoom_after  = qBound (
        -1.0f + cnst::min_ui_float,
        zoom_after + zoom_delta,
        1.0f - cnst::min_ui_float);
}