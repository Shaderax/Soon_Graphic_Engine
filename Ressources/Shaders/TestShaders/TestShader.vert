#version 450
#extension GL_ARB_separate_shader_objects : enable
/*
struct Another
{
	int value;
	float valuefl;
};

struct Particle
{
	float x;
	float y;
	Another ano;
};

layout(set = 0, binding = 1) buffer Bu
{
	Particle particles[];
	Another parti[];
};

layout(set = 0, binding = 2) uniform Truc
{
	int two;
	float ok[];
	//float ok[10];
};

layout(set = 0, binding = 3) uniform Wallah
{
	float one;	
} ko[];
*/

layout(set = 0, binding = 0) uniform UniformCamera
{
	mat4 view;
	mat4 proj;
} uc;

layout(set = 1, binding = 0) uniform UniformModel
{
	mat4 model;
} um;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

//layout(location = 2) in vec3 Pos;

layout(location = 0) out vec2 outCoord;
//layout(location = 1) out vec3 outNormal;

void main()
{
	gl_Position = uc.proj * uc.view * um.model * vec4(inPosition/* + Pos*/, 1.0);// + Pos, 1.0);
	//vec3 i = Pos;
	outCoord = inTexCoord;
	//outNormal = inNormal;
}