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

bool Image::load(const std::string& file, int& width, int& height, int& channels)
{
	clearData();
	data = SOIL_load_image(file.c_str(), &width, &height, &channels, format);
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

void Image::clearData()
{
	if (data) {
		SOIL_free_image_data(data);
	}
}

} // fw
