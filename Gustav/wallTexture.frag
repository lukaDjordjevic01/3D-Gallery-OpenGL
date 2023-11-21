#version 330 core

in vec2 chTex; //koordinate teksture
out vec4 outCol;

uniform sampler2D uTex; //teksturna jedinica

void main()
{
	//outCol = vec4(0.0, 1.0, 0.0, 1.0);
	outCol = texture(uTex, chTex);
}