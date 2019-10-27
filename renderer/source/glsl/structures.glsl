struct Globals
{
  float time;
  uvec2 resolution;
  float world_size = 2.0f;
};

uniform Globals globals;

struct Camera_3d
{
  vec3 position    = (0.0f, 0.0f, -3.0f);
  vec3 forward     = (0.0f, 0.0f, 1.0f);
  float fov        = 3.1415926535897932384;
  float near_plane = 1.0f;
};

struct Camera_2d
{
  vec2 position = (0.0f, 0.0f);
  float zoom = 0.0f;
};
