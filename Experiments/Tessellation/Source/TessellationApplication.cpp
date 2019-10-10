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
const GLint tessLevelInnerLocation = 1;
const GLint tessLevelOuterLocation = 2;

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
	cameraController.setResetMode(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 3.14f, 0.0f), SDLK_r);

	std::string path = "Shaders/simple";
	if (!shader.createProgram({path + ".vert", path + ".tcs", path + ".tes", path + ".frag"})) {
		return false;
	}
	std::cout << "Loaded shader " << path << " (" << shader.getProgram() << ")\n";

	createBuffers();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void TessellationApplication::update()
{
	fw::toggleRelativeMouseMode();

	cameraController.update();

	if (fw::Input::isKeyReleased(SDLK_UP)) {
		++tessLevelInner;
	}
	if (fw::Input::isKeyReleased(SDLK_DOWN)) {
		tessLevelInner = std::max(1.0f, --tessLevelInner);
	}
	if (fw::Input::isKeyReleased(SDLK_RIGHT)) {
		++tessLevelOuter;
	}
	if (fw::Input::isKeyReleased(SDLK_LEFT)) {
		tessLevelOuter = std::max(1.0f, --tessLevelOuter);
	}

	glm::mat4 viewMatrix = camera.updateViewMatrix();
	mvpMatrix = camera.getProjectionMatrix() * viewMatrix; // modelMatrix = I
}

void TessellationApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(shader.getProgram());

	glUniformMatrix4fv(mvpMatrixLocation, 1, 0, glm::value_ptr(mvpMatrix));
	glUniform1f(tessLevelInnerLocation, tessLevelInner);
	glUniform1f(tessLevelOuterLocation, tessLevelOuter);

	glBindVertexArray(VAO);
	glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TessellationApplication::gui()
{
	fw::displayFps();
	ImGui::Text("Inner %.0f", tessLevelInner);
	ImGui::Text("Outer %.0f", tessLevelOuter);
}

void TessellationApplication::createBuffers()
{
	std::vector<float> positions{
		-1.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  0.0f,
		 0.0f,  0.73f, 0.0f
	};
	std::vector<unsigned int> indices{0, 1, 2};
	numIndices = indices.size();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLsizeiptr bufferSize = sizeof(float) * positions.size();
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferStorage(GL_ARRAY_BUFFER, bufferSize, positions.data(), 0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	GLsizeiptr indexSize = sizeof(unsigned int) * indices.size();
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, indexSize, indices.data(), 0);
}
