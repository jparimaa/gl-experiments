#pragma once

#include <fw/Application.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <fw/Shader.h>
#include <fw/Model.h>
#include <fw/Image.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class DSAApplication : public fw::Application
{
public:
    explicit DSAApplication();
    virtual ~DSAApplication();
    DSAApplication(const DSAApplication&) = delete;
    DSAApplication(DSAApplication&&) = delete;
    DSAApplication& operator=(const DSAApplication&) = delete;
    DSAApplication& operator=(DSAApplication&&) = delete;

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    fw::Camera camera;
    fw::CameraController cameraController;
    fw::Shader shader;
    fw::Image image;
    fw::Transformation objTransformation;
    glm::mat4 mvpMatrix;
    GLuint texture = 0;
    GLuint sampler = 0;
    GLuint VAO = 0;
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;
    GLuint uniformBuffer = 0;
    unsigned int numIndices = 0;

    void createTexture(const fw::Image& image);
    void createSampler();
    void createBuffers(const fw::Model& model);
};
