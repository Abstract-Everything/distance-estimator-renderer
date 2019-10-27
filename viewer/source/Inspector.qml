import renderer.inspector 1.0

import QtQuick 2.0
import QtQuick.Controls 1.4

Inspector_Class
{
  id: inspector

  ComboBox
  {
    id: shader_selector

    width: parent.width

    model: inspector.shaders
    onCurrentIndexChanged: inspector.shader_selection_changed (inspector.shaders[currentIndex])
  }

  TabView
  {
    id: variable_fields

    width: parent.width
    anchors.top: shader_selector.bottom
    anchors.bottom: parent.bottom

    function create_variable_fields ()
    {
      while (variable_fields.getTab (0) != null)
        variable_fields.removeTab (0);

      var tabs_source = inspector.uniforms_source;
      tabs_source.forEach (tab_source =>
          Qt.createQmlObject (tab_source, variable_fields, "Variable Fields"));

      {
        // Hack, without this some variables are not found when querying them, this "loads" all properties
        for (var i = 0; i < variable_fields.count; ++i)
          variable_fields.currentIndex = i;

        variable_fields.currentIndex = 0;
      }
    }

    Component.onCompleted: inspector.uniforms_changed.connect (create_variable_fields)
  }
}
