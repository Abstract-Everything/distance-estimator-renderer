#include "3d/ray_march.frag"
#include "3d/colouring.frag"

struct Cuboid
{
  vec3 origin = (0.0f, 0.0f, 0.0f);
  vec3 forward = (0.0f, 0.0f, 1.0f);
  vec3 world_up = (0.0f, 1.0f, 0.0f);
  vec3 size = (1.0f, 1.0f, 1.0f);
};

uniform Cuboid cube;

vec3 project (vec3 relative)
{
  vec3 forward = normalize (cube.forward);
  vec3 right = normalize (cross (cube.world_up, forward));
  vec3 up = normalize (cross (forward, right));

  return vec3 (
    dot (relative, right),
    dot (relative, up),
    dot (relative, forward));
}

float DE (vec3 position)
{
  vec3 relative = position - cube.origin; 
  vec3 projection = abs (project (relative));
  vec3 offset = projection - cube.size;
  return length (max (offset, 0.0f));
}
