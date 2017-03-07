#include "MultiDrawApplication.h"
#include <Framework/Framework.h>
#include <Framework/Image.h>
#include <Framework/Model.h>
#include <Framework/Input.h>
#include <Framework/imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace
{

const GLint mvpMatrixLocation = 0;
const GLint timeLocation = 1;
const GLint textureLocation = 2;

} // anonymous

MultiDrawApplication::MultiDrawApplication()
{
}

MultiDrawApplication::~MultiDrawApplication()
{
	glDeleteVertexArrays(distinct.numBuffers, distinct.VAOs.data());
	glDeleteBuffers(distinct.numBuffers, distinct.vertexBuffers.data());
	glDeleteBuffers(distinct.numBuffers, distinct.uvBuffers.data());
	glDeleteBuffers(distinct.numBuffers, distinct.indexBuffers.data());

	glDeleteVertexArrays(1, &multi.VAO);
	glDeleteBuffers(1, &multi.vertexBuffer);
	glDeleteBuffers(1, &multi.uvBuffer);
	glDeleteBuffers(1, &multi.indexBuffer);

	glDeleteTextures(1, &texture);
}

bool MultiDrawApplication::initialize()
{
	cameraController.setCamera(&camera);
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
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

	// Model
	fw::Model model;
	std::string modelFile = "../Assets/Models/sponza.obj";
	if (model.loadModel(modelFile)) {
		std::cout << "Loaded model " << modelFile << "\n";
	}
	if (model.getMeshes().empty()) {
		std::cerr << "ERROR: Empty model\n";
		return false;
	}

	// Buffers
	distinct.numBuffers = model.getMeshes().size();
	distinct.VAOs.resize(distinct.numBuffers);
	distinct.vertexBuffers.resize(distinct.numBuffers);
	distinct.uvBuffers.resize(distinct.numBuffers);
	distinct.indexBuffers.resize(distinct.numBuffers);
	distinct.numIndices.resize(distinct.numBuffers);

	glGenVertexArrays(distinct.numBuffers, distinct.VAOs.data());
	glGenBuffers(distinct.numBuffers, distinct.vertexBuffers.data());
	glGenBuffers(distinct.numBuffers, distinct.uvBuffers.data());
	glGenBuffers(distinct.numBuffers, distinct.indexBuffers.data());


	for (GLsizei i = 0; i < distinct.numBuffers; ++i) {
		glBindVertexArray(distinct.VAOs[i]);

		const fw::Model::Mesh& mesh = model.getMeshes()[i];

		GLsizeiptr vertexSize = sizeof(glm::vec3) * mesh.vertices.size();
		glBindBuffer(GL_ARRAY_BUFFER, distinct.vertexBuffers[i]);
		glBufferData(GL_ARRAY_BUFFER, vertexSize, &mesh.vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		GLsizeiptr uvSize = sizeof(glm::vec2) * mesh.uvs.size();
		glBindBuffer(GL_ARRAY_BUFFER, distinct.uvBuffers[i]);
		glBufferData(GL_ARRAY_BUFFER, uvSize, &mesh.uvs[0], GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		distinct.numIndices[i] = mesh.indices.size();
		GLsizeiptr indexSize = sizeof(unsigned int) * distinct.numIndices[i];
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, distinct.indexBuffers[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, &mesh.indices[0], GL_STATIC_DRAW);
	}

	objTransformation.scale = glm::vec3(0.003f, 0.003f, 0.003f);
	objTransformation.updateModelMatrix();

	return true;
}

void MultiDrawApplication::update()
{
	if (fw::Input::isKeyDown(SDLK_r)) {
		fw::Transformation& t = camera.getTransformation();
		t.position = glm::vec3(-1.0f, 1.0f, 0.0f);
		t.rotation = glm::vec3(0.0f, 0.6f, 0.0f);
	}

	cameraController.update();

	glm::mat4 viewMatrix = camera.updateViewMatrix();
	mvpMatrix = camera.getProjectionMatrix() * viewMatrix * objTransformation.getModelMatrix();
}

void MultiDrawApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader.getProgram());

	glUniformMatrix4fv(mvpMatrixLocation, 1, 0, glm::value_ptr(mvpMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureLocation, 0);

	for (GLsizei i = 0; i < distinct.numBuffers; ++i) {
		glBindVertexArray(distinct.VAOs[i]);
		glDrawElements(GL_TRIANGLES, distinct.numIndices[i], GL_UNSIGNED_INT, 0);
	}
}

void MultiDrawApplication::gui()
{
	//ImGui::Begin("ImGui Window");
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	std::stringstream ss;
	ss << std::setprecision(1) 
		<< objTransformation.position.x << " "
		<< objTransformation.position.y << " "
		<< objTransformation.position.z;
	ImGui::Text("Position %.1f %.1f %.1f", 
				camera.getTransformation().position.x,
				camera.getTransformation().position.y,
				camera.getTransformation().position.z);
	//ImGui::End();
}
