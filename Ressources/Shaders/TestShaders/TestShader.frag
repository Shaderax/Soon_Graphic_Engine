#version 450
#extension GL_ARB_separate_shader_objects : enable

//layout(set = 1, binding = 0) uniform sampler2D latexture;
layout(set = 0, binding = 1) uniform Bite
{
	vec3 caca;
} oui;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(0.51f, 0.678f, 0.79f, 1.0f);
}
