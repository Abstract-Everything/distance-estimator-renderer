import renderer.inspector 1.0

import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4

Inspector_
{
  id: inspector
  
  ComboBox
  {
    id: shader_selector
	
	width: parent.width
	
	model: inspector.shaders
	onCurrentIndexChanged: inspector.update_shader(currentIndex)
  }
  
  TabView
  {
    id: uniform_fields
  
    width: parent.width
    anchors.top: shader_selector.bottom
    anchors.bottom: parent.bottom
    
    function create_uniform_fields()
    {
  	  while (uniform_fields.getTab(0) != null)
  	  {
  	    uniform_fields.removeTab(0);
  	  }
  	  
  	  var tabs_source = inspector.create_uniforms_qml_source();
  	  tabs_source.forEach(tab_source =>
  	    Qt.createQmlObject(tab_source, uniform_fields, "Uniform Fields"));
		
      {
        // Hack, without this some variables are not found when querying them, this "loads" all properties
        for (var i = 0; i < uniform_fields.count; ++i)
		{
          uniform_fields.currentIndex = i;
		}

        uniform_fields.currentIndex = 0;
      }
    }
      
    Component.onCompleted: 
	{
	  uniform_fields.create_uniform_fields();
	  inspector.uniforms_changed.connect(create_uniform_fields);
	}
  }
}