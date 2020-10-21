#pragma once

#include <QKeyEvent>
#include <QMap>
#include <QMouseEvent>
#include <QQuickItem>
#include <QVector2D>
#include <QWheelEvent>

class Screen_Input : public QQuickItem
{
	Q_OBJECT

public:
	Screen_Input(QQuickItem* parent = nullptr);

	Q_INVOKABLE void set_event_filter (QObject* mouse_area);

	QMap<Qt::Key, bool> move_keys() const;
	QVector2D           get_and_reset_pan_direction();
	float               get_and_reset_zoom_direction();

protected:
	bool eventFilter (QObject* watched, QEvent* event) override;

private:
	QMap<Qt::Key, bool> move_keys_pressed;
	QVector2D           last_mouse_position;
	QVector2D           pan_direction;
	float               zoom_direction = 0.0f;

	void reset_move_direction();
	void update_move_direction (QKeyEvent const& key_event);
	void update_pan_direction (QMouseEvent const& mouse_event);
	void update_zoom_direction (QWheelEvent const& wheel_event);
};
