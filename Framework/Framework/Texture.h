#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

namespace fw
{

class Texture
{
public:
	explicit Texture();
	~Texture();
	Texture(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&&) = delete;

	bool setTexImage2D(const std::string& file);

	GLuint getId() const;

private:
	GLuint id = 0;
	GLint level = 0;
	GLint internalFormat = GL_RGBA;
	GLenum format = GL_RGBA;
	GLenum type = GL_UNSIGNED_BYTE;
};

} // fw