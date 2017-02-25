#include "ExampleApplication.h"
#include <Framework/Framework.h>
#include <Framework/ImageLoader.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <cmath>

namespace
{

GLint mvpMatrixLocation = 0;
GLint timeLocation = 1;
GLint textureLocation = 2;

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
	fw::ImageLoader imageLoader;
	std::string textureFile = "../Assets/Textures/checker.png";
	int width = 0;
	int height = 0;
	int channels = 0;
	unsigned char* data = imageLoader.loadImage(textureFile, width, height, channels);
	if (data) {
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
	fw::Transformation& t = camera.getTransformation();
	glm::vec3& p = t.position;
	float timeSinceStart = static_cast<float>(fw::Framework::getTimeSinceStart()) / 1000.0f;
	p.z = 1.0f + std::sin(timeSinceStart);

	glm::vec3& r = t.rotation;
	r.y = 1.0f * std::sin(0.5f + 1.2f * timeSinceStart);

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

	GLfloat time = static_cast<GLfloat>(fw::Framework::getTimeSinceStart()) / 1000.0f;
	glUniform1f(timeLocation, time);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
