#requires_implementation
#vertex_shader "3d/camera_frame.vert"

#include "structures.glsl"

struct Ray_Marcher
{
	uint  max_steps    = 50;
	float hit_distance = 0.01f;
};

uniform Camera_3d   camera;
uniform Ray_Marcher ray_marcher;

in vec3 f_ray_position;
in vec3 f_ray_direction;

out vec4 fragment_colour;

float DE (vec3 position);
vec3  colour_fragment (uint steps, uint max_steps, float closest_distance, bool hit, vec3 origin, vec3 direction, vec3 position);

vec3 march (vec3 origin, vec3 direction)
{
	bool  hit              = false;
	vec3  position         = origin;
	float closest_distance = globals.world_size;
	float total_distance   = 0.0f;
	float last_distance    = 0.0f;
	uint  steps            = 0u;

	for (steps = 0u; steps < ray_marcher.max_steps && !hit; ++steps)
	{
		float distance = DE (position);
		total_distance += distance;
		closest_distance = min (distance, closest_distance);
		position         = origin + direction * total_distance;
		hit              = ray_marcher.hit_distance > distance;
	}

	return colour_fragment (steps, ray_marcher.max_steps, closest_distance, hit, origin, direction, position);
}

void main()
{
	vec3 colour     = march (f_ray_position, normalize (f_ray_direction));
	fragment_colour = vec4 (abs (colour), 1.0f);
}
