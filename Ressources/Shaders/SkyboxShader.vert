#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformCamera
{
	mat4 view;
	mat4 proj;
} uc;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTexCoord;

layout(location = 0) out vec3 fragTexCubeCoord;

void main()
{
	fragTexCubeCoord = inPos;
	vec3 position = mat3(uc.view) * inPos;
	gl_Position = uc.proj * vec4(position, 1.0f);
}
