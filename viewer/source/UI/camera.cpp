#include "camera.hpp"

#include "renderer.hpp"

#include <QDebug>
#include <QKeyEvent>
#include <QQuickWindow>
#include <QtMath>

Camera::Camera()
	: QQuickItem()
{
	connect(&Singletons::renderer, &Singletons::Renderer::update_shader, this, &Camera::shader_updated);
}

void Camera::initialise_signals()
{
	connect(window(), &QQuickWindow::beforeRendering, this, &Camera::update_uniforms);
}

void Camera::set_event_filter(QObject* mouse_area)
{
	if (mouse_area == nullptr)
	{
		qDebug() << "Camera could not initialize mouse area";
		return;
	}

	mouse_area->installEventFilter(this);
}

void Camera::shader_updated()
{
	Uniform position = Singletons::renderer.get_uniform("camera.position");
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

	move_direction.setX(0);
	move_direction.setY(0);
	move_direction.setZ(0);
}

bool Camera::eventFilter(QObject* watched, QEvent* event)
{
	switch (event->type())
	{
	case QEvent::HoverEnter:
		watched->setProperty("focus", true);
		return true;
		break;

	case QEvent::HoverLeave:
		watched->setProperty("focus", false);
		return true;
		break;

	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*>(event);
		update_move_direction(*key_event);
		return true;
	}
	break;

	case QEvent::MouseButtonPress:
	case QEvent::MouseMove:
	case QEvent::MouseButtonRelease:
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
		update_pan_direction(*mouse_event);
		return true;
	}
	break;

	}

	return QObject::eventFilter(watched, event);
}

void Camera::update_uniforms()
{
	float deltaTime = static_cast<float>(m_timer.elapsed()) / 1000.0f;
	update_position(deltaTime);
	update_view(deltaTime);
	m_timer.restart();
}

void Camera::update_position(float deltaTime)
{
	const QVector3D move_offset = move_direction * deltaTime;
	Uniform position = Singletons::renderer.get_uniform("camera.position");
	if (dimensions == Dimensions::Two)
	{
		position.set_value(position.value(0).toFloat() + move_offset.x(), 0);
		position.set_value(position.value(1).toFloat() + move_offset.y(), 1);
	}
	else if (dimensions == Dimensions::Three)
	{
		position.set_value(position.value(0).toFloat() + move_offset.x(), 0);
		position.set_value(position.value(1).toFloat() + move_offset.y(), 1);
		position.set_value(position.value(2).toFloat() + move_offset.z(), 2);
	}
	Singletons::renderer.set_uniform(position);
}

void Camera::update_view(float deltaTime)
{
	const QVector2D offset = QVector2D(-1.0f, 1.0f) * pan_direction;
	if (dimensions == Dimensions::Two)
	{
		Uniform position = Singletons::renderer.get_uniform("camera.position");
		position.set_value(position.value(0).toFloat() + offset.x(), 0);
		position.set_value(position.value(1).toFloat() + offset.y(), 1);
		Singletons::renderer.set_uniform(position);
	}
	else if (dimensions == Dimensions::Three)
	{
		const float pi_2 = qAcos(-1);
		Uniform yaw = Singletons::renderer.get_uniform("camera.yaw");
		Uniform pitch = Singletons::renderer.get_uniform("camera.pitch");
		yaw.set_value(yaw.value(0).toFloat() + offset.x() * pi_2, 0);
		pitch.set_value(pitch.value(0).toFloat() + offset.y() * pi_2, 0);
		Singletons::renderer.set_uniform(yaw);
		Singletons::renderer.set_uniform(pitch);
	}
	pan_direction = { 0.0f, 0.0f };
}

void Camera::update_move_direction(QKeyEvent const& key_event)
{
	if (dimensions == Dimensions::Two)
	{
		update_move_direction_2d(key_event);
	}
	else if (dimensions == Dimensions::Three)
	{
		update_move_direction_3d(key_event);
	}
}

void Camera::update_move_direction_2d(QKeyEvent const& key_event)
{

	if (key_event.type() == QEvent::KeyPress)
	{
		switch (key_event.key())
		{
		case Qt::Key_A:
			move_direction.setX(-1);
			break;

		case Qt::Key_D:
			move_direction.setX(1);
			break;

		case Qt::Key_W:
			move_direction.setY(1);
			break;

		case Qt::Key_S:
			move_direction.setY(-1);
			break;
		}
	}
	else if (key_event.type() == QEvent::KeyRelease)
	{
		switch (key_event.key())
		{
		case Qt::Key_A:
		case Qt::Key_D:
			move_direction.setX(0);
			break;

		case Qt::Key_W:
		case Qt::Key_S:
			move_direction.setY(0);
			break;
		}
	}
}

void Camera::update_move_direction_3d(QKeyEvent const& key_event)
{
	if (key_event.type() == QEvent::KeyPress)
	{
		switch (key_event.key())
		{
		case Qt::Key_A:
			move_direction.setX(-1);
			break;

		case Qt::Key_D:
			move_direction.setX(1);
			break;

		case Qt::Key_Space:
			move_direction.setY(1);
			break;

		case Qt::Key_Control:
			move_direction.setY(-1);
			break;

		case Qt::Key_W:
			move_direction.setZ(1);
			break;

		case Qt::Key_S:
			move_direction.setZ(-1);
			break;
		}
	}
	else if (key_event.type() == QEvent::KeyRelease)
	{
		switch (key_event.key())
		{
		case Qt::Key_A:
		case Qt::Key_D:
			move_direction.setX(0);
			break;

		case Qt::Key_Space:
		case Qt::Key_Control:
			move_direction.setY(0);
			break;

		case Qt::Key_W:
		case Qt::Key_S:
			move_direction.setZ(0);
			break;
		}
	}
}

void Camera::update_pan_direction(QMouseEvent const &mouse_event)
{
	QVector2D mouse_position {
		static_cast<float>(mouse_event.pos().x() / width()),
		static_cast<float>(mouse_event.pos().y() / height())
	};

	switch (mouse_event.type())
	{
	case QEvent::MouseButtonPress:
		last_mouse_position = mouse_position;
		break;

	case QEvent::MouseMove:
		pan_direction = QVector2D(mouse_position - last_mouse_position);
		last_mouse_position = mouse_position;
		break;

	case QEvent::MouseButtonRelease:
		pan_direction = { 0.0f, 0.0f };
		break;

	}
}
