#version 330 core
in vec2 TexCoord;

out vec4 color;

uniform vec4 spriteColor;
uniform sampler2D ourTexture;

void main()
{
	color =  (spriteColor * texture(ourTexture, TexCoord));
}