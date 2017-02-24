#pragma once

#include <Framework/Application.h>
#include <Framework/Shader.h>
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
	fw::Shader shader;
	GLuint texture = 0;
	GLint timeLocation = 0;
	GLint textureLocation = 0;
	GLuint VAO = 0;
	GLuint VBO = 0;	
	GLuint EBO = 0;
};
