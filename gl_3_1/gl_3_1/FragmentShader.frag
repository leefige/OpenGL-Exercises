/*
 * GLSL Fragment Shader code for OpenGL version 3.3
 */

#version 330 core

// output color
out vec4 color;
uniform vec4 ourColor;

void main()
{
	color = ourColor;
}
