#version 330 core 

layout(location = 0) in vec2 inPos;
uniform vec2 circlePos;

void main()
{
	gl_Position = vec4(inPos.x + circlePos.x, inPos.y + circlePos.y, 0.0, 1.0);
}