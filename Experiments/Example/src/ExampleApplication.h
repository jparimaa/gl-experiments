#pragma once

#include <fw/Application.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <fw/Shader.h>
#include <fw/Model.h>
#include <fw/Image.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class ExampleApplication : public fw::Application
{
public:
    explicit ExampleApplication();
    virtual ~ExampleApplication();
    ExampleApplication(const ExampleApplication&) = delete;
    ExampleApplication(ExampleApplication&&) = delete;
    ExampleApplication& operator=(const ExampleApplication&) = delete;
    ExampleApplication& operator=(ExampleApplication&&) = delete;

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
    GLuint VAO = 0;
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;
    unsigned int numIndices = 0;

    void createBuffers(const fw::Model& model);
};
