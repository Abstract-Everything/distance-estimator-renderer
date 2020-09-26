#pragma once

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

	QVector3D move_direction;
	QVector2D last_mouse_position;
	QVector2D pan_direction;

	void update_uniforms();
	void update_position (float deltaTime);
	void update_view (float deltaTime);

	void update_move_direction (QKeyEvent const& key_event);
	void update_move_direction_2d (QKeyEvent const& key_event);
	void update_move_direction_3d (QKeyEvent const& key_event);

	void update_pan_direction (QMouseEvent const& mouse_event);
};