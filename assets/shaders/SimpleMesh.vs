#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 mvp;

void main() {
  gl_Position = mvp * vec4(position, 1.0f);
  
  TexCoord = texCoord;
}
