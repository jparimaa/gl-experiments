#pragma once

#include <Framework/Application.h>
#include <Framework/Camera.h>
#include <Framework/CameraController.h>
#include <Framework/Shader.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class MultiDrawApplication : public fw::Application
{
public:
	explicit MultiDrawApplication();
	virtual ~MultiDrawApplication();
	MultiDrawApplication(const MultiDrawApplication&) = delete;
	MultiDrawApplication(MultiDrawApplication&&) = delete;
	MultiDrawApplication& operator=(const MultiDrawApplication&) = delete;
	MultiDrawApplication& operator=(MultiDrawApplication&&) = delete;

	virtual bool initialize() final;
	virtual void update() final;
	virtual void render() final;
	virtual void gui() final;

private:
	fw::Camera camera;
	fw::CameraController cameraController;
	fw::Shader shader;
	fw::Transformation objTransformation;
	glm::mat4 mvpMatrix;
	GLuint texture = 0;
	GLuint VAO = 0;
	GLuint vertexBuffer = 0;	
	GLuint uvBuffer = 0;
	GLuint indexBuffer = 0;
	unsigned int numIndices = 0;
};
