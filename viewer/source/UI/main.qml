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

  minimumWidth: 640
  minimumHeight: 480

  width: 640
  height: 480
  
  SplitView
  {
	anchors.fill: parent
	orientation: Qt.Horizontal
	
    Viewport
    {
	  id: viewport
	  
	  Layout.fillWidth: true
	  Layout.minimumWidth: 160
    }
	
	Inspector
	{
	  id: inspector
	  
	  Layout.minimumWidth: 160
	}
  }
}
