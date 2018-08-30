#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

out vec3 FragPos;
out mat3 TBN;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform sampler2D normalMap;


void main() {
  gl_Position = projection * view * model * vec4(position, 1.0f);
  FragPos = vec3(model * vec4(position, 1.0f));
  vec3 T = normalize(vec3(model * vec4(tangent, 0.0f)));
  vec3 N = normalize(vec3(model * vec4(normal, 0.0f)));
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);
  TBN = mat3(T, B, N);
  //Normal = mat3(transpose(inverse(model))) * normal;
  TexCoord = texCoord;
}
