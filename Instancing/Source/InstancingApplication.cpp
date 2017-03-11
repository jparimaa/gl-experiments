#include "InstancingApplication.h"
#include <Framework/Framework.h>
#include <Framework/Input.h>
#include <Framework/Common.h>
#include <Framework/Model.h>
#include <Framework/imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL.h>
#include <iostream>

namespace
{

const GLint modelMatrixLocation = 0;
const GLint mvpMatrixLocation = 1;
const GLint textureLocation = 2;
const GLsizei numPrimitives = 10;

} // anonymous

InstancingApplication::InstancingApplication()
{
}

InstancingApplication::~InstancingApplication()
{
}

bool InstancingApplication::initialize()
{
	cameraController.setCamera(&camera);
	cameraController.setResetMode(glm::vec3(0.0f, 0.7f, 2.5f), glm::vec3(-0.4f, 0.0f, 0.0f), SDLK_r);
	objTransformation.rotation = glm::vec3(0.0f, 3.141f, 0.0f);
	objTransformation.scale = glm::vec3(0.2f, 0.2f, 0.2f);
	objTransformation.updateModelMatrix();
	SDL_SetRelativeMouseMode(SDL_TRUE);
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);	

	std::string shaderPath = "Shaders/instancing";
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

	std::vector<float> AOS;
	std::vector<unsigned int> indices;
	unsigned int indexOffset = 0;
	
	for (const auto& mesh : model.getMeshes()) {
		for (unsigned int i = 0; i < mesh.vertices.size(); ++i) {
			AOS.push_back(mesh.vertices[i].x);
			AOS.push_back(mesh.vertices[i].y);
			AOS.push_back(mesh.vertices[i].z);
			AOS.push_back(mesh.uvs[i].x);
			AOS.push_back(mesh.uvs[i].y);
			AOS.push_back(mesh.normals[i].x);
			AOS.push_back(mesh.normals[i].y);
			AOS.push_back(mesh.normals[i].z);
		}
		for (const auto& index : mesh.indices) {
			indices.push_back(index + indexOffset);
		}
		indexOffset += mesh.vertices.size();
	}
	numIndices = indices.size();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	std::size_t floatSize = sizeof(float);
	GLsizei stride = 8 * floatSize;

	GLsizeiptr bufferSize = floatSize * AOS.size();
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, AOS.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * floatSize));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(5 * floatSize));
	glEnableVertexAttribArray(2);

	GLsizeiptr indexSize = sizeof(unsigned int) * indices.size();
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indices.data(), GL_STATIC_DRAW);
	
	return true;
}

void InstancingApplication::update()
{
	fw::toggleRelativeMouseMode();

	if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
		cameraController.update();
	}

	glm::mat4 viewMatrix = camera.updateViewMatrix();
	mvpMatrix = camera.getProjectionMatrix() * viewMatrix * objTransformation.getModelMatrix();
}

void InstancingApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader.getProgram());

	glUniformMatrix4fv(modelMatrixLocation, 1, 0, glm::value_ptr(objTransformation.getModelMatrix()));
	glUniformMatrix4fv(mvpMatrixLocation, 1, 0, glm::value_ptr(mvpMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image.getTexture());
	glUniform1i(textureLocation, 0);

	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0, numPrimitives);
}

void InstancingApplication::gui()
{
	fw::displayFps();
	fw::displayPosition("Position %.1f %.1f %.1f", camera.getTransformation().position);
}