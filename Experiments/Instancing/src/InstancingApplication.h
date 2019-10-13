#pragma once

#include <fw/Application.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <fw/Shader.h>
#include <fw/Image.h>
#include <fw/Model.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <array>

const GLsizei numPrimitives = 100;

class InstancingApplication : public fw::Application
{
public:
    explicit InstancingApplication();
    virtual ~InstancingApplication();
    InstancingApplication(const InstancingApplication&) = delete;
    InstancingApplication(InstancingApplication&&) = delete;
    InstancingApplication& operator=(const InstancingApplication&) = delete;
    InstancingApplication& operator=(InstancingApplication&&) = delete;

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    fw::Camera camera;
    fw::CameraController cameraController;
    fw::Shader shader;
    fw::Image image;

    GLuint VAO = 0;
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;
    GLsizei numIndices = 0;

    GLuint transformationMatricesBuffer = 0;
    std::array<glm::vec3, numPrimitives> rotationAxes;
    std::array<fw::Transformation, numPrimitives> transformations;
    std::array<glm::mat4, numPrimitives> modelMatrices;
    std::array<glm::mat4, numPrimitives> mvpMatrices;

    void createVertexBuffer(const fw::Model& model);
    void createUniformBuffer();
};
