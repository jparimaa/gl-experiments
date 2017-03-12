#include "InstancingApplication.h"
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
const GLint transformationMatricesLocation = 1;
const GLintptr numPrimitivesMatrixSize = numPrimitives * sizeof(glm::mat4);
const GLsizeiptr transformationMatricesBufferSize = 2 * numPrimitivesMatrixSize;

} // anonymous

InstancingApplication::InstancingApplication()
{
}

InstancingApplication::~InstancingApplication()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &transformationMatricesBuffer);
}

bool InstancingApplication::initialize()
{
	cameraController.setCamera(&camera);
	cameraController.setResetMode(glm::vec3(4.5f, 4.5f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), SDLK_r);
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

	createVertexBuffer(model);
	createUniformBuffer();

	float randMax = static_cast<float>(RAND_MAX);
	for (unsigned int i = 0; i < numPrimitives; ++i) {		
		float x = static_cast<float>(i % 10);
		float y = static_cast<float>(i / 10);
		fw::Transformation& t = transformations[i];
		t.position = glm::vec3(x, y, 0.0f);
		t.rotation = glm::vec3(0.0f, 3.141f, 0.0f);
		t.scale = glm::vec3(0.2f, 0.2f, 0.2f);

		float rx = static_cast<float>(rand()) / randMax;
		float ry = static_cast<float>(rand()) / randMax;
		float rz = static_cast<float>(rand()) / randMax;
		rotationAxes[i] = glm::normalize(glm::vec3(rx, ry, rz));
	}
	
	return true;
}

void InstancingApplication::update()
{
	fw::toggleRelativeMouseMode();

	if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
		cameraController.update();
	}

	glm::mat4 viewMatrix = camera.updateViewMatrix();

	for (unsigned int i = 0; i < numPrimitives; ++i) {
		transformations[i].rotate(rotationAxes[i], 0.5f * fw::Framework::getFrameTime());
		modelMatrices[i] = transformations[i].updateModelMatrix();
		mvpMatrices[i] = camera.getProjectionMatrix() * viewMatrix * modelMatrices[i];
	}
}

void InstancingApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader.getProgram());
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image.getTexture());
	glUniform1i(textureLocation, 0);

	GLintptr matrixSize = numPrimitives * sizeof(glm::mat4);
	glBindBuffer(GL_UNIFORM_BUFFER, transformationMatricesBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, numPrimitivesMatrixSize, modelMatrices.data());
	glBufferSubData(GL_UNIFORM_BUFFER, numPrimitivesMatrixSize, numPrimitivesMatrixSize, mvpMatrices.data());

	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0, numPrimitives);
}

void InstancingApplication::gui()
{
	fw::displayFps();
	fw::displayPosition("Position %.1f %.1f %.1f", camera.getTransformation().position);
}

void InstancingApplication::createVertexBuffer(const fw::Model& model)
{
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
}

void InstancingApplication::createUniformBuffer()
{
	glGenBuffers(1, &transformationMatricesBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, transformationMatricesBuffer);
	glBufferData(GL_UNIFORM_BUFFER, transformationMatricesBufferSize, 0, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, transformationMatricesBuffer);
}
