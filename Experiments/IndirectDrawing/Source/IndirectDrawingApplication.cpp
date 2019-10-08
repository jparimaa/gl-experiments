#pragma warning(disable:4996)

#include "IndirectDrawingApplication.h"
#include <Framework/Framework.h>
#include <Framework/Input.h>
#include <Framework/Common.h>
#include <Framework/imgui/imgui.h>
#include <SDL.h>
#include <iostream>
#include <algorithm>
#include <cstdlib>

namespace
{

const GLuint modelBufferIndex = 0;
const GLuint mvpBufferIndex = 1;
const GLuint indexToMatrixBufferIndex = 2;

} // anonymous

IndirectDrawingApplication::IndirectDrawingApplication()
{
}

IndirectDrawingApplication::~IndirectDrawingApplication()
{
	glDeleteBuffers(1, &commandBuffer);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &modelBuffer);
	glDeleteBuffers(1, &mvpBuffer);
	glDeleteBuffers(1, &indexToMatrixBuffer);
}

bool IndirectDrawingApplication::initialize()
{
	cameraController.setCamera(&camera);
	cameraController.setResetMode(glm::vec3(2.0f, 1.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), SDLK_r);
	SDL_SetRelativeMouseMode(SDL_TRUE);

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

	std::vector<fw::Model> models;
	std::vector<std::string> modelFiles = {
		"../Assets/Models/Untracked/attack_droid.obj",
		"../Assets/Models/monkey.3ds"
	};
	for (const auto& modelFile : modelFiles) {
		models.push_back(fw::Model());
		fw::Model& model = models.back();
		if (!model.loadModel(modelFile)) {
			return false;
		}
		numMeshes += model.getMeshes().size();
		++numModels;
		std::cout << "Loaded model " << modelFile << "\n";
	}

	transformMatrixBufferSize = numModels * sizeof(glm::mat4);
	indexToMatrixBufferSize = numMeshes * sizeof(unsigned int);

	transforms.resize(numModels);
	axes.resize(numModels);
	modelMatrices.resize(numModels);
	mvpMatrices.resize(numModels);

	transforms[0].scale = glm::vec3(0.07f, 0.07f, 0.07f);
	transforms[0].position = glm::vec3(4.0f, 0.0f, 0.0f);
	axes[0] = glm::vec3(0.0f, 1.0f, 0.0f);
	axes[1] = glm::vec3(1.0f, 0.0f, 0.0f);

	createVertexbuffer(models);
	createTransformBuffers();
	createCommandBuffer(models);

	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	return true;
}

void IndirectDrawingApplication::update()
{
	fw::toggleRelativeMouseMode();

	if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
		cameraController.update();
	}

	glm::mat4 viewMatrix = camera.updateViewMatrix();

	for (unsigned int i = 0; i < numModels; ++i) {
		transforms[i].rotate(axes[i], 0.2f * fw::Framework::getFrameTime());
		modelMatrices[i] = transforms[i].updateModelMatrix();
		mvpMatrices[i] = camera.getProjectionMatrix() * viewMatrix * modelMatrices[i];
	}
}

void IndirectDrawingApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader.getProgram());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image.getTexture());

	GLbitfield access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelBuffer);
	glm::mat4* modelBufferData = (glm::mat4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, transformMatrixBufferSize, access);
	std::copy(modelMatrices.begin(), modelMatrices.end(), modelBufferData);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mvpBuffer);
	glm::mat4* mvpBufferData = (glm::mat4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, transformMatrixBufferSize, access);
	std::copy(mvpMatrices.begin(), mvpMatrices.end(), mvpBufferData);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindVertexArray(VAO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, numMeshes, 0);
}

void IndirectDrawingApplication::gui()
{
	fw::displayFps();
	fw::displayVec3("Position %.1f %.1f %.1f", camera.getTransformation().position);
	fw::displayVec3("Rotation %.1f %.1f %.1f", camera.getTransformation().rotation);
}

void IndirectDrawingApplication::createVertexbuffer(const std::vector<fw::Model>& models)
{
	std::vector<float> AOS;
	std::vector<unsigned int> indices;
	unsigned int indexOffset = 0;

	for (const auto& model : models) {
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
	}

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

void IndirectDrawingApplication::createTransformBuffers()
{
	glGenBuffers(1, &modelBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, transformMatrixBufferSize, nullptr, GL_MAP_WRITE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, modelBufferIndex, modelBuffer);

	glGenBuffers(1, &mvpBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mvpBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, transformMatrixBufferSize, nullptr, GL_MAP_WRITE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, mvpBufferIndex, mvpBuffer);

	glGenBuffers(1, &indexToMatrixBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexToMatrixBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, indexToMatrixBufferSize, nullptr, GL_MAP_WRITE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, indexToMatrixBufferIndex, indexToMatrixBuffer);
}

void IndirectDrawingApplication::createCommandBuffer(const std::vector<fw::Model>& models)
{
	std::size_t cmdSize = numMeshes * sizeof(IndirectCommand);
	glGenBuffers(1, &commandBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
	glBufferStorage(GL_DRAW_INDIRECT_BUFFER, cmdSize, nullptr, GL_MAP_WRITE_BIT);
	GLbitfield access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
	IndirectCommand* drawCmd = (IndirectCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, cmdSize, access);

	GLuint firstIndex = 0;
	unsigned int cmdIndex = 0;
	for (const auto& model : models) {
		for (const auto& mesh : model.getMeshes()) {
			drawCmd[cmdIndex].count = mesh.indices.size();
			drawCmd[cmdIndex].instanceCount = 1;
			drawCmd[cmdIndex].firstIndex = firstIndex;
			drawCmd[cmdIndex].baseVertex = 0;
			drawCmd[cmdIndex].baseInstance = 0;
			firstIndex += drawCmd[cmdIndex].count;
			++cmdIndex;
		}
	}
	glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexToMatrixBuffer);
	unsigned int* indexToMatrix = (unsigned int*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, indexToMatrixBufferSize, access);
	unsigned int index = 0;
	for (unsigned int i = 0; i < numModels; ++i) {
		for (unsigned int j = 0; j < models[i].getMeshes().size(); ++j) {
			indexToMatrix[index] = i;
			++index;
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}