#include "3d/colouring.frag"
#include "3d/ray_march.frag"

struct Plane
{
	vec3 position = (0.0f, 0.0f, 0.0f);
	vec3 facing = (0.0f, 0.0f, 1.0f);
	vec3 right = (1.0f, 0.0f, 0.0f);
	float width = 1.0f;
	float height = 1.0f;
};

uniform Plane plane;

vec3 project(vec3 component, float component_length, vec3 relative)
{
	component = normalize (component);
	float projected = dot (relative, component);
	float distance = min (component_length, abs (projected));
	return sign (projected) * distance * component;
}

float DE (vec3 position)
{
	vec3 relative = position - plane.position;
	vec3 up = normalize (cross (plane.right, plane.facing));
	vec3 closest = project (plane.right, plane.width, relative)
				   + project (up, plane.height, relative);

	return length (relative - closest);
}