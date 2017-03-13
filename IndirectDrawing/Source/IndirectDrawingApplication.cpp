#include "IndirectDrawingApplication.h"
#include <Framework/Framework.h>
#include <Framework/Input.h>
#include <Framework/Common.h>
#include <Framework/imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL.h>
#include <iostream>
#include <cstdlib>

namespace
{

const GLint textureLocation = 0;

} // anonymous

IndirectDrawingApplication::IndirectDrawingApplication()
{
}

IndirectDrawingApplication::~IndirectDrawingApplication()
{
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &vertexBuffer);
}

bool IndirectDrawingApplication::initialize()
{
	cameraController.setCamera(&camera);
	cameraController.setResetMode(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), SDLK_r);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	std::string shaderPath = "Shaders/indirect";
	std::vector<std::string> shaderFiles = {
		shaderPath + ".vert",
		shaderPath + ".frag"
	};
	if (!shader.createProgram(shaderFiles)) {
		return false;
	}
	std::cout << "Loaded shader " << shaderPath << " (" << shader.getProgram() << ")\n";

	std::string textureFile = "../Assets/Textures/green_square.png";
	if (!image.load(textureFile)) {
		return false;
	}
	image.create2dTexture();
	std::cout << "Loaded texture " << textureFile << " (" << image.getTexture() << ")\n";

	fw::Model model;
	std::string modelFile = "../Assets/Models/monkey.3ds";
	if (!model.loadModel(modelFile)) {
		return false;
	}
	std::cout << "Loaded model " << modelFile << "\n";

	return true;
}

void IndirectDrawingApplication::update()
{
	fw::toggleRelativeMouseMode();

	if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
		cameraController.update();
	}

	glm::mat4 viewMatrix = camera.updateViewMatrix();
}

void IndirectDrawingApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void IndirectDrawingApplication::gui()
{
	fw::displayFps();
	fw::displayPosition("Position %.1f %.1f %.1f", camera.getTransformation().position);
}

