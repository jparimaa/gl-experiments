#include "TextureArrayApplication.h"
#include <Framework/Framework.h>
#include <iostream>

TextureArrayApplication::TextureArrayApplication()
{
}

TextureArrayApplication::~TextureArrayApplication()
{
}

bool TextureArrayApplication::initialize()
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Shader
	std::string shaderPath = "Shaders/array";
	std::vector<std::string> shaderFiles = {
		shaderPath + ".vert",
		shaderPath + ".frag"
	};
	if (shader.createProgram(shaderFiles)) {
		std::cout << "Loaded shader " << shaderPath << " (" << shader.getProgram() << ")\n";
	} else {
		std::cerr << "ERROR: Shader creation failed\n";
		return false;
	}
	
	return true;
}

void TextureArrayApplication::update()
{
}

void TextureArrayApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader.getProgram());
}

void TextureArrayApplication::gui()
{
}
