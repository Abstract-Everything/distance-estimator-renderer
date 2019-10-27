#requires_implementation

vec2 complex_multiplication (vec2 a, vec2 b)
{
  return vec2 (a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

float random (vec2 co)
{
  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}
