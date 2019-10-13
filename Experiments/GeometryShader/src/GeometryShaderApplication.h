#pragma once

#include <fw/Application.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <fw/Shader.h>
#include <fw/Model.h>
#include <fw/Image.h>
#include <fw/Transformation.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class GeometryShaderApplication : public fw::Application
{
public:
    explicit GeometryShaderApplication();
    virtual ~GeometryShaderApplication();
    GeometryShaderApplication(const GeometryShaderApplication&) = delete;
    GeometryShaderApplication(GeometryShaderApplication&&) = delete;
    GeometryShaderApplication& operator=(const GeometryShaderApplication&) = delete;
    GeometryShaderApplication& operator=(GeometryShaderApplication&&) = delete;

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    fw::Camera camera;
    fw::CameraController cameraController;
    fw::Shader shader;
    fw::Image image;
    glm::mat4 modelViewMatrix;
    fw::Transformation transform;

    unsigned int numIndices = 0;
    GLuint VAO = 0;
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;

    void createBuffer(const fw::Model& model);
};
