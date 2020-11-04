#requires_implementation

#include "structures.glsl"
#include "3d/utility_3d.frag"

uniform Colouring colouring;

vec3 background_colour (vec3 direction)
{
	return (0.2f * direction.y) + colouring.background_colour;
}

float ambient_occlusion (uint steps, uint max_iterations)
{
	return 1.0f - float (steps) / float (max_iterations);
}

float glow (float closest_distance)
{
	float x = min (closest_distance, 1.0f);
	return -log (x / 1.1f + 0.1f);
}

vec3 diffuse_colour (uint steps, uint max_steps, float closest_distance, vec3 direction, vec3 position)
{
	vec3  normal  = vec3 (normal_estimator (direction, position, 1e-06));
	float ambient = ambient_occlusion (steps, max_steps);
	vec3 colour = vec3 ((0.6f + 0.4f * ambient) * dot (-direction, normal));
	return 0.9f * colour + glow (closest_distance) * 0.1f;
}
