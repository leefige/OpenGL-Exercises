/*
 * OpenGL version 3.3 project.
 * 
 * LearnOpenGL 1: create a window and change the background color as time passes.
 */
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "clock.hpp"

using namespace cg;

// window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const float COLOR_CHANGE_SPEED = 0.1f;

// callbacks
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
	// init GLFW, set GL version & pipeline info
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create a window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL 1: window", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Error creating window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// use newly created window as context
	glfwMakeContextCurrent(window);

	// register callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	// connect GLAD to GLFW by registerring glfwGetProcAddress() as GLAD loader function
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cerr << "Error registerring gladLoadGLLoader" << std::endl;
		glfwTerminate();
		return -2;
	}

	GLfloat red = 0.2f;
	GLfloat green = 0.3f;
	GLfloat blue = 0.3f;
	double tmp;
	Clock my_clock;
	my_clock.reset();

	// update loop
	while (glfwWindowShouldClose(window) == 0) {
		// check event queue
		glfwPollEvents();

		// your update code here
		my_clock.lap();
		double time_delta = my_clock.elapsedSecond();

		red = modf(red + COLOR_CHANGE_SPEED * time_delta, &tmp);
		green = modf(green + COLOR_CHANGE_SPEED * time_delta * 0.85, &tmp);
		blue = modf(blue + COLOR_CHANGE_SPEED * time_delta * 0.75, &tmp);
	
		// drawing
		glClearColor(red, green, blue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// swap buffer
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// exit when pressing ESC
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
