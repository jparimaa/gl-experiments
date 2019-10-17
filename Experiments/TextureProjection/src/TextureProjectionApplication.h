#pragma once

#include <fw/Application.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <fw/Shader.h>
#include <fw/Model.h>
#include <fw/Image.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class TextureProjectionApplication : public fw::Application
{
public:
    explicit TextureProjectionApplication();
    virtual ~TextureProjectionApplication();
    TextureProjectionApplication(const TextureProjectionApplication&) = delete;
    TextureProjectionApplication(TextureProjectionApplication&&) = delete;
    TextureProjectionApplication& operator=(const TextureProjectionApplication&) = delete;
    TextureProjectionApplication& operator=(TextureProjectionApplication&&) = delete;

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    struct RenderObject
    {
        ~RenderObject();

        GLuint VAO = 0;
        GLuint vertexBuffer = 0;
        GLuint indexBuffer = 0;
        unsigned int numIndices = 0;
    };

    struct ObjectTransform
    {
        fw::Transformation objTransformation;
        glm::mat4 mvpMatrix;
        glm::mat4 projectionMatrix;
    };

    fw::Camera camera;
    fw::Camera projector;
    fw::CameraController cameraController;
    fw::Shader shader;
    fw::Image checkerImage;
    fw::Image greyImage;
    fw::Image projectionImage;

    RenderObject cube;
    ObjectTransform cubeTrans;
    ObjectTransform floorTrans;

    void createBuffers(const fw::Model& model, RenderObject& renderObject);
};
