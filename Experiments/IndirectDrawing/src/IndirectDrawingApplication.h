#pragma once

#include <fw/Application.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <fw/Shader.h>
#include <fw/Image.h>
#include <fw/Model.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

struct IndirectCommand
{
    GLuint count = 0;
    GLuint instanceCount = 0;
    GLuint firstIndex = 0;
    GLuint baseVertex = 0;
    GLuint baseInstance = 0;
};

class IndirectDrawingApplication : public fw::Application
{
public:
    explicit IndirectDrawingApplication();
    virtual ~IndirectDrawingApplication();
    IndirectDrawingApplication(const IndirectDrawingApplication&) = delete;
    IndirectDrawingApplication(IndirectDrawingApplication&&) = delete;
    IndirectDrawingApplication& operator=(const IndirectDrawingApplication&) = delete;
    IndirectDrawingApplication& operator=(IndirectDrawingApplication&&) = delete;

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    fw::Camera camera;
    fw::CameraController cameraController;
    fw::Shader shader;
    fw::Image image;

    unsigned int numModels = 0;
    unsigned int numMeshes = 0;

    GLuint commandBuffer = 0;

    GLuint VAO = 0;
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;

    std::size_t transformMatrixBufferSize = 0;
    std::size_t indexToMatrixBufferSize = 0;
    GLuint modelBuffer = 0;
    GLuint mvpBuffer = 0;
    GLuint indexToMatrixBuffer = 0;

    std::vector<fw::Transformation> transforms;
    std::vector<glm::vec3> axes;
    std::vector<glm::mat4> modelMatrices;
    std::vector<glm::mat4> mvpMatrices;

    void createCommandBuffer(const std::vector<fw::Model>& models);
    void createVertexbuffer(const std::vector<fw::Model>& models);
    void createTransformBuffers();
};
