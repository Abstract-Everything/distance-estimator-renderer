#include "camera_controller.hpp"

#include "constants.hpp"
#include "singletons.hpp"

namespace
{

float get_uniform_vector_value (QString const& name, int index = 0)
{
	return Singletons::renderer().get_uniform (name).value (index).toFloat();
}

void set_uniform_vector_value (QString const& name, QList<float> values)
{
	int index = 0;
	for (float value : values)
	{
		Uniform uniform = Singletons::renderer().get_uniform (name);
		uniform.set_value (value, index++);
		Singletons::renderer().set_uniform (uniform);
	}
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

void Camera_Controller::update_uniforms (Camera_Screen_Input const& input)
{
	update_camera_dimensions();
	if (dimensions == Dimensions::Zero)
	{
		return;
	}

	update_zoom (input);
	update_position (input);
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

void Camera_Controller::update_zoom (Camera_Screen_Input const& input)
{
	if (input.zoom_direction == 0.0f)
	{
		return;
	}
	/*
	 * This is used to swap between normalized zoom in to an exponentially
	 * decreasing zoom. So that if it is being scaled to 10 digits:
	 * Linear | Exponential decrease
	 * 0      | 0
	 * 0.1    | 0.9
	 * 0.2    | 0.99
	 * ...
	 * 1      | 0.9999999999
	 *
	 * If zooming out use linear value.
	 */
	float zoom_previous
		= decreasing_to_linear_zoom (get_uniform_vector_value (zoom_name));
	const float offset = input.zoom_direction / scroll_wheels_to_max_zoom;
	float zoom_value = linear_to_decreasing_zoom(zoom_previous + offset);
	set_uniform_vector_value (zoom_name, {zoom_value});
}

float Camera_Controller::linear_to_decreasing_zoom (float linear_zoom)
{
	if (linear_zoom < 0.0f)
	{
		return linear_zoom;
	}
	float next_digit = linear_zoom * cnst::max_ui_float_decimal_points;
	float power      = qPow (10, next_digit);
	return (power - 1.0f) / power;
}

float Camera_Controller::decreasing_to_linear_zoom (float decreasing_zoom)
{
	if (decreasing_zoom < 0.0f)
	{
		return decreasing_zoom;
	}
	float inverse_remaining_zoom = 1.0f / (1.0f - decreasing_zoom);
	return log10f (inverse_remaining_zoom) / cnst::max_ui_float_decimal_points;
}

void Camera_Controller::update_position (Camera_Screen_Input const& input)
{
	const float multiplier = get_zoom_factor();
	if (dimensions == Dimensions::Two)
	{
		update_position_2d (multiplier, input);
	}
	else if (dimensions == Dimensions::Three)
	{
		update_position_3d (multiplier, input);
	}
}

void Camera_Controller::update_position_2d (
	float                      multiplier,
	Camera_Screen_Input const& input)
{
	const QVector2D position_offset
		= QVector2D (
			  input.determine_move_direction (Qt::Key_D, Qt::Key_A),
			  input.determine_move_direction (Qt::Key_W, Qt::Key_S))
		  * multiplier * move_speed;

	if (position_offset.lengthSquared() == 0.0f)
	{
		return;
	}

	QVector2D previous_position{
		get_uniform_vector_value (pos_name, 0),
		get_uniform_vector_value (pos_name, 1)};

	QVector3D position = previous_position + position_offset;
	set_uniform_vector_value (pos_name, {position.x(), position.y()});
}

void Camera_Controller::update_position_3d (
	float                      multiplier,
	Camera_Screen_Input const& input)
{
	const QVector3D position_offset
		= QVector3D (
			  input.determine_move_direction (Qt::Key_D, Qt::Key_A),
			  input.determine_move_direction (Qt::Key_Space, Qt::Key_Control),
			  input.determine_move_direction (Qt::Key_W, Qt::Key_S))
		  * multiplier * move_speed;

	if (position_offset.lengthSquared() == 0.0f)
	{
		return;
	}

	QVector3D previous_position{
		get_uniform_vector_value (pos_name, 0),
		get_uniform_vector_value (pos_name, 1),
		get_uniform_vector_value (pos_name, 2)};

	auto [right, up, forward] = get_basis();
	QVector3D position = previous_position
						 + position_offset.x() * right
						 + position_offset.y() * up
						 + position_offset.z() * forward;

	set_uniform_vector_value (
		pos_name,
		{position.x(), position.y(), position.z()});
}

void Camera_Controller::update_view (Camera_Screen_Input const& input)
{
	const QVector2D offset
		= get_zoom_factor() * QVector2D (-1.0f, 1.0f) * input.pan_direction;
	if (abs (offset.lengthSquared()) == 0.0f)
	{
		return;
	}

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

	float yaw_value = get_uniform_vector_value (yaw_name) + view_offset.x();
	set_uniform_vector_value (yaw_name, {yaw_value});

	const float pitch_value = qBound (
		-cnst::pi_2,
		get_uniform_vector_value (pitch_name) + view_offset.y(),
		cnst::pi_2);
	set_uniform_vector_value (pitch_name, {pitch_value});
}

float Camera_Controller::get_zoom_factor()
{
	const float zoom = get_uniform_vector_value (zoom_name);
	return qBound (cnst::min_ui_float, zoom * -1.0f + 1.0f, 2.0f);
}

std::tuple<QVector3D, QVector3D, QVector3D> Camera_Controller::get_basis()
{
	const float pitch = get_uniform_vector_value (pitch_name);
	const float yaw   = get_uniform_vector_value (yaw_name);

	const QVector3D general_up (0.0f, 1.0f, 0.0f);

	QVector3D forward
		= QVector3D (qSin (yaw), qSin (pitch), qCos (yaw)).normalized();

	QVector3D right
		= QVector3D::crossProduct (general_up, forward).normalized();

	QVector3D up = QVector3D::crossProduct (forward, right).normalized();

	return {right, up, forward};
}
