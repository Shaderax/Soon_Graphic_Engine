#version 450

layout (binding = 0, set = 0) uniform UniformCamera
{
	mat4 view;
	mat4 proj;
} uc;

layout (location = 0) in vec3 inPosition;

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
};

void main() 
{
	gl_PointSize = 1.0f;
	gl_Position = uc.proj * uc.view * vec4(inPosition, 1.0);
}
