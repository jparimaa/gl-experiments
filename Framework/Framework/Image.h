#pragma once

#include <string>
#include <vector>
#include <SOIL.h>

namespace fw
{

class Image
{
public:
	explicit Image();
	~Image();
	Image(const Image&) = delete;
	Image(Image&&) = delete;
	Image& operator=(const Image&) = delete;
	Image& operator=(Image&&) = delete;

	bool load(const std::string& file, int& width, int& height, int& channels);
	unsigned char* getData() const;

private:	
	unsigned char* data = nullptr;
	int format = SOIL_LOAD_RGBA;

	void clearData();
};

} // fw
