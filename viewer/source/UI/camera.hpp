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

	const float move_speed = 1.0f;

	QElapsedTimer m_timer;
	Dimensions    dimensions = Dimensions::Zero;

	float     zoom_before = 0.0f;
	float     zoom_after  = 0.0f;

	QMap<Qt::Key, bool> move_keys_pressed;

	QVector2D last_mouse_position;
	QVector2D pan_direction;

	float get_zoom();
	void  set_zoom (float zoom);
	float get_zoom_factor();

	std::tuple<QVector3D, QVector3D, QVector3D> get_basis();

	void  update_uniforms();
	void  update_zoom (float delta_time);
	void  update_position (float delta_time);
	void  update_position_2d (float multiplier, Uniform& position);
	void  update_position_3d (float multiplier, Uniform& position);
	float determine_move_direction (Qt::Key forward, Qt::Key backwards);

	void update_view (float delta_time);
	void update_view_2d (QVector2D const& offset);
	void update_view_3d (QVector2D const& offset);

	void  reset_move_direction();
	void  update_move_direction (QKeyEvent const& key_event);

	void update_pan_direction (QMouseEvent const& mouse_event);
	void update_zoom_offset (QWheelEvent const& wheel_event);
};