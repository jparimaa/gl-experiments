#include "ExampleApplication.h"
#include <Framework\Framework.h>
#include <iostream>
#include <fstream>

ExampleApplication::ExampleApplication()
{
}

ExampleApplication::~ExampleApplication()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

bool ExampleApplication::initialize()
{
	std::string shaderPath = "Shaders/simple";
	std::vector<std::string> shaderFiles = {
		shaderPath + ".vert",
		shaderPath + ".frag"
	};
	if (shader.createProgram(shaderFiles)) {
		std::cout << "Created shader program " << shaderPath << ", ID: " << shader.getProgram() << "\n";
	} else {
		std::cerr << "ERROR: Shader creation failed\n";
		return false;
	}

	timeLocation = glGetUniformLocation(shader.getProgram(), "time");

	GLfloat vertices[] = {
		0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	return true;
}

void ExampleApplication::update()
{
}

void ExampleApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shader.getProgram());
	GLfloat time = static_cast<GLfloat>(fw::Framework::getTimeSinceStart()) / 1000.0f;
	glUniform1f(timeLocation, time);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
