#version 410 core

uniform vec3 ambient_light;
uniform vec3 camPos;

in vec3 color_obj;
in vec2 TexCoord;

uniform sampler2D texture1;

out vec4 Color;

void main()
{
	// Ambient
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * vec3(1.0f, 1.0f, 1.0f);

	vec3 res = (ambient) * color_obj;
	Color = texture(texture1, TexCoord) * vec4(res, 1.0f);
	Color = vec4(res, 1.0f);
}
