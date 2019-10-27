#requires_implementation
#vertex_shader "3d/camera_frame.vert"

#version 300 es
precision mediump float;

#include "structures.glsl"

struct Ray_Marcher
{
  uint max_steps = 50;
  float hit_distance = 0.01f;
};

uniform Camera_3d camera;
uniform Ray_Marcher ray_marcher;
 
in vec3 f_ray_position;
in vec3 f_ray_direction;

out vec4 fragment_colour;

uint steps = 0u;
float total_distance = 0.0f;
float last_distance = 0.0f;;
float closest_distance = 0.0f;

float DE (vec3 position);
vec3 colour_fragment (bool hit, vec3 origin, vec3 direction, vec3 position);

vec3 march(vec3 origin, vec3 direction)
{
  bool hit = false;
  vec3 position = origin;
  closest_distance = globals.world_size;

  for (steps = 0u; steps < ray_marcher.max_steps; ++steps)
  {
    float distance = DE(position);
    total_distance += distance;
    closest_distance = closest_distance > distance ? distance : closest_distance;
    position = origin + direction * total_distance;
    hit = ray_marcher.hit_distance > distance;
    if (hit) break;
  }

  return colour_fragment(hit, origin, direction, position);
}

void main()
{
  vec3 colour = march(f_ray_position, normalize(f_ray_direction));
  fragment_colour = vec4(abs(colour), 1.0f);
}
