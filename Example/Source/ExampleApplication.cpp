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
	// Shader
	std::string shaderPath = "Shaders/simple";
	std::vector<std::string> shaderFiles = {
		shaderPath + ".vert",
		shaderPath + ".frag"
	};
	if (shader.createProgram(shaderFiles)) {
		std::cout << "Created shader program " << shaderPath << " (" << shader.getProgram() << ")\n";
	} else {
		std::cerr << "ERROR: Shader creation failed\n";
		return false;
	}
	
	// Texture
	std::string textureFile = "../Assets/Textures/checker.png";
	if (texture.setTexImage2D(textureFile)) {
		std::cout << "Loaded texture " << textureFile << " (" << texture.getId() << ")\n";
	} else {
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, texture.getId());
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Uniforms	
	glUseProgram(shader.getProgram());
	timeLocation = glGetUniformLocation(shader.getProgram(), "time");
	if (timeLocation == -1) {
		std::cerr << "ERROR: Invalid uniform location: time\n";
	}
	textureLocation = glGetUniformLocation(shader.getProgram(), "tex0");
	if (timeLocation == -1) {
		std::cerr << "ERROR: Invalid uniform location: tex0\n";
	}

	// Buffers
	GLfloat vertices[] = {
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f
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

	GLsizei stride = 5 * sizeof(GLfloat);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	GLvoid* offset = (GLvoid*)(3 * sizeof(GLfloat));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(1);

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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.getId());
	glUniform1i(textureLocation, 0);

	GLfloat time = static_cast<GLfloat>(fw::Framework::getTimeSinceStart()) / 1000.0f;
	glUniform1f(timeLocation, time);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
