#pragma once

#include <Framework/Application.h>
#include <Framework/Camera.h>
#include <Framework/Shader.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class ExampleApplication : public fw::Application
{
public:
	explicit ExampleApplication();
	virtual ~ExampleApplication();
	ExampleApplication(const ExampleApplication&) = delete;
	ExampleApplication(ExampleApplication&&) = delete;
	ExampleApplication& operator=(const ExampleApplication&) = delete;
	ExampleApplication& operator=(ExampleApplication&&) = delete;

	virtual bool initialize() final;
	virtual void update() final;
	virtual void render() final;

private:
	fw::Camera camera;
	fw::Shader shader;
	glm::mat4 mvpMatrix;
	GLuint texture = 0;
	GLuint VAO = 0;
	GLuint VBO = 0;	
	GLuint EBO = 0;
};
