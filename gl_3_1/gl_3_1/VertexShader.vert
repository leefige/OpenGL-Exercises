/*
 * GLSL Vertex Shader code for OpenGL version 3.3
 */

#version 330 core

// input vertex attributes
layout (location = 0) in vec3 position;

void main()
{
	gl_Position = vec4(position, 1.0);
}
