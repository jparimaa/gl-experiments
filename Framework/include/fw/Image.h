#pragma once

#include <GL/glew.h>

#include <string>
#include <vector>

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

    bool load(const std::string& file);
    unsigned char* getData() const;
    int getWidth() const;
    int getHeight() const;
    int getChannels() const;
    GLuint create2dTexture();
    GLuint getTexture() const;

    // Todo: Set texture parameters.

private:
    unsigned char* data = nullptr;
    int width = 0;
    int height = 0;
    int channels = 0;

    GLuint texture = 0;
    GLenum target = GL_TEXTURE_2D;
    GLint level = 0;
    GLint internalFormat = GL_RGBA8;
    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;

    GLint sWrap = GL_REPEAT;
    GLint tWrap = GL_REPEAT;
    GLint minFilter = GL_NEAREST_MIPMAP_LINEAR;
    GLint magFilter = GL_LINEAR;

    void clearData();
};
} // namespace fw
