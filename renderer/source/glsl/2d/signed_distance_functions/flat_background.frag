#vertex_shader "2d/complex_plane.vert"

#include "structures.glsl"

uniform Colouring colouring;

out vec4 fragment_colour;

void main()
{
	fragment_colour = vec4 (colouring.background_colour, 1.0f);
}
