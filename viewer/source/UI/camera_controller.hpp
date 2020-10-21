#pragma once

#include "uniform.hpp"

#include <QElapsedTimer>
#include <QMap>
#include <QVector2D>
#include <QVector3D>

#include <utility>

enum class Dimensions
{
	Zero,
	Two,
	Three
};

class Camera_Screen_Input
{
public:
	Camera_Screen_Input (
		QMap<Qt::Key, bool> const& move_keys_pressed,
		QVector2D const&           pan_direction,
		float                      zoom_direction,
		float                      width,
		float                      height);

	const QMap<Qt::Key, bool> move_keys_pressed;
	const QVector2D           pan_direction;
	const float               zoom_direction;
	const float               width;
	const float               height;

	float determine_move_direction (Qt::Key forward, Qt::Key backwards) const;
};

class Camera_Controller
{
public:
	void update_uniforms (float delta_time, Camera_Screen_Input const& input);

private:
	const QString pos_name   = "camera.position";
	const QString pitch_name = "camera.pitch";
	const QString yaw_name   = "camera.yaw";
	const QString zoom_name  = "camera.zoom";

	const float scroll_wheels_to_max_zoom = 20;
	const float scroll_speed              = 2.0f;
	const float move_speed                = 1.0f;

	Dimensions dimensions = Dimensions::Zero;

	float linear_zoom_target = 0.0f;
	float zoom_previous      = 0.0f;
	float zoom_target        = 0.0f;
	float zoom_acumulated_delta = 0.0f;

	void update_camera_dimensions();

	void update_zoom_target (Camera_Screen_Input const& input);
	void update_zoom (float delta_time);
	float calculate_camera_zoom(float linear_zoom);

	void update_position (float delta_time, Camera_Screen_Input const& input);

	void update_position_2d (
		float                      multiplier,
		Camera_Screen_Input const& input,
		Uniform&                   position);

	void update_position_3d (
		float                      multiplier,
		Camera_Screen_Input const& input,
		Uniform&                   position);

	void update_view (Camera_Screen_Input const& input);

	void
	update_view_2d (QVector2D const& offset, Camera_Screen_Input const& input);

	void
	update_view_3d (QVector2D const& offset, Camera_Screen_Input const& input);

	float get_zoom_factor();

	std::tuple<QVector3D, QVector3D, QVector3D> get_basis();
};