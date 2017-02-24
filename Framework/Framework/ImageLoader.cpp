#include "ImageLoader.h"
#include <iostream>

namespace fw
{

ImageLoader::ImageLoader()
{
}

ImageLoader::~ImageLoader()
{
	clearData();
}

unsigned char* ImageLoader::loadImage(const std::string& file, int& width, int& height, int& channels)
{
	clearData();
	data = SOIL_load_image(file.c_str(), &width, &height, &channels, format);
	if (!data) {
		std::cerr << "ERROR: Could not load texture file " << file << "\n";
		return nullptr;
	}
	return data;
}

void ImageLoader::clearData()
{
	if (data) {
		SOIL_free_image_data(data);
	}
}

} // fw