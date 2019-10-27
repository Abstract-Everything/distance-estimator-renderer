import renderer.main_window 1.0

import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

Main_Window_Class
{
  id: main_window
  objectName: "main_window"
  visible: true

  title: qsTr ("Fractal renderer")

  minimumWidth: 640
  minimumHeight: 480

  width: 1920
  height: 1080

  SplitView
  {
    anchors.fill: parent
    orientation: Qt.Horizontal

    Viewport
    {
      id: viewport
      objectName: "viewport"

      Layout.fillWidth: true
      Layout.minimumWidth: 160
    }

    Inspector
    {
      id: inspector
      objectName: "inspector"

      Layout.minimumWidth: 160
    }
  }
}
