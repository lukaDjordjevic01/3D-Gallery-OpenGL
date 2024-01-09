#version 330 core 

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex;
out vec2 chTex;

uniform int flipHorizontal;
uniform int flipVertical; 

void main()
{
	vec2 flippedTex = vec2(flipHorizontal == 1 ? 1.0 - inTex.x : inTex.x, flipVertical == 1 ? 1.0 - inTex.y : inTex.y);

	gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0);
	chTex = flippedTex;
}