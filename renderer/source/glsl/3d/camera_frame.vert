#version 300 es
precision highp float;

#include "structures.glsl"

uniform Camera_3d camera;
 
layout(location = 0) in vec2 v_position;

out vec3 f_ray_position;
out vec3 f_ray_direction;
 
void main()
{
  vec3 world_up = vec3 (0.0f, 1.0f, 0.0f);
  float aspect = float(globals.resolution.y) / float (globals.resolution.x);
  float distance = atan(camera.fov) * camera.near_plane;

  vec3 forward = normalize (camera.forward) * camera.near_plane;
  vec3 right = normalize (cross (world_up, forward)) * distance;
  vec3 up = normalize (cross (forward, right)) * distance * aspect;

  f_ray_direction = forward + right * v_position.x + up * v_position.y;
  f_ray_position = camera.position;

  gl_Position = vec4(v_position, 0, 1);
}
