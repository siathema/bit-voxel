#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coord;


out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  TexCoord = tex_coord;
  vec4 npos = vec4(position, 0.0, 1.0);
  gl_Position = npos;
}
