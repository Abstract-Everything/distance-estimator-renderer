import renderer.camera 1.0

import QtQuick 2.2

Camera_
{
	id: camera

	Component.onCompleted: camera.initialise_signals()

	MouseArea
	{
		id: mouse_area
		anchors.fill: parent

		hoverEnabled: true
		Component.onCompleted: camera.set_event_filter (mouse_area)
	}
}
