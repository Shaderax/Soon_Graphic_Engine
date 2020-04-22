#version 410 core

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec2 text_coord;

uniform vec2 v_color;
uniform mat4 mvp;

out	vec3 color_obj;
out vec2 TexCoord;

void main()
{
	TexCoord = vec2(text_coord.x, text_coord.y);

	color_obj = vertex * v_color[0] + v_color[1];

	gl_Position = mvp * vec4(vertex, 1.0f);
}
