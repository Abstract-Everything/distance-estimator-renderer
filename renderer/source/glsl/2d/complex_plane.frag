#requires_implementation
#vertex_shader "2d/complex_plane.vert"

#include "2d/colouring.frag"
#include "structures.glsl"

struct Complex_Plane
{
	float hit_distance = 0.0001;
};

uniform Complex_Plane complex_plane;

uniform Fragment_Globals f_globals;
uniform Camera_2d camera;

in vec2 f_position;

out vec4 fragment_colour;

float DE (vec2 position);
vec3  colour (float distance, float hit_distance);

vec3 shade (vec2 position)
{
	return colour (DE (position), complex_plane.hit_distance, f_globals.time);
}

void main()
{
	vec3 colour     = shade (f_position + camera.position);
	fragment_colour = vec4 (abs (colour), 1.0f);
}
