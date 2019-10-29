#version 330

uniform vec4 color;

out vec4 outColor;

void main()
{
	outColor = color;
	outColor.a = 0.3;
}
