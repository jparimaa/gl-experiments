#include "ImageLoadStoreApplication.h"
#include <Framework/Framework.h>
#include <Framework/Common.h>
#include <iostream>

namespace
{

const GLuint widthLocation = 0;
const GLuint heightLocation = 1;
const GLuint timeLocation = 2;
const GLuint texBindingPoint = 6;

} // anonymous

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

	auto loadShader = [&] (const std::string& shaderPath, fw::Shader* shader) {
		std::vector<std::string> shaderFiles = {
			shaderPath + ".vert",
			shaderPath + ".frag"
		};
		if (!shader->createProgram(shaderFiles)) {
			return false;
		}
		std::cout << "Loaded shader " << shaderPath << " (" << shader->getProgram() << ")\n";
		return true;
	};

	if (!loadShader("Shaders/loadstore", &loadStoreShader) || !loadShader("Shaders/display", &displayShader)) {
		return false;
	}

	glGenVertexArrays(1, &VAO); // VAO is required even though it is empty
	glBindVertexArray(VAO);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8UI, width, height);
	glBindImageTexture(texBindingPoint, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
		
	return true;
}

void ImageLoadStoreApplication::update()
{
}

void ImageLoadStoreApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(loadStoreShader.getProgram());
	glUniform1i(widthLocation, width);
	glUniform1i(heightLocation, height);
	// Attributeless draw call for each pixel, all calculations in vertex shader
	glDrawArrays(GL_POINTS, 0, width * height);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glUseProgram(displayShader.getProgram());
	glUniform1i(widthLocation, width);
	glUniform1i(heightLocation, height);
	glUniform1f(timeLocation, fw::Framework::getTimeSinceStart());
	// Attributeless draw call for a triangle that covers the screen
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void ImageLoadStoreApplication::gui()
{
}
