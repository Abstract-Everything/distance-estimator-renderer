#requires_implementation

float DE (vec3 position);
vec3  normal_estimator (vec3 direction, vec3 position, float distance)
{
	vec3 offset = vec3 (distance * 0.5, 0.0, 0.0);
	position -= direction * distance * 0.5f;
	return normalize (vec3 (
		DE (position + offset.xyy) - DE (position - offset.xyy),
		DE (position + offset.yxy) - DE (position - offset.yxy),
		DE (position + offset.yyx) - DE (position - offset.yyx)));
}

vec3 reflect_ray (vec3 direction, vec3 position)
{
	vec3 normal = normal_estimator (direction, position, 1e-06);
	vec3 parallel_to_normal = dot (-direction, normal) * normal;
	return direction + 2.0f * parallel_to_normal;
}
