import renderer.viewport 1.0
import renderer.screen_input 1.0

import QtQuick 2.2

Viewport_
{
	id: viewport

	Screen_Input
	{
		id: screen_input
		anchors.fill: parent
	}

	Component.onCompleted: viewport.set_screen_input(screen_input)
}
