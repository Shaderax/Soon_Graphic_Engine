#version 410 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 anormal;

uniform vec2 o_color;
uniform mat4 mvp;
uniform mat4 model;

out	vec3 color_obj;
out vec3 Normal;
out vec3 FragPos;

void main()
{
	FragPos = vec3(model * vec4(vertex, 1.0));
	Normal =/* mat3(transpose(inverse(model))) * */ anormal;  

	color_obj = vec3(0.8f, 0.8f, 0.8f);

	gl_Position = mvp * vec4(vertex, 1.0f);
}
