import renderer.screen_input 1.0

import QtQuick 2.2

Screen_Input_
{
	id: screen_input

	MouseArea
	{
		id: mouse_area
		anchors.fill: parent

		hoverEnabled: true
		Component.onCompleted: screen_input.set_event_filter (mouse_area)
	}
}
