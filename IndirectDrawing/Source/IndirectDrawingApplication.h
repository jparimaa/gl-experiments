#pragma once

#include <Framework/Application.h>
#include <Framework/Camera.h>
#include <Framework/CameraController.h>
#include <Framework/Shader.h>
#include <Framework/Image.h>
#include <Framework/Model.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

struct IndirectCommand
{
	GLuint vertexCount = 0;
	GLuint instanceCount = 0;
	GLuint firstIndex = 0;
	GLuint baseVertex = 0;
	GLuint baseInstance = 0;
};

class IndirectDrawingApplication : public fw::Application
{
public:
	explicit IndirectDrawingApplication();
	virtual ~IndirectDrawingApplication();
	IndirectDrawingApplication(const IndirectDrawingApplication&) = delete;
	IndirectDrawingApplication(IndirectDrawingApplication&&) = delete;
	IndirectDrawingApplication& operator=(const IndirectDrawingApplication&) = delete;
	IndirectDrawingApplication& operator=(IndirectDrawingApplication&&) = delete;

	virtual bool initialize() final;
	virtual void update() final;
	virtual void render() final;
	virtual void gui() final;

private:
	fw::Camera camera;
	fw::CameraController cameraController;
	fw::Shader shader;
	fw::Image image;

	unsigned int numModels = 0;
	unsigned int numMeshes = 0;
	std::size_t matrixBufferSize = 0;

	GLuint commandBuffer = 0;
	IndirectCommand* drawCmd = nullptr;

	GLuint VAO = 0;
	GLuint vertexBuffer = 0;
	GLuint indexBuffer = 0;

	GLuint modelBuffer = 0;
	GLuint mvpBuffer = 1;

	std::vector<fw::Transformation> transforms;
	std::vector<glm::vec3> rotationAxes;
	std::vector<glm::mat4> modelMatrices;
	std::vector<glm::mat4> mvpMatrices;

	void createCommandBuffer(const fw::Model& model);
	void createVertexbuffer(const fw::Model& model);
	void createTransformBuffers();
};
