#include "TessellationApplication.h"
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

} // anonymous

TessellationApplication::TessellationApplication()
{
}

TessellationApplication::~TessellationApplication()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
}

bool TessellationApplication::initialize()
{
	fw::printSystemInfo();

	cameraController.setCamera(&camera);
	cameraController.setResetMode(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 3.14f, 0.0f), SDLK_r);
	
	std::string path = "Shaders/simple";
	if (!shader.createProgram({path + ".vert", path + ".frag"})) {
		return false;
	}
	std::cout << "Loaded shader " << path << " (" << shader.getProgram() << ")\n";
	
	fw::Model model;
	std::string modelFile = "../Assets/Models/cube.obj";
	if (!model.loadModel(modelFile)) {
		return false;
	}
	std::cout << "Loaded model " << modelFile << "\n";
	
	createBuffers(model);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	
	return true;
}

void TessellationApplication::update()
{
	fw::toggleRelativeMouseMode();

	cameraController.update();
	
	glm::mat4 viewMatrix = camera.updateViewMatrix();
	mvpMatrix = camera.getProjectionMatrix() * viewMatrix; // modelMatrix = I
}

void TessellationApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(shader.getProgram());

	glUniformMatrix4fv(mvpMatrixLocation, 1, 0, glm::value_ptr(mvpMatrix));
	
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TessellationApplication::gui()
{
	fw::displayFps();
}

void TessellationApplication::createBuffers(const fw::Model& model)
{
	std::vector<float> AOS;
	std::vector<unsigned int> indices;
	fw::loadBufferData(model, AOS, indices);

	numIndices = indices.size();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	std::size_t floatSize = sizeof(float);
	GLsizei stride = 8 * floatSize;

	GLsizeiptr bufferSize = floatSize * AOS.size();
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferStorage(GL_ARRAY_BUFFER, bufferSize, AOS.data(), 0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * floatSize));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(5 * floatSize));
	glEnableVertexAttribArray(2);

	GLsizeiptr indexSize = sizeof(unsigned int) * indices.size();
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, indexSize, indices.data(), 0);
}
