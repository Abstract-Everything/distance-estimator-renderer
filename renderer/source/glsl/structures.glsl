#version 300 es
precision mediump float;

struct Globals
{
	float time;
	uvec2 resolution;
	float world_size = 2.0f;
};

uniform Globals globals;

struct Camera_3d
{
	vec3  position   = (0.0f, 0.0f, -3.0f);
	float roll       = 0.0f;
	float yaw        = 0.0f;
	float pitch      = 0.0f;
	float fov        = 3.1415926535897932384;
	float near_plane = 1.0f;
	float zoom       = 1.0f;
};

struct Camera_2d
{
	vec2  position = (0.0f, 0.0f);
	float zoom     = 1.0f;
};

struct Colouring
{
	vec3 background_colour = (0.4f, 0.4f, 0.4f);
};
