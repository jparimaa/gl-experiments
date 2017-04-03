#include "GeometryShaderApplication.h"
#include <Framework/Framework.h>
#include <Framework/Input.h>
#include <Framework/Common.h>
#include <Framework/imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

GeometryShaderApplication::GeometryShaderApplication()
{
}

GeometryShaderApplication::~GeometryShaderApplication()
{
}

bool GeometryShaderApplication::initialize()
{
	cameraController.setCamera(&camera);
	
	std::string path = "Shaders/explosion";
	if (!shader.createProgram({path + ".vert"/*, path + ".geom"*/, path + ".frag"})) {
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
	
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void GeometryShaderApplication::update()
{
	if (fw::Input::isKeyDown(SDLK_r)) {
		fw::Transformation& t = camera.getTransformation();
		t.position = glm::vec3(0.0f, 0.0f, 3.0f);
		t.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	cameraController.update();

	glm::mat4 viewMatrix = camera.updateViewMatrix();
	mvpMatrix = camera.getProjectionMatrix() * viewMatrix;
}

void GeometryShaderApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader.getProgram());
}

void GeometryShaderApplication::gui()
{
	fw::displayFps();
}
