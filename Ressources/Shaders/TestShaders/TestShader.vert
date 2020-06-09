#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Particle
{
	vec3 position;
	vec3 velocity;
};

layout(binding = 0, set = 0) buffer Particles
{
	Particle particles[ ];
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 outCoord;
layout(location = 1) out vec3 outNormal;

void main()
{
	gl_Position = vec4(inPosition.x - 0.5f, inPosition.y - 0.5f, inPosition.z - 0.2f, 1.0);;
	outCoord = inTexCoord;
	outNormal = inNormal;
}
