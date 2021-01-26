#version 330 core
out vec4 FragColor;

struct Material {
	// ambient
    vec3 ambient;

	bool useDiffMap;
    vec3 diffuse;
    sampler2D diffuseMap;

	bool useSpecMap;
    vec3 specular;
    sampler2D specularMap;
	float shininess;
	float specularStrength;

	float opacity;
};

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
    vec3 position;
	
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 diffuse;
	vec3 specular;
};

#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 1

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform Material material;

void DirLightContrib(DirLight light, vec3 normal, inout vec3 diffuse, inout vec3 specular);
void PointLightContrib(PointLight light, vec3 normal, inout vec3 diffuse, inout vec3 specular);
void SpotLightContrib(SpotLight light, vec3 normal, inout vec3 diffuse, inout vec3 specular);


void main() {
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 ambientLight = dirLight.ambient * material.ambient;
	vec3 diffuseLight;
	vec3 specularLight;
	// Calculate light color on fragment
	DirLightContrib(dirLight, norm, diffuseLight, specularLight);
	for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
		PointLightContrib(pointLights[i], norm, diffuseLight, specularLight);
	}
	for (int i = 0; i < NR_SPOT_LIGHTS; ++i) {
		SpotLightContrib(spotLights[i], norm, diffuseLight, specularLight);
	}

	vec3 ambientColor = material.ambient * (material.useDiffMap ? vec3(texture(material.diffuseMap, TexCoords)) : vec3(1.0));
	vec3 diffuseColor = material.diffuse * (material.useDiffMap ? vec3(texture(material.diffuseMap, TexCoords)) : vec3(1.0));
	vec3 specularColor = material.specular * (material.useSpecMap ? vec3(texture(material.specularMap, TexCoords)) : vec3(1.0));

	vec3 ambient = ambientLight * ambientColor;
	vec3 diffuse = diffuseLight * diffuseColor;
	vec3 specular = specularLight * specularColor * material.specularStrength;

	vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result, material.opacity);
}


void DirLightContrib(DirLight light, vec3 normal, inout vec3 diffuse, inout vec3 specular) {
	vec3 lightDir = normalize(-light.direction);

	float lambertian = max(dot(normal, lightDir), 0.0);
	float spec = 0.0;

	if (lambertian > 0.0) {
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 halfDir = normalize(lightDir + viewDir);
		float specAngle = max(dot(normal, halfDir), 0.0);
		spec = pow(specAngle, material.shininess);
	}

	diffuse += light.diffuse * lambertian;
	specular += light.specular * spec;
}

void PointLightContrib(PointLight light, vec3 normal, inout vec3 diffuse, inout vec3 specular) {
	vec3 displacement = light.position - FragPos;
	float attenuation = 1.0 / length(displacement);
	vec3 lightDir = normalize(displacement);

	float lambertian = max(dot(normal, lightDir), 0.0);
	float spec = 0.0;

	if (lambertian > 0.0) {
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 halfDir = normalize(lightDir + viewDir);
		float specAngle = max(dot(normal, halfDir), 0.0);
		spec = pow(specAngle, material.shininess);
	}

	diffuse += light.diffuse * lambertian * attenuation;
	specular += light.specular * spec * attenuation;
}

void SpotLightContrib(SpotLight light, vec3 normal, inout vec3 diffuse, inout vec3 specular) {
	vec3 displacement = light.position - FragPos;
	float attenuation = 1.0 / length(displacement);
	vec3 lightDir = normalize(displacement);

	float theta = dot(lightDir, normalize(-light.direction)); 
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	float lambertian = max(dot(normal, lightDir), 0.0);
	float spec = 0.0;

	if (lambertian > 0.0) {
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 halfDir = normalize(lightDir + viewDir);
		float specAngle = max(dot(normal, halfDir), 0.0);
		spec = pow(specAngle, material.shininess);
	}

	diffuse += light.diffuse * lambertian * attenuation * intensity;
	specular += light.specular * spec * attenuation * intensity;
}
