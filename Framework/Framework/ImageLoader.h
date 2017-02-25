#pragma once

#include <string>
#include <vector>
#include <SOIL.h>

namespace fw
{

class ImageLoader
{
public:
	explicit ImageLoader();
	~ImageLoader();
	ImageLoader(const ImageLoader&) = delete;
	ImageLoader(ImageLoader&&) = delete;
	ImageLoader& operator=(const ImageLoader&) = delete;
	ImageLoader& operator=(ImageLoader&&) = delete;

	unsigned char* loadImage(const std::string& file, int& width, int& height, int& channels);

private:	
	unsigned char* data = nullptr;
	int format = SOIL_LOAD_RGBA;

	void clearData();
};

} // fw
