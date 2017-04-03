#pragma once

#include <Framework/Application.h>
#include <Framework/Camera.h>
#include <Framework/CameraController.h>
#include <Framework/Shader.h>
#include <Framework/Model.h>
#include <Framework/Image.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class GeometryShaderApplication : public fw::Application
{
public:
	explicit GeometryShaderApplication();
	virtual ~GeometryShaderApplication();
	GeometryShaderApplication(const GeometryShaderApplication&) = delete;
	GeometryShaderApplication(GeometryShaderApplication&&) = delete;
	GeometryShaderApplication& operator=(const GeometryShaderApplication&) = delete;
	GeometryShaderApplication& operator=(GeometryShaderApplication&&) = delete;

	virtual bool initialize() final;
	virtual void update() final;
	virtual void render() final;
	virtual void gui() final;

private:
	fw::Camera camera;
	fw::CameraController cameraController;
	fw::Shader shader;
	fw::Image image;
	glm::mat4 mvpMatrix;
};
