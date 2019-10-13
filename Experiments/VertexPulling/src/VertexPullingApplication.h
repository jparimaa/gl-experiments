#pragma once

#include <fw/Application.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <fw/Shader.h>
#include <fw/Model.h>
#include <fw/Image.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class VertexPullingApplication : public fw::Application
{
public:
    explicit VertexPullingApplication();
    virtual ~VertexPullingApplication();
    VertexPullingApplication(const VertexPullingApplication&) = delete;
    VertexPullingApplication(VertexPullingApplication&&) = delete;
    VertexPullingApplication& operator=(const VertexPullingApplication&) = delete;
    VertexPullingApplication& operator=(VertexPullingApplication&&) = delete;

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
    GLuint vnBuffer = 0; // Vertex & normal
    GLuint uvBuffer = 0;
    GLuint indexBuffer = 0;
    unsigned int numIndices = 0;
    GLuint vnTexBuffer = 0;
    GLuint uvTexBuffer = 0;

    void createBuffers(const fw::Model& model);
};
