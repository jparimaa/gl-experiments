#include "ExampleApplication.h"
#include <Framework/Framework.h>
#include <Framework/Image.h>
#include <Framework/Model.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <cmath>

namespace
{

const GLint mvpMatrixLocation = 0;
const GLint timeLocation = 1;
const GLint textureLocation = 2;

} // anonymous

ExampleApplication::ExampleApplication()
{
}

ExampleApplication::~ExampleApplication()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &texture);
}

bool ExampleApplication::initialize()
{
	cameraController.setCamera(&camera);

	fw::Model model;
	std::string modelFile = "../Assets/Models/attack_droid.obj";
	if (model.loadModel(modelFile)) {
		std::cout << "Loaded model " << modelFile << "\n";
	}

	// Shader
	std::string shaderPath = "Shaders/simple";
	std::vector<std::string> shaderFiles = {
		shaderPath + ".vert",
		shaderPath + ".frag"
	};
	if (shader.createProgram(shaderFiles)) {
		std::cout << "Loaded shader " << shaderPath << " (" << shader.getProgram() << ")\n";
	} else {
		std::cerr << "ERROR: Shader creation failed\n";
		return false;
	}
	
	// Texture
	fw::Image image;
	std::string textureFile = "../Assets/Textures/checker.png";
	int width = 0;
	int height = 0;
	int channels = 0;
	if (image.load(textureFile, width, height, channels)) {
		unsigned char* data = image.getData();
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		std::cout << "Loaded texture " << textureFile << " (" << texture << ")\n";
	} else {
		return false;
	}
		
	// Buffers
	GLfloat vertices[] = {
		 0.5f,  0.5f, -1.0f,   1.0f, 1.0f,
		 0.5f, -0.5f, -1.0f,   1.0f, 0.0f,
		-0.5f, -0.5f, -1.0f,   0.0f, 0.0f,
		-0.5f,  0.5f, -1.0f,   0.0f, 1.0f
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
	cameraController.update();

	glm::mat4 viewMatrix = camera.updateViewMatrix();
	mvpMatrix = camera.getProjectionMatrix() * viewMatrix /* * modelMatrix */;
}

void ExampleApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shader.getProgram());

	glUniformMatrix4fv(mvpMatrixLocation, 1, 0, glm::value_ptr(mvpMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureLocation, 0);

	glUniform1f(timeLocation, fw::Framework::getTimeSinceStart());

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
