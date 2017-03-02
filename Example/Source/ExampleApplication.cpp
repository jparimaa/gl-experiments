#include "ExampleApplication.h"
#include <Framework/Framework.h>
#include <Framework/Image.h>
#include <Framework/Model.h>
#include <Framework/Input.h>
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
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteTextures(1, &texture);
}

bool ExampleApplication::initialize()
{
	cameraController.setCamera(&camera);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

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

	// Model
	fw::Model model;
	std::string modelFile = "../Assets/Models/monkey.3ds";
	if (model.loadModel(modelFile)) {
		std::cout << "Loaded model " << modelFile << "\n";
	}
	if (model.getMeshes().empty()) {
		std::cerr << "ERROR: Empty model\n";
		return false;
	}

	// Buffers
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	const fw::Model::Mesh mesh = model.getMeshes()[0];

	GLsizeiptr vertexSize = sizeof(glm::vec3) * mesh.vertices.size();
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexSize, &mesh.vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	GLsizeiptr uvSize = sizeof(glm::vec2) * mesh.uvs.size();
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvSize, &mesh.uvs[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	numIndices = mesh.indices.size();
	GLsizeiptr indexSize = sizeof(unsigned int) * numIndices;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, &mesh.indices[0], GL_STATIC_DRAW);

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

	return true;
}

void ExampleApplication::update()
{
	if (fw::Input::isKeyDown(SDLK_r)) {
		fw::Transformation& t = camera.getTransformation();
		t.position = glm::vec3(0.0f, 0.0f, 3.0f);
		t.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	cameraController.update();

	objTransformation.rotate(glm::vec3(0.0f, 1.0f, 0.0f), fw::Framework::getFrameTime() * 0.7f);
	objTransformation.updateModelMatrix();
	
	glm::mat4 viewMatrix = camera.updateViewMatrix();
	mvpMatrix = camera.getProjectionMatrix() * viewMatrix * objTransformation.getModelMatrix();
}

void ExampleApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader.getProgram());

	glUniformMatrix4fv(mvpMatrixLocation, 1, 0, glm::value_ptr(mvpMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureLocation, 0);

	glUniform1f(timeLocation, fw::Framework::getTimeSinceStart());

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}
