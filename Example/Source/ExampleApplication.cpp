#include "ExampleApplication.h"
#include <Framework/Framework.h>
#include <Framework/Input.h>
#include <Framework/Common.h>
#include <Framework/imgui/imgui.h>
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
}

bool ExampleApplication::initialize()
{
	fw::printSystemInfo();

	cameraController.setCamera(&camera);
	
	std::string path = "Shaders/simple";
	if (!shader.createProgram({path + ".vert", path + ".frag"})) {
		return false;
	}
	std::cout << "Loaded shader " << path << " (" << shader.getProgram() << ")\n";
	
	fw::Model model;
	std::string modelFile = "../Assets/Models/monkey.3ds";
	if (!model.loadModel(modelFile)) {
		return false;
	}
	std::cout << "Loaded model " << modelFile << "\n";

	std::string textureFile = "../Assets/Textures/checker.png";
	if (!image.load(textureFile)) {
		return false;
	}
	image.create2dTexture();
	std::cout << "Loaded texture " << textureFile << " (" << image.getTexture() << ")\n";

	createBuffers(model);

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

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
	glBindTexture(GL_TEXTURE_2D, image.getTexture());
	glUniform1i(textureLocation, 0);

	glUniform1f(timeLocation, fw::Framework::getTimeSinceStart());

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

void ExampleApplication::gui()
{
	fw::displayFps();
}

void ExampleApplication::createBuffers(const fw::Model& model)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	const fw::Model::Mesh mesh = model.getMeshes()[0];

	GLsizeiptr vertexSize = sizeof(glm::vec3) * mesh.vertices.size();
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferStorage(GL_ARRAY_BUFFER, vertexSize, &mesh.vertices[0], 0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	GLsizeiptr uvSize = sizeof(glm::vec2) * mesh.uvs.size();
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferStorage(GL_ARRAY_BUFFER, uvSize, &mesh.uvs[0], 0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	numIndices = mesh.indices.size();
	GLsizeiptr indexSize = sizeof(unsigned int) * numIndices;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, indexSize, &mesh.indices[0], 0);
}
