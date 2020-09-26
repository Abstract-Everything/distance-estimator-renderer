#include "structures.glsl"

uniform Camera_3d camera;

layout (location = 0) in vec2 v_position;

out vec3 f_ray_position;
out vec3 f_ray_direction;

void main()
{
	float aspect = float (globals.resolution.y) / float (globals.resolution.x);
	float distance = atan (camera.fov) * camera.near_plane * camera.zoom;

	vec3 above  = vec3 (0.0f, cos (camera.pitch), -sin (camera.pitch));
	vec3 toward = vec3 (sin (camera.yaw), 0.0f, cos (camera.yaw));

	vec3 right   = normalize (cross (above, toward)) * distance;
	vec3 forward = normalize (cross (right, above)) * camera.near_plane;
	vec3 up      = normalize (cross (forward, right)) * distance * aspect;

	f_ray_direction = forward + right * v_position.x + up * v_position.y;
	f_ray_position  = camera.position;

	gl_Position = vec4 (v_position, 0, 1);
}
