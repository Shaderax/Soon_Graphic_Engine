#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 4) uniform sampler2D texSamplere;

layout(set = 0, binding = 0) uniform UniformCamera
{
	mat4 view;
	mat4 proj;
} uc;

layout(set = 1, binding = 0) uniform UniformModel
{
	mat4 model;
} um;

layout(set = 5, binding = 0) uniform UniformModelee
{
	mat4 model;
} ume[52];

layout(set = 5, binding = 0) uniform UniformModeleee
{
	mat4 model;
} umi;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;

void main()
{
	gl_Position = uc.proj * uc.view * um.model * vec4(inPosition, 1.0);
	fragNormal = inNormal;
	fragTexCoord = inTexCoord;
}
