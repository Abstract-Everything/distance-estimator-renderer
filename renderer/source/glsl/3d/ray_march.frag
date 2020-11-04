#requires_implementation
#vertex_shader "3d/camera_frame.vert"

#include "structures.glsl"
#include "3d/utility_3d.frag"

struct Ray_Marcher
{
	uint  max_steps	   = 50;
	uint  max_ray_hits = 2;
	float hit_distance = 0.01f;
};

struct Material
{
	float specular_intensity = 0.0f;
};

uniform Fragment_Globals f_globals;
uniform Camera_3d		 camera;
uniform Ray_Marcher		 ray_marcher;
uniform Material		 material;

in vec3 f_ray_position;
in vec3 f_ray_direction;

out vec4 fragment_colour;

vec3 march (vec3 origin, vec3 direction)
{
	uint  steps            = 0u;
	uint  current_ray_hits = 0u;

	vec3  position         = origin;
	float closest_distance = f_globals.world_size;

	vec3  colour			= vec3(0.0f);
	float colour_multiplier = 1.0f;

	for (steps = 0u; steps < ray_marcher.max_steps && current_ray_hits < ray_marcher.max_ray_hits; ++steps)
	{
		float distance = DE (position);
		closest_distance = min (distance, closest_distance);
		position         += direction * distance;

		if (ray_marcher.hit_distance > distance)
		{
			current_ray_hits += 1u;

			float colour_intensity = colour_multiplier * (1.0f - material.specular_intensity);
			colour_multiplier -= colour_intensity;

			colour += colour_intensity * diffuse_colour (steps, ray_marcher.max_steps, closest_distance, direction, position);
			closest_distance = f_globals.world_size;

			direction = reflect_ray(direction, position);
			position += 2.0f * ray_marcher.hit_distance * direction;
		}
	}

	return colour + colour_multiplier * background_colour (direction);
}

void main()
{
	vec3 colour     = march (f_ray_position, normalize (f_ray_direction));
	fragment_colour = vec4 (abs (colour), 1.0f);
}
