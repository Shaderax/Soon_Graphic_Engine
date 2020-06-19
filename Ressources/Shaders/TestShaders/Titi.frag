#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 1) buffer Truc
{
	int prout;
	int ici[];
	int ii[];
} ouais;

layout(set = 0, binding = 1) uniform Truc
{
	vec3 iui;
	int ici[];
	int ci[];
} ok;

layout(location = 0) in vec2 inCoord;
layout(location = 1) in vec2 inNormal;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(0.51f, 0.678f, 0.79f, 1.0f);
}
