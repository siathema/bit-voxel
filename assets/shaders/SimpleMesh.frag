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
in vec3 Normal;
in vec3 FragPos;

out vec4 color;


uniform sampler2D ourTexture1;
uniform Material material;
uniform Light light;
uniform vec3 viewPos;


void main() {
// Phong lighting
  // Ambient light calculation
  vec3 ambient = light.ambient * material.ambient;

  // Diffuse light calculation
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * (diff * material.diffuse);

  // Specular light calculation
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * material.specular);

  vec4 lightMix = vec4((ambient + diffuse + specular), 1.0f);
  vec3 normalColor = (Normal + 1) * 0.5f;
  color = vec4(normalColor, 1.0f);//mix(lightMix ,texture(ourTexture1, TexCoord), 0.7);
}
