#include "camera_controller.hpp"

#include "constants.hpp"
#include "singletons.hpp"

namespace
{

float get_uniform_float_value (QString const& name)
{
	return Singletons::renderer().get_uniform (name).value (0).toFloat();
}

void set_uniform_float_value(QString const& name, float value)
{
	Uniform uniform = Singletons::renderer().get_uniform (name);
	uniform.set_value (value, 0);
	Singletons::renderer().set_uniform (uniform);
}

}

Camera_Screen_Input::Camera_Screen_Input (
	QMap<Qt::Key, bool> const& move_keys_pressed,
	QVector2D const&           pan_direction,
	float                      zoom_direction,
	float                      width,
	float                      height)
	: move_keys_pressed (move_keys_pressed)
	, pan_direction (pan_direction)
	, zoom_direction (zoom_direction)
	, width (width)
	, height (height)
{
}

float Camera_Screen_Input::determine_move_direction (
	Qt::Key forward,
	Qt::Key backwards) const
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
	return direction;
}

void Camera_Controller::update_uniforms (
	float                      delta_time,
	Camera_Screen_Input const& input)
{
	update_camera_dimensions();
	if (dimensions == Dimensions::Zero)
	{
		return;
	}

	update_zoom_target (input);
	update_zoom (delta_time);
	update_position (delta_time, input);
	update_view (input);
}

void Camera_Controller::update_camera_dimensions()
{
	dimensions = Dimensions::Zero;
	if (!Singletons::renderer().exists_uniform (pos_name))
	{
		return;
	}

	const Uniform position = Singletons::renderer().get_uniform (pos_name);
	if (position.size() == 2)
	{
		dimensions = Dimensions::Two;
	}
	else if (position.size() == 3)
	{
		dimensions = Dimensions::Three;
	}
}

void Camera_Controller::update_zoom_target (Camera_Screen_Input const& input)
{
	const float offset = input.zoom_direction / scroll_wheels_to_max_zoom;
	if (offset != 0.0f)
	{
		linear_zoom_target = qBound (-1.0f, linear_zoom_target + offset, 1.0f);
		zoom_acumulated_delta = 0.0f;
		zoom_previous         = get_uniform_float_value(zoom_name);
		zoom_target           = calculate_camera_zoom (linear_zoom_target);
	}
}

void Camera_Controller::update_zoom (float delta_time)
{
	if (zoom_acumulated_delta >= 1.0f
		|| qAbs (zoom_target - zoom_previous) < cnst::min_ui_float)
	{
		return;
	}

	zoom_acumulated_delta = qMin(zoom_acumulated_delta + delta_time, 1.0f);
	float offset     = (zoom_target - zoom_previous) * zoom_acumulated_delta;
	float zoom_value = zoom_previous + offset;
	set_uniform_float_value (zoom_name, zoom_value);
}

float Camera_Controller::calculate_camera_zoom (float linear_zoom)
{
	/*
	 * This is used to map a normalized zoom in to an exponentially decreasing
	 * zoom. So that if it is being scaled to 10 digits:
	 * Linear | Exponential decrease
	 * 0      | 0
	 * 0.1    | 0.9
	 * 0.2    | 0.99
	 * ...
	 * 1      | 0.9999999999
	 * 
	 * If zooming out use linear value
	 */
	if (linear_zoom < 0.0f)
	{
		return linear_zoom;
	}
	float next_digit = linear_zoom * cnst::max_ui_float_decimal_points;
	float power      = qPow (10, next_digit);
	return (power - 1.0f) / power;
}

void Camera_Controller::update_position (
	float                      delta_time,
	Camera_Screen_Input const& input)
{
	const float multiplier = delta_time * get_zoom_factor();
	Uniform     position   = Singletons::renderer().get_uniform (pos_name);
	if (dimensions == Dimensions::Two)
	{
		update_position_2d (multiplier, input, position);
	}
	else if (dimensions == Dimensions::Three)
	{
		update_position_3d (multiplier, input, position);
	}
	Singletons::renderer().set_uniform (position);
}

void Camera_Controller::update_position_2d (
	float                      multiplier,
	Camera_Screen_Input const& input,
	Uniform&                   position)
{
	const QVector2D move_offset
		= QVector2D (
			  input.determine_move_direction (Qt::Key_D, Qt::Key_A),
			  input.determine_move_direction (Qt::Key_W, Qt::Key_S))
		  * multiplier * move_speed;

	position.set_value (position.value (0).toFloat() + move_offset.x(), 0);
	position.set_value (position.value (1).toFloat() + move_offset.y(), 1);
}

void Camera_Controller::update_position_3d (
	float                      multiplier,
	Camera_Screen_Input const& input,
	Uniform&                   position)
{
	const QVector3D move_offset
		= QVector3D (
			  input.determine_move_direction (Qt::Key_D, Qt::Key_A),
			  input.determine_move_direction (Qt::Key_Space, Qt::Key_Control),
			  input.determine_move_direction (Qt::Key_W, Qt::Key_S))
		  * multiplier * move_speed;

	auto [right, up, forward] = get_basis();
	QVector3D position_value  = move_offset.x() * right + move_offset.y() * up
							   + move_offset.z() * forward;

	position.set_value (position.value (0).toFloat() + position_value.x(), 0);
	position.set_value (position.value (1).toFloat() + position_value.y(), 1);
	position.set_value (position.value (2).toFloat() + position_value.z(), 2);
}

void Camera_Controller::update_view (Camera_Screen_Input const& input)
{
	const QVector2D offset
		= get_zoom_factor() * QVector2D (-1.0f, 1.0f) * input.pan_direction;
	if (dimensions == Dimensions::Two)
	{
		update_view_2d (offset, input);
	}
	else if (dimensions == Dimensions::Three)
	{
		update_view_3d (offset, input);
	}
}

void Camera_Controller::update_view_2d (
	QVector2D const&           offset,
	Camera_Screen_Input const& input)
{
	const float aspect      = input.height / input.width;
	QVector2D   view_offset = QVector2D (offset.x(), offset.y() * aspect)
							/ cnst::screen_in_pixels_2d;

	Uniform position = Singletons::renderer().get_uniform (pos_name);
	position.set_value (position.value (0).toFloat() + view_offset.x(), 0);
	position.set_value (position.value (1).toFloat() + view_offset.y(), 1);
	Singletons::renderer().set_uniform (position);
}

void Camera_Controller::update_view_3d (
	QVector2D const&           offset,
	Camera_Screen_Input const& input)
{
	QVector2D view_offset
		= QVector2D (offset.x() / input.width, offset.y() / input.height)
		  * cnst::pi_2;

	float yaw_value = get_uniform_float_value (yaw_name) + view_offset.x();
	set_uniform_float_value (yaw_name, yaw_value);

	const float pitch_value = qBound (
		-cnst::pi_2,
		get_uniform_float_value (pitch_name) + view_offset.y(),
		cnst::pi_2);
	set_uniform_float_value (pitch_name, pitch_value);
}

float Camera_Controller::get_zoom_factor()
{
	const float zoom = get_uniform_float_value(zoom_name);
	return qBound (cnst::min_ui_float, zoom * -1.0f + 1.0f, 2.0f);
}

std::tuple<QVector3D, QVector3D, QVector3D> Camera_Controller::get_basis()
{
	const float pitch = get_uniform_float_value (pitch_name);
	const float yaw = get_uniform_float_value (yaw_name);

	const QVector3D general_up (0.0f, 1.0f, 0.0f);

	QVector3D forward
		= QVector3D (qSin (yaw), qSin (pitch), qCos (yaw)).normalized();

	QVector3D right
		= QVector3D::crossProduct (general_up, forward).normalized();

	QVector3D up = QVector3D::crossProduct (forward, right).normalized();

	return {right, up, forward};
}
