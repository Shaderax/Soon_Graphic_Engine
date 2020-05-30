#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 1) uniform sampler2D latexture;

layout(location = 0) in vec2 inCoord;
layout(location = 1) in vec2 inNormal;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = texture(latexture, inCoord);// * vec4(0.51f, 0.678f, 0.79f, 1.0f);
	//outColor = vec4(1.0f);//texture(latexture, inCoord);
}
