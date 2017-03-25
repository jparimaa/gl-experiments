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
};
