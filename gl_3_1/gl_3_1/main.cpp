/*
 * OpenGL version 3.3 project.
 */
#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader_loader.hpp"

using namespace cg;

// window settings
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

// normalized coordinates
constexpr GLfloat vertices[] = {
	// Positions        
	0.5f, -0.5f, 0.0f,  // Bottom Right
	-0.5f, -0.5f, 0.0f,  // Bottom Left
	0.0f, 0.5f, 0.0f   // Top 
};

// callbacks
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// install Shaders from files
const GLuint installShaderProgram(const std::string& vertexFilename, const std::string& fragmentFilename);

int main()
{
	// Setup a GLFW window

	// init GLFW, set GL version & pipeline info
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create a window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tutorial 3.1: color-changing triangle", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Error creating window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// use newly created window as context
	glfwMakeContextCurrent(window);

	// register callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	// ===========================================================================

	// Connect GLAD to GLFW by registerring glfwGetProcAddress() as GLAD loader function,
	// this must be done after setting current context

	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cerr << "Error registerring gladLoadGLLoader" << std::endl;
		glfwTerminate();
		return -2;
	}

	// ===========================================================================

	// Install GLSL Shader programs

	const GLuint shaderProgram = installShaderProgram("VertexShader.vert", "FragmentShader.frag");
	if (shaderProgram == 0) {
		std::cerr << "Error installing Shader Program" << std::endl;
		glfwTerminate();
		return -3;
	}

	// ===========================================================================

	// Set up vertex data (and buffer(s)) and attribute pointers

	// bind VAO
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// bind VBO, buffer data to it
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// unbind VBO & VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ===========================================================================

	// Update loop

	while (glfwWindowShouldClose(window) == 0) {
		// check event queue
		glfwPollEvents();

		/* your update code here */
	
		// draw background
		GLfloat red = 0.2f;
		GLfloat green = 0.3f;
		GLfloat blue = 0.3f;
		glClearColor(red, green, blue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		// Update the uniform color
		GLfloat timeValue = GLfloat(glfwGetTime());
		GLfloat scalor = sin(timeValue) / 2 + 0.5f;
		GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUniform4f(vertexColorLocation, 1.0f * scalor, 0.5f * scalor, 0.2f * scalor, 1.0f * scalor);

		// draw a triangle
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// swap buffer
		glfwSwapBuffers(window);
	}

	// properly de-allocate all resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// exit when pressing ESC
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// resize window
	glViewport(0, 0, width, height);
}

const GLuint installShaderProgram(const std::string& vertexFilename, const std::string& fragmentFilename)
{
	// Build and compile our shader programs

	// Vertex shader
	const GLuint vertexShader = ShaderLoader::createShaderFromFile(vertexFilename, GL_VERTEX_SHADER);
	if (vertexShader == 0) {
		std::cerr << "Cannot create Vertex Shader from file '" << vertexFilename << "'." << std::endl;
		return 0;
	}

	// Fragment shader
	const GLuint fragmentShader = ShaderLoader::createShaderFromFile(fragmentFilename, GL_FRAGMENT_SHADER);
	if (fragmentShader == 0) {
		std::cerr << "Cannot create Fragment Shader from file '" << fragmentFilename << "'." << std::endl;
		glDeleteShader(vertexShader);
		return 0;
	}

	// link shaders: including vertex & fragment shaders
	const GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// check for linking errors
	GLint success;
	const GLsizei logLen = 512;
	GLchar infoLog[logLen];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, logLen, NULL, infoLog);
		std::cerr << "Link Shader error: " << infoLog << std::endl;
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(shaderProgram);
		return 0;
	}

	// release input shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}