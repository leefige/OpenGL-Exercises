# THU Computer Graphics OpenGL Tutorial Exercises

Exercises of OpenGL Tutorials given in Computer Graphics course.

## Build environment

- Windows 10 64-bit
- Visual Studio 2019 (x86_64)
- OpenGL version 3.3 (Core Profile)
- [GLFW](https://www.glfw.org/download.html)
- [GLAD](https://glad.dav1d.de/)
- [GLM](https://github.com/g-truc/glm)

Please set `GLFW_HOME` and `GLAD_HOME` environment variables to their root dirs. Set the VC++ include path, library path, and the link additional dependencies (`glfw3.lib`).

You also need to put `glad.c` into your project dir as this repo does.

## TOC

- gl_1: Creating a window with GLFW; background color changes with time
- gl_2: Draw a Hexagram with GLSL
- gl_3_1: Draw a triangle with GLSL whose color changes with time (sin function)
- gl_3_2: Draw a triangle with GLSL using color specified from vertex attributes
- gl_3_3: Brand new `Shader` class for creating Shader Program from files
- gl_4: Model, view and perspective matrices from GLM
- gl_5: Camera system, keyboard & mouse control
- gl_6_1: Text rendering
- gl_7: Particle system (firework)

(TBC)
