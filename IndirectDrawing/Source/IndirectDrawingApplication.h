#pragma once

#include <Framework/Application.h>
#include <Framework/Camera.h>
#include <Framework/CameraController.h>
#include <Framework/Shader.h>
#include <Framework/Image.h>
#include <Framework/Model.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

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
};
