#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform Coucou
{
	vec3 bondour;
} cou;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outCoord;

void main()
{
	gl_Position = vec4(inPosition.x + cou.bondour.x, inPosition.y + cou.bondour.y, 0, 1.0);
	outCoord = inTexCoord;
}
