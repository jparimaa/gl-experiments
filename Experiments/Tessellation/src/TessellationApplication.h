#pragma once

#include <fw/Application.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <fw/Shader.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class TessellationApplication : public fw::Application
{
public:
    explicit TessellationApplication();
    virtual ~TessellationApplication();
    TessellationApplication(const TessellationApplication&) = delete;
    TessellationApplication(TessellationApplication&&) = delete;
    TessellationApplication& operator=(const TessellationApplication&) = delete;
    TessellationApplication& operator=(TessellationApplication&&) = delete;

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    fw::Camera camera;
    fw::CameraController cameraController;
    fw::Shader shader;
    glm::mat4 mvpMatrix;
    GLuint VAO = 0;
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;
    unsigned int numIndices = 0;

    float tessLevelInner = 1.0f;
    float tessLevelOuter = 1.0f;

    void createBuffers();
};
