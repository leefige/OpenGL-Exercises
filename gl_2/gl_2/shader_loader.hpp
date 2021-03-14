#ifndef CG_SHADER_LOADER_H_
#define CG_SHADER_LOADER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#include <glad/glad.h>

namespace cg
{

class ShaderLoader
{
	const std::string source;

private:
	ShaderLoader() = delete;
	ShaderLoader& operator=(const std::string& sourceCode) = delete;
	ShaderLoader& operator=(std::string&& sourceCode) = delete;

	explicit ShaderLoader(const std::string& sourceCode) : source(sourceCode) {}
	explicit ShaderLoader(std::string&& sourceCode) : source(sourceCode) {}

	static std::unique_ptr<ShaderLoader> loadFile(const std::string& filename)
	{
		std::ifstream fin;
		fin.open(filename, std::ios::in);
		if (!fin.is_open()) {
			std::cerr << "ShaderLoader: cannot open file '" << filename << "'" << std::endl;
			return nullptr;
		}

		std::ostringstream tmp;
		tmp << fin.rdbuf();
		if (!tmp.good()) {
			std::cerr << "ShaderLoader: read file '" << filename << "' error" << std::endl;
			fin.close();
			return nullptr;
		}

		fin.close();
		return std::unique_ptr<ShaderLoader>(new ShaderLoader(tmp.str()));
	}

public:
	static const GLuint createShaderFromFile(const std::string& filename, GLenum type)
	{
		auto glsl = ShaderLoader::loadFile(filename);
		if (glsl == nullptr) {
			std::cerr << "ShaderLoader: cannot load Shader file '" << filename << "'." << std::endl;
			return 0;
		}
		const GLchar* const shaderSource = glsl->source.c_str();
		const GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &shaderSource, NULL);
		glCompileShader(shader);

		// Check compile errors
		GLint success;
		const GLsizei logLen = 512;
		GLchar infoLog[logLen];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, logLen, NULL, infoLog);
			std::cerr << "ShaderLoader: Shader file '" << filename << "' compile error: " << infoLog << std::endl;
			glDeleteShader(shader);
			return 0;
		}
		return shader;
	}
};

} /* namespace cg */

#endif /* CG_SHADER_LOADER_H_ */
