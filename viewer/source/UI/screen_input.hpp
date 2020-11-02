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
	QVector2D           pan_direction() const;
	float               zoom_direction() const;
	void                reset_input();

signals:
	void input_updated();

protected:
	bool eventFilter (QObject* watched, QEvent* event) override;

private:
	QMap<Qt::Key, bool> m_move_keys_pressed;
	QVector2D           m_last_mouse_position;
	QVector2D           m_pan_direction;
	float               m_zoom_direction = 0.0f;

	void reset_move_direction();
	void update_move_direction (QKeyEvent const& key_event);
	void update_pan_direction (QMouseEvent const& mouse_event);
	void update_zoom_direction (QWheelEvent const& wheel_event);
};
