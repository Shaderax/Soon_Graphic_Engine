#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform Coucou
{
	vec3 bondour;
} cou;

layout(location = 0) in vec3 inPosition;

void main()
{
	gl_Position = vec4(inPosition.x + cou.bondour.x, inPosition.y + cou.bondour.y, inPosition.z, 1.0);
}
