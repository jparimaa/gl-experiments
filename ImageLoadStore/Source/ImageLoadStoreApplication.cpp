#include "ImageLoadStoreApplication.h"
#include <Framework/Framework.h>
#include <Framework/Common.h>
#include <iostream>

ImageLoadStoreApplication::ImageLoadStoreApplication()
{
}

ImageLoadStoreApplication::~ImageLoadStoreApplication()
{
}

bool ImageLoadStoreApplication::initialize()
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Shader
	std::string shaderPath = "Shaders/loadstore";
	std::vector<std::string> shaderFiles = {
		shaderPath + ".vert",
		shaderPath + ".frag"
	};
	if (!shader.createProgram(shaderFiles)) {
		return false;
	}
	std::cout << "Loaded shader " << shaderPath << " (" << shader.getProgram() << ")\n";
	
	return true;
}

void ImageLoadStoreApplication::update()
{
}

void ImageLoadStoreApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader.getProgram());
}

void ImageLoadStoreApplication::gui()
{
}
