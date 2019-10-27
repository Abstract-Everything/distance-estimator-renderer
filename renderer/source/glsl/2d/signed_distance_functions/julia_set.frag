#include "2d/complex_plane.frag"
#include "structures.glsl"
#include "utility.frag"

struct Julia_Set
{
  uint iterations = 1000;
  vec2 constant = (0.0f, 0.0f);
};

uniform Julia_Set julia_set;

uint iterations;
vec2 step;

float DE(vec2 position)
{
  step = position;
  vec2 d_step = vec2 (1.0f, 0.0f);

  for (iterations = 0u; iterations < julia_set.iterations; ++iterations)
  {
    d_step = 2.0f * complex_multiplication (step, d_step);
    step = complex_multiplication(step, step) + julia_set.constant;
    if (dot (step, step) > 5.0f) break;
  }
  if (iterations == julia_set.iterations) return 0.0f;

  float r = length (step);
  float dr = length (d_step);

  return 0.5f * r * log(r) / dr;
}

uint get_iterations ()
{
  return iterations;
}

uint get_max_iterations ()
{
  return julia_set.iterations;
}

float get_step ()
{
  return dot (step, step);
}
