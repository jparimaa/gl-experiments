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
    virtual void gui() final{};

private:
    struct RenderObject
    {
        fw::Transformation transform;
        glm::mat4 mvpMatrix;
    };

    fw::Camera camera;
    fw::CameraController cameraController;
    fw::Shader diffuseShader;
    fw::Shader shadowMapShader;
    fw::Shader simpleShader;
    fw::Image image;

    std::vector<RenderObject> renderObjects;
    std::vector<RenderObject> lightRenderObjects;

    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightDirections;
    std::vector<glm::vec4> lightColors;
    std::vector<glm::mat4> lightSpaceMatrices;

    GLuint VAO = 0;
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;
    unsigned int numIndices = 0;

    std::vector<GLuint> shadowMapBuffers;
    std::vector<GLuint> shadowMapTextures;

    void createBuffers(const fw::Model& model);
    void createShadowMaps();
    void createScene();
};
