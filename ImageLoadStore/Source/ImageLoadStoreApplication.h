#pragma once

#include <Framework/Application.h>
#include <Framework/Shader.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class ImageLoadStoreApplication : public fw::Application
{
public:
	explicit ImageLoadStoreApplication();
	virtual ~ImageLoadStoreApplication();
	ImageLoadStoreApplication(const ImageLoadStoreApplication&) = delete;
	ImageLoadStoreApplication(ImageLoadStoreApplication&&) = delete;
	ImageLoadStoreApplication& operator=(const ImageLoadStoreApplication&) = delete;
	ImageLoadStoreApplication& operator=(ImageLoadStoreApplication&&) = delete;

	virtual bool initialize() final;
	virtual void update() final;
	virtual void render() final;
	virtual void gui() final;

private:
	fw::Shader shader;
};
