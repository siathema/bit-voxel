#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 TexCoord;
in mat3 TBN;
in vec3 FragPos;
out vec4 color;


uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform mat4 model;
uniform mat4 view;


void main() {
// Phong lighting
  // Ambient light calculation
  vec3 ambient = (light.ambient * vec3(texture( diffuseMap, TexCoord))) * 0.5;

  vec3 normmap = texture(normalMap, TexCoord).rgb;
  normmap = normalize(normmap * 2.0 - 1.0);
  vec3 Normal = normalize(TBN * normmap);

  // Diffuse light calculation
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * texture(diffuseMap, TexCoord).rgb;

  // Specular light calculation
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * spec * texture(specularMap, TexCoord).rbg;

  vec4 lightMix = vec4((ambient + diffuse + specular), 1.0f);
  color = lightMix;//mix(lightMix ,texture(ourTexture1, TexCoord), 0.7);
}
