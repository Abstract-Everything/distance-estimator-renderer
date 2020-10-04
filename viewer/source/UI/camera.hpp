#pragma once

#include "uniform.hpp"

#include <QElapsedTimer>
#include <QQuickItem>
#include <QVector3D>

class Camera : public QQuickItem
{
	Q_OBJECT

public:
	Camera();

	Q_INVOKABLE void initialise_signals();
	Q_INVOKABLE void set_event_filter (QObject* mouse_area);

public slots:
	void shader_updated();

protected:
	bool eventFilter (QObject* watched, QEvent* event) override;

private:
	enum class Dimensions
	{
		Zero,
		Two,
		Three
	};

	QElapsedTimer m_timer;
	Dimensions    dimensions = Dimensions::Zero;

	float     zoom_offset = 0.0f;
	float     zoom_delta = 0.0f;
	QVector3D move_direction;
	QVector2D last_mouse_position;
	QVector2D pan_direction;

	float get_zoom();
	float get_zoom_factor();

	void update_uniforms();
	void update_zoom (float delta_time);
	void update_position (float delta_time);
	void update_position_2d (QVector3D const& move_offset, Uniform& position);
	void update_position_3d (QVector3D const& move_offset, Uniform& position);

	void update_view (float delta_time);
	void update_view_2d (QVector2D const& offset);
	void update_view_3d (QVector2D const& offset);

	void update_move_direction (QKeyEvent const& key_event);
	void update_move_direction_2d (QKeyEvent const& key_event);
	void update_move_direction_3d (QKeyEvent const& key_event);

	void update_pan_direction (QMouseEvent const& mouse_event);
	void update_zoom_offset (QWheelEvent const& wheel_event);
};