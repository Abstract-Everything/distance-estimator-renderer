#requires_implementation
#include "structures.glsl"

uniform Colouring colouring;

uint  get_iterations();
uint  get_max_iterations();
float get_step();

vec3 colour (float distance, float hit_distance)
{
	float co = (float (get_iterations()) + 1.0 - log2 (0.5 * log2 (get_step())))
			   / float (get_max_iterations());
	vec3 colour = vec3 (6.2831 * sqrt (co))
				  + abs (colouring.background_colour - vec3 (globals.time));
	return 0.5f
		   * vec3 (
			   abs (cos (colour.x)),
			   abs (cos (colour.y)),
			   abs (cos (colour.z)));
}
