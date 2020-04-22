#version 410 core

layout (location = 0) in vec3 vertex;

uniform vec2 o_color;
uniform mat4 mvp;

out	vec3 color_obj;

void main()
{
	color_obj = vec3(vertex.x * o_color.x, vertex.y * o_color.y, vertex.z * o_color.x);

	gl_Position = mvp * vec4(vertex, 1.0f);
}
