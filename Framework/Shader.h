#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

namespace fw
{

class Shader
{
public:
	explicit Shader();
	~Shader();
	Shader(const Shader&) = delete;
	Shader(Shader&&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&&) = delete;

	bool attachShader(GLenum shaderType, const std::string& filename);
	bool linkProgram();
	

private:
	GLuint program;
	std::vector<GLuint> shaders;

	bool compileShader(GLuint shader, const std::string& filename);
	void deleteShaders();
};

} // fs