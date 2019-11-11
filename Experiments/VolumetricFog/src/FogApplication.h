#pragma once

#include <fw/Application.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <fw/Shader.h>
#include <fw/Model.h>
#include <fw/Image.h>

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <vector>

class FogApplication : public fw::Application
{
public:
    explicit FogApplication();
    virtual ~FogApplication();

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    struct RenderObject
    {
        fw::Transformation transform;
        glm::mat4 mvp;
    };

    fw::Camera camera;
    fw::CameraController cameraController;
    fw::Shader shader;
    fw::Image image;
    std::vector<RenderObject> renderObjects;
    GLuint VAO = 0;
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;
    unsigned int numIndices = 0;

    void createBuffers(const fw::Model& model);
    void createScene();
};
