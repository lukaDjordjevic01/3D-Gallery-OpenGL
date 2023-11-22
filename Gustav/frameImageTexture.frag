#version 330 core

in vec2 chTex; //koordinate teksture
out vec4 outCol;

uniform sampler2D imageTex; //teksturna jedinica

void main()
{
	outCol = texture(imageTex, chTex);
}