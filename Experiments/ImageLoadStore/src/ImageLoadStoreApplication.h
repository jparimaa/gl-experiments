#pragma once

#include <fw/Application.h>
#include <fw/Shader.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class ImageLoadStoreApplication : public fw::Application
{
public:
    explicit ImageLoadStoreApplication();
    virtual ~ImageLoadStoreApplication();
    ImageLoadStoreApplication(const ImageLoadStoreApplication&) = delete;
    ImageLoadStoreApplication(ImageLoadStoreApplication&&) = delete;
    ImageLoadStoreApplication& operator=(const ImageLoadStoreApplication&) = delete;
    ImageLoadStoreApplication& operator=(ImageLoadStoreApplication&&) = delete;

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    fw::Shader loadStoreShader;
    fw::Shader displayShader;
    GLuint VAO;
    GLuint texture;

    int width = 512;
    int height = 512;
};
