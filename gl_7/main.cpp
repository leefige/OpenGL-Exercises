/*
 * OpenGL version 3.3 project.
 */
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL2/SOIL2.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "shader.hpp"
#include "particalsys.hpp"

using namespace cg;

// window settings
int screenWidth = 800;
int screenHeight = 600;

FireWork** fireWorks;
const int fireWorkNum = 3;

GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

// normalized coordinates
constexpr GLfloat vertices[] = {
	// Positions        // Colors
	0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // Bottom Right
	-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // Bottom Left
	0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f   // Top 
};

// callbacks
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main()
{
	// Setup a GLFW window

	// init GLFW, set GL version & pipeline info
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create a window
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "My OpenGL project", nullptr, nullptr);
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

	// ---------------------------------------------------------------

	// Connect GLAD to GLFW by registerring glfwGetProcAddress() as GLAD loader function,
	// this must be done after setting current context

	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cerr << "Error registerring gladLoadGLLoader" << std::endl;
		glfwTerminate();
		return -2;
	}

	// ---------------------------------------------------------------

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Install GLSL Shader programs
	auto shaderProgram = Shader::Create("VertexShader.vert", "FragmentShader.frag");
	if (shaderProgram == nullptr) {
		std::cerr << "Error creating Shader Program" << std::endl;
		glfwTerminate();
		return -3;
	}

    // ---------------------------------------------------------------

    fireWorks = new FireWork * [fireWorkNum];
    for (int i = 0; i < fireWorkNum; ++i)     {
        fireWorks[i] = new FireWork(500, 0.5, glm::vec3(rand() % (screenWidth / 3) + screenWidth / 3 * i, screenHeight / 4, 0),
                                    glm::vec3(0.0, 70, 0.0), glm::vec3(0.0, -9.8, 0.0), (double)rand() / (double)RAND_MAX * 2 + 12);
        // massNum: 500, each firework consists of 500 particle
        // mass: 5, each particle's mass is 5
        // position: start position
        // speed: initial speed is (0, 70, 0)
        // gravity: gravity is (0, -9.8, 0)
        // lifeValue: existing time in seconds
    }

    GLfloat particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };


	// ---------------------------------------------------------------

	// Set up vertex data (and buffer(s)) and attribute pointers

	// bind VAO
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// bind VBO, buffer data to it
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

	// set vertex attribute pointers
	// position attribute
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// unbind VBO & VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


    // ---------------------------------------------------------------

    // Load and create a texture
    GLuint texture;
    // ====================
    // Texture
    // ====================
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image("Particle.bmp", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.


	// ---------------------------------------------------------------

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Update loop

	while (glfwWindowShouldClose(window) == 0) {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		// check event queue
		glfwPollEvents();

		/* your update code here */
	
		// draw background
		GLfloat red = 0.1f;
		GLfloat green = 0.1f;
		GLfloat blue = 0.1f;
		glClearColor(red, green, blue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < fireWorkNum; ++i) {
            fireWorks[i]->Process(deltaTime * 3, glm::vec3(rand() % (screenWidth / 3) + screenWidth / 3 * i, screenHeight / 4, 0)); // apply gravity and update speed, position
        }

        // Draw
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glm::mat4 projection = glm::ortho(0.0f, GLfloat(screenWidth), 0.0f, GLfloat(screenHeight), -1.0f, 100.0f);
        shaderProgram->Use();
        GLint projLoc = glGetUniformLocation(shaderProgram->Program(), "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        for (int i = 0; i < fireWorkNum; ++i)         {
            if (fireWorks[i]->hasExploded) {
                for (int j = 0; j < fireWorks[i]->GetMassNum(); ++j) {
                    GLint offsetLoc = glGetUniformLocation(shaderProgram->Program(), "offset");
                    glUniform2f(offsetLoc, fireWorks[i]->GetMass(j)->position[0], fireWorks[i]->GetMass(j)->position[1]);
                    GLint colorLoc = glGetUniformLocation(shaderProgram->Program(), "color");
                    glUniform4f(colorLoc, fireWorks[i]->GetMass(j)->color.r, fireWorks[i]->GetMass(j)->color.g, fireWorks[i]->GetMass(j)->color.b,
                                fireWorks[i]->GetMass(j)->color.a);
                    glBindTexture(GL_TEXTURE_2D, texture);
                    glBindVertexArray(VAO);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                    glBindVertexArray(0);
                }
            } else {// before explosion, only one point
                int j = 0;
                GLint offsetLoc = glGetUniformLocation(shaderProgram->Program(), "offset");
                glUniform2f(offsetLoc, fireWorks[i]->GetMass(j)->position[0], fireWorks[i]->GetMass(j)->position[1]);
                GLint colorLoc = glGetUniformLocation(shaderProgram->Program(), "color");
                glUniform4f(colorLoc, fireWorks[i]->GetMass(j)->color.r, fireWorks[i]->GetMass(j)->color.g, fireWorks[i]->GetMass(j)->color.b,
                            fireWorks[i]->GetMass(j)->color.a);
                glBindTexture(GL_TEXTURE_2D, texture);
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
            }
        }

		// swap buffer
		glfwSwapBuffers(window);
	}

	// properly de-allocate all resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

/* ======================== helper functions ======================== */

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// exit when pressing ESC
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    screenWidth = width;
    screenHeight = height;
	// resize window
	glViewport(0, 0, width, height);
}
