#pragma once

#include <Framework/Application.h>
#include <Framework/Camera.h>
#include <Framework/CameraController.h>
#include <Framework/Shader.h>
#include <Framework/Model.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class TessellationApplication : public fw::Application
{
public:
	explicit TessellationApplication();
	virtual ~TessellationApplication();
	TessellationApplication(const TessellationApplication&) = delete;
	TessellationApplication(TessellationApplication&&) = delete;
	TessellationApplication& operator=(const TessellationApplication&) = delete;
	TessellationApplication& operator=(TessellationApplication&&) = delete;

	virtual bool initialize() final;
	virtual void update() final;
	virtual void render() final;
	virtual void gui() final;

private:
	fw::Camera camera;
	fw::CameraController cameraController;
	fw::Shader shader;
	glm::mat4 mvpMatrix;	
	GLuint VAO = 0;
	GLuint vertexBuffer = 0;
	GLuint indexBuffer = 0;
	unsigned int numIndices = 0;

	float tessLevelInner = 1.0f;
	float tessLevelOuter = 1.0f;

	void createBuffers(const fw::Model& model);
};
