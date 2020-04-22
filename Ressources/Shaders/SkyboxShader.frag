#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 1) uniform samplerCube texSampler;

layout(location = 0) in vec3 fragTexCubeCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(1.0f) * texture(texSampler, fragTexCubeCoord);
}
