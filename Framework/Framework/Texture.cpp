#include "Texture.h"
#include <SOIL.h>
#include <iostream>

namespace fw
{

Texture::Texture()
{
	glGenTextures(1, &id);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

bool Texture::setTexImage2D(const std::string& file)
{
	glBindTexture(GL_TEXTURE_2D, id);

	int width;
	int height;
	int channels;
	unsigned char* data = SOIL_load_image(file.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);

	if (!data) {
		std::cerr << "ERROR: Could not load texture file " << file << "\n";
		return false;
	}

	glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, 0, format, type, data);

	SOIL_free_image_data(data);

	return true;
}

GLuint Texture::getId() const
{
	return id;
}

} // fw