#include "fw/Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <cmath>
#include <algorithm>

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
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(file.c_str(), &width, &height, &channels, 4);
    if (!data)
    {
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
    GLsizei numMipmaps = static_cast<GLsizei>(std::floor(log2(static_cast<double>(std::max(width, height)))));
    glTexStorage2D(target, numMipmaps, internalFormat, width, height);
    glTexSubImage2D(target, 0, 0, 0, width, height, format, type, data);
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
    if (data)
    {
        stbi_image_free(data);
    }
}

} // namespace fw
