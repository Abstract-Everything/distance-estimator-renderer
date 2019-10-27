#include "2d/complex_plane.frag"
#include "structures.glsl"
#include "utility.frag"

struct Mandelbroth
{
  uint iterations = 1000;
};

uniform Mandelbroth mandelbroth;

uint iterations;
vec2 step;

float DE(vec2 position)
{
  step = vec2 (0.0f, 0.0f);
  vec2 d_step = vec2 (1.0f, 0.0f);

  for (iterations = 0u; iterations < mandelbroth.iterations; ++iterations)
  {
    d_step = 2.0f * complex_multiplication (step, d_step) + 1.0;
    step = complex_multiplication(step, step) + position;
    if (dot (step, step) > 5.0f) break;
  }
  if (iterations == mandelbroth.iterations) return 0.0f;

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
  return mandelbroth.iterations;
}

float get_step ()
{
  return dot (step, step);
}
