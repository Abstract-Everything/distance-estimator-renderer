#requires_implementation

uniform Colouring colouring;

float DE (vec3 position);
vec3 normal_estimator (vec3 direction, vec3 position, float distance)
{
  vec3 offset = vec3 (distance * 0.5, 0.0, 0.0);
  position -= direction * distance * 0.5f;
  return normalize(vec3(
    DE(position + offset.xyy) - DE(position - offset.xyy),
    DE(position + offset.yxy) - DE(position - offset.yxy),
    DE(position + offset.yyx) - DE(position - offset.yyx)
  ));
}

vec3 background_colour (vec3 position, vec3 direction)
{
  return (0.2f * direction.y) + colouring.background_colour;
}

float ambient_occlusion (uint steps, uint max_iterations)
{
  return 1.0f - float(steps) / float(max_iterations);
}

float glow (float closest_distance)
{
  float x = min (closest_distance, 1.0f);
  return -log (x / 1.1f + 0.1f);
}

vec3 colour_fragment (bool hit, vec3 origin, vec3 direction, vec3 position)
{
  vec3 colour = background_colour(origin, direction);

  if (hit)
  {
    vec3 normal = vec3(normal_estimator(direction, position, 1e-06));
    float ambient = ambient_occlusion(steps, ray_marcher.max_steps);
    colour = vec3 ((0.6f + 0.4f * ambient) * dot (-direction, normal)); 
  }

  return 0.9f * colour + glow (closest_distance) * 0.1f;
}

