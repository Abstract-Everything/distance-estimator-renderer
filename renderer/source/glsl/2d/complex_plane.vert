#version 300 es
precision highp float;

#include "structures.glsl"

uniform Camera_2d camera;

layout(location = 0) in vec2 v_position;

out vec2 f_position;

void main()
{
  float aspect = float(globals.resolution.y) / float (globals.resolution.x);
  float screen_width = float (globals.resolution.x) / 2048.0f;
  float frame_width = max (screen_width * camera.zoom, 0.000001f);

  float width = v_position.x * frame_width;
  float height = v_position.y * frame_width * aspect;
  f_position = vec2(width, height);

  gl_Position = vec4(v_position, 0, 1);
}
