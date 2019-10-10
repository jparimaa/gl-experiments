#pragma once

#include <Framework/Application.h>
#include <Framework/Shader.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class BindlessTextureApplication : public fw::Application
{
public:
	explicit BindlessTextureApplication();
	virtual ~BindlessTextureApplication();
	BindlessTextureApplication(const BindlessTextureApplication&) = delete;
	BindlessTextureApplication(BindlessTextureApplication&&) = delete;
	BindlessTextureApplication& operator=(const BindlessTextureApplication&) = delete;
	BindlessTextureApplication& operator=(BindlessTextureApplication&&) = delete;

	virtual bool initialize() final;
	virtual void update() final;
	virtual void render() final;
	virtual void gui() final;

private:
	fw::Shader shader;

	GLuint texture = 0;
	GLuint64 handle = 0;
	GLuint textureBuffer = 0;

	GLuint VAO = 0;
	GLuint vertexBuffer = 0;
	GLuint indexBuffer = 0;

	void createTextures();
	void createVertexBuffer();
};
