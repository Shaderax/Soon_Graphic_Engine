#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec2 inCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inFragPos;

layout(set = 0, binding = 1) uniform Light
{
	vec3 viewPos;
	vec3 lightPos;
} l;

layout(set = 1, binding = 1) uniform sampler2D latexture;
layout(set = 1, binding = 2) uniform ColorReact
{
	vec3 ambient;
	float shininess;
} cr;


layout(location = 0) out vec4 outColor;

void main()
{
	float ambientStrength = 0.2;
	vec3 ambient = cr.ambient * ambientStrength;

	vec3 norm = normalize(inNormal);
	vec3 lightDir = normalize(l.lightPos - inFragPos); 
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(1.0f);

	float specularStrength = 0.5;
	vec3 viewDir = normalize(l.viewPos - inFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), cr.shininess);
	vec3 specular = specularStrength * spec * vec3(1.0f);  

	outColor = vec4(ambient + diffuse + specular, 1.0f);//vec4(gl_FragCoord.w * 70);// * vec4(0.51f, 0.678f, 0.79f, 1.0f);
	//if (outColor.w < 1)
	//	discard;
}
