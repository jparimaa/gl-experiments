#include "Image.h"
#include <iostream>

namespace fw
{

Image::Image()
{
}

Image::~Image()
{
	clearData();	
}

bool Image::load(const std::string& file)
{
	clearData();
	data = SOIL_load_image(file.c_str(), &width, &height, &channels, soilFormat);
	if (!data) {
		std::cerr << "ERROR: Could not load texture file " << file << "\n";
		return false;
	}
	return true;
}

unsigned char* Image::getData() const
{
	return data;
}

int Image::getWidth() const
{
	return width;
}

int Image::getHeight() const
{
	return height;
}

int Image::getChannels() const
{
	return channels;
}

GLuint Image::create2dTexture()
{
	glGenTextures(1, &texture);
	glBindTexture(target, texture);
	glTexImage2D(target, level, internalFormat, width, height, 0, format, type, data);
	glGenerateMipmap(target);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, sWrap);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, tWrap);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
	return texture;
}

GLuint Image::getTexture() const
{
	return texture;
}

void Image::clearData()
{
	glDeleteTextures(1, &texture);
	if (data) {
		SOIL_free_image_data(data);
	}
}

} // fw
