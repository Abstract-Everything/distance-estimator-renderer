import renderer.main_window 1.0
import renderer.viewport 1.0
import renderer.inspector 1.0

import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

Main_Window_
{
	id: main_window
	visible: true

	title: qsTr ("Fractal renderer")

	minimumWidth: 1280
	minimumHeight: 720

	// If these are not specified the split initialises incorrectly
	width:  1280
	height: 720

	SplitView
	{
		anchors.fill: parent
		orientation: Qt.Horizontal

		Viewport
		{
			width: main_window.width / 2
			Layout.minimumWidth: 256
		}

		Inspector
		{
			width: main_window.width / 2
			Layout.minimumWidth: 256
		}
	}
}
