#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 1) uniform sampler2D latexture;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(0.51f, 0.678f, 0.79f, 1.0f);
}
