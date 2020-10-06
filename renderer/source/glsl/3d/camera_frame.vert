#include "structures.glsl"
#include "constants.glsl"

uniform Camera_3d camera;

layout (location = 0) in vec2 v_position;

out vec3 f_ray_position;
out vec3 f_ray_direction;

void main()
{
	float aspect = float (globals.resolution.y) / float (globals.resolution.x);
	float zoom  = clamp (camera.zoom * -1.0f + 1.0f, min_zoom, 2.0f);
	float width = tan (camera.fov / 2.0f) * camera.near_plane * zoom;

	vec3 forward  = normalize (vec3 (
		  sin (camera.yaw)
		, sin (camera.pitch)
		, cos (camera.yaw)
		)
	);

	vec3 right = normalize (cross (vec3(0.0f, 1.0f, 0.0f), forward));
	vec3 up	   = normalize (cross (forward, right));

	f_ray_direction =
		forward
		+ width * v_position.x * right
		+ width * aspect * v_position.y * up;

	f_ray_position  = camera.position;

	gl_Position = vec4 (v_position, 0.0f, 1.0f);
}
