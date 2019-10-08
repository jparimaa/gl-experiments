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

	bool createProgram(const std::vector<std::string>& files);	
	GLuint getProgram() const;

private:
	GLuint program = 0;
	std::vector<GLuint> shaders;

	bool attachShader(const std::string& filename);	
	bool compileShader(GLuint shader, const std::string& filename);
	bool linkProgram();
	void deleteShaders();
};

} // fs
