#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformCamera
{
	mat4 view;
	mat4 proj;
} uc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragTexCubeCoord;

void main()
{
	fragTexCubeCoord = inPosition;
	gl_Position = uc.proj * uc.view * vec4(inPosition, 1.0);
}