#version 410 core

uniform vec3 ambient_light;

in vec3 color_obj;

out vec4 Color;

void main()
{
	// Ambient
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * vec3(1.0f, 1.0f, 1.0f);

	vec3 res = (ambient) * color_obj;
	Color = vec4(res, 1.0f);
}
