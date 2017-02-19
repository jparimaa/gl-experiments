#include "ExampleApplication.h"
#include <Shader.h>
#include <GL/glew.h>
#include <iostream>

ExampleApplication::ExampleApplication()
{

}

ExampleApplication::~ExampleApplication()
{
}

bool ExampleApplication::initialize()
{
	fw::Shader s;
	if (s.attachShader(GL_VERTEX_SHADER, "../Assets/Shaders/passthrough.vert") &&
		s.attachShader(GL_FRAGMENT_SHADER, "../Assets/Shaders/passthrough.frag")) {
		if (s.linkProgram()) {
			std::cout << "Shader program linked\n";
		}
	}	
	return true;
}

void ExampleApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
