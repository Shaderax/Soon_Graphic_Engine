#version 410 core

uniform vec3 ambient_light;
uniform vec3 pos_light;
uniform vec3 camPos;

in vec3 color_obj;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D texture1;

out vec4 Color;

void main()
{
	// Ambient
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * vec3(1.0f, 1.0f, 1.0f);

	// Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(pos_light - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = vec3(1.0f, 1.0f, 1.0f) * diff;

	// Specular
	float specularStrength = 1.0;
	vec3 viewDir = normalize(camPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	if (diff <= 0.0)
		spec = 0.0;
	vec3 specular = specularStrength * spec * vec3(1.0f, 1.0f, 1.0f);  

	vec3 res = (ambient + diffuse + specular) * color_obj;
	Color = texture(texture1, TexCoord) * vec4(res, 1.0f);
}
