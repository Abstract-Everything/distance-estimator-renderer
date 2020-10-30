#version 300 es
precision mediump float;

struct Vertex_Globals
{
	uvec2 resolution;
};

struct Fragment_Globals
{
	float time = 0.0f;
	float world_size = 2.0f;
};

struct Camera_3d
{
	vec3  position   = (0.0f, 0.0f, -3.0f);
	float roll       = 0.0f;
	float yaw        = 0.0f;
	float pitch      = 0.0f;
	float fov        = 1.57079632679;
	float near_plane = 0.5f;
	float zoom       = 0.0f;
};

struct Camera_2d
{
	vec2  position = (0.0f, 0.0f);
	float zoom     = 0.0f;
};

struct Colouring
{
	vec3 background_colour = (0.4f, 0.4f, 0.4f);
};
