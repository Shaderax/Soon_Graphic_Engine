#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 2) uniform sampler2D texSampler;

layout(binding = 0, set = 3) uniform UniformMaterial
{
	vec3	_ambient;
	vec3	_diffuse;
	vec3	_specular;
	float	_shininess;
} um;

layout(binding = 0, set = 4) uniform UniformLight
{
	vec3 _direction;
	vec3 _lightColor;
	float _intensity;
} ul;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	vec3 ambient = ul._intensity * ul._lightColor;

	vec3 norm = normalize(fragNormal);
	float diff = max(dot(norm, ul._direction), 0.0);
	vec3 diffuse = diff * ul._lightColor;

	outColor = vec4(ambient + diffuse, 1.0f) * texture(texSampler, fragTexCoord);

///
//	float specularStrength = 1.0;

//	vec3 reflectDir = reflect(-ul._lightColor, norm);  
//	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//	if (diff <= 0.0)
//		spec = 0.0;
//	vec3 specular = specularStrength * spec * vec3(1.0f, 1.0f, 1.0f);  
//
//	vec3 res = (ambient + diffuse + specular) * color_obj;
//	Color = texture(texture1, TexCoord) * vec4(res, 1.0f);
}
