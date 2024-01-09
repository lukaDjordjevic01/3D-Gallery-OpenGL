#version 330 core

out vec4 outCol;

uniform float circlePulse;

void main()
{
	outCol = vec4(
        0.5 + 0.5 * sin(circlePulse),
        0.5 + 0.5 * sin(circlePulse + 2.0),
        0.5 + 0.5 * sin(circlePulse + 4.0),
        1.0
    );
}