#include "3d/ray_march.frag"
#include "3d/colouring.frag"

struct Sphere
{
  vec3 origin = (0.0f, 0.0f, 0.0f);
  float radius = 0.5f;
};

uniform Sphere sphere;

float DE (vec3 position)
{
  return abs(length(position - sphere.origin) - sphere.radius);
}
