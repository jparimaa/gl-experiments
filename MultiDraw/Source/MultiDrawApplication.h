#pragma once

#include <Framework/Application.h>
#include <Framework/Camera.h>
#include <Framework/CameraController.h>
#include <Framework/Shader.h>
#include <Framework/Model.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

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
	struct DistinctBuffers
	{
		GLsizei numBuffers = 0;
		std::vector<GLuint> VAOs;
		std::vector<GLuint> vertexBuffers;
		std::vector<GLuint> uvBuffers;
		std::vector<GLuint> indexBuffers;
		std::vector<GLsizei> numIndices;
	};

	struct MultiBuffer
	{
		GLuint VAO = 0;
		GLuint vertexBuffer = 0;
		GLuint uvBuffer = 0;
		GLuint indexBuffer = 0;
		GLsizei numIndices = 0;
		std::vector<GLsizei> counts;
		std::vector<GLsizei> indexLocations;
	};

	fw::Camera camera;
	fw::CameraController cameraController;
	fw::Shader shader;
	fw::Transformation objTransformation;
	glm::mat4 mvpMatrix;
	GLuint texture = 0;
	DistinctBuffers distinct;
	MultiBuffer multi;
	bool renderMultiBuffer = true;
	unsigned int drawCalls = 0;

	void createDistinctBuffers(const fw::Model& model);
	void createMultiBuffer(const fw::Model& model);
};
