#pragma once

#include <fw/Application.h>
#include <fw/Shader.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class TextureArrayApplication : public fw::Application
{
public:
    explicit TextureArrayApplication();
    virtual ~TextureArrayApplication();
    TextureArrayApplication(const TextureArrayApplication&) = delete;
    TextureArrayApplication(TextureArrayApplication&&) = delete;
    TextureArrayApplication& operator=(const TextureArrayApplication&) = delete;
    TextureArrayApplication& operator=(TextureArrayApplication&&) = delete;

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    fw::Shader shader;
    GLuint textureArray = 0;
    GLuint VAO = 0;
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;

    void createVertexBuffer();
    void createTextures();
};
