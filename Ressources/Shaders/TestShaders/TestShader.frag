#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 1) uniform sampler2D latexture;
/*
layout(set = 0, binding = 1) buffer Bu
{
	float arr[][10];
} ba[40];
*/
//layout(set = 0, binding = 1) uniform sampler2DArray jkjkatexture;

layout(location = 0) in vec2 inCoord;
//layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

float rand(float x)
{
	return fract(sin(x)*1.0);
}

void main()
{
	outColor = texture(latexture, inCoord);// * vec4(0.51f, 0.678f, 0.79f, 1.0f);
	//if (outColor.w < 1)
	//	discard;
}
