#include "Shader.h"
#include <iostream>
#include <iterator>
#include <fstream>

namespace fw
{

Shader::Shader()
{	
}

Shader::~Shader()
{
	glDeleteProgram(program);
	deleteShaders();
}

bool Shader::createProgram(const std::vector<std::string>& files)
{
	program = glCreateProgram();
	for (const auto file : files) {
		if (!attachShader(file)) {
			return false;
		}
	}
	
	return linkProgram();
}

GLuint Shader::getProgram() const
{
	return program;
}

bool Shader::attachShader(const std::string& filename)
{
	auto getShaderType = [] (const std::string& s) {
		if (s.find(".vert") != std::string::npos) {
			return GL_VERTEX_SHADER;
		}
		if (s.find(".geom") != std::string::npos) {
			return GL_GEOMETRY_SHADER;
		}
		if (s.find(".frag") != std::string::npos) {
			return GL_FRAGMENT_SHADER;
		}
		return 0;
	};

	GLuint shader = glCreateShader(getShaderType(filename));
	if (shader == 0) {
		std::cerr << "ERROR: Could not create shader from file " << filename << "\n";
		return false;
	}
	if (!compileShader(shader, filename)) {
		glDeleteShader(shader);
		return false;
	}

	shaders.push_back(shader);
	glAttachShader(program, shader);
	return true;
}

bool Shader::compileShader(GLuint shader, const std::string& filename)
{
	std::string shaderCode;
	std::ifstream file(filename.c_str());
	if (file) {
		shaderCode.append((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
	} else {
		std::cerr << "ERROR: Could not open file: " << filename << "\n";
		return false;
	}

	const char* shaderCodeCstr = shaderCode.c_str();
	glShaderSource(shader, 1, &shaderCodeCstr, NULL);

	glCompileShader(shader);
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		std::cerr << "ERROR: Shader compilation failed: " << filename << "\n";
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
		std::copy(infoLog.begin(), infoLog.end(), std::ostream_iterator<GLchar>(std::cerr, ""));
		std::string outFile = "failed_shader.out";
		std::ofstream ofs(outFile.c_str());
		if (ofs) {
			ofs << shaderCode;
			ofs.close();
			std::cerr << "Failed shader code written to file: " << outFile << "\n";
		}
		return false;
	}
	return true;
}

bool Shader::linkProgram()
{
	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		std::copy(infoLog.begin(), infoLog.end(), std::ostream_iterator<GLchar>(std::cerr, ""));
		glDeleteProgram(program);
	}

	deleteShaders();
	return isLinked == GL_TRUE;
}

void Shader::deleteShaders()
{
	for (GLuint shader : shaders) {
		glDetachShader(program, shader);
		glDeleteShader(shader);
	}
	shaders.clear();
}

} // fw
