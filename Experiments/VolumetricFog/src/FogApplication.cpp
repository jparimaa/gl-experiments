#include "FogApplication.h"

#include <fw/Framework.h>
#include <fw/Input.h>
#include <fw/Common.h>

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <cmath>

namespace
{
const GLint mvpMatrixLocation = 0;
const GLint lightSpaceMatrixLocation = 0;
const GLint modelMatrixLocation = 1;
const GLint lightDirectionsLocation = 10;
const GLint lightColorsLocation = 14;
const GLint lightColorLocation = 2;
const GLsizei numLights = 2;
const int shadowMapWidth = 1024;
const int shadowMapHeight = 1024;

} // namespace

FogApplication::FogApplication()
{
}

FogApplication::~FogApplication()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
}

bool FogApplication::initialize()
{
    fw::printSystemInfo();

    glm::vec3 p = glm::vec3(0.0f, 2.0f, 20.f);
    camera.getTransformation().position = p;
    cameraController.setCamera(&camera);
    cameraController.setResetMode(p, glm::vec3(0.0f, 0.0f, 0.0f), SDLK_r);
    cameraController.setMovementSpeed(3.0f);

    std::string path = std::string(ROOT_PATH) + "Experiments/VolumetricFog/shaders/diffuse";
    if (!diffuseShader.createProgram({path + ".vert", path + ".frag"}))
    {
        return false;
    }
    std::cout << "Loaded shader " << path << " (" << diffuseShader.getProgram() << ")\n";

    path = std::string(ROOT_PATH) + "Experiments/VolumetricFog/shaders/shadowMap";
    if (!shadowMapShader.createProgram({path + ".vert", path + ".frag"}))
    {
        return false;
    }
    std::cout << "Loaded shader " << path << " (" << shadowMapShader.getProgram() << ")\n";

    path = std::string(ROOT_PATH) + "Experiments/VolumetricFog/shaders/simple";
    if (!simpleShader.createProgram({path + ".vert", path + ".frag"}))
    {
        return false;
    }
    std::cout << "Loaded shader " << path << " (" << simpleShader.getProgram() << ")\n";

    fw::Model model;
    std::string modelFile = std::string(ASSETS_PATH) + "Models/cube.obj";
    if (!model.loadModel(modelFile))
    {
        return false;
    }
    std::cout << "Loaded model " << modelFile << "\n";

    std::string textureFile = std::string(ASSETS_PATH) + "Textures/checker.png";
    if (!image.load(textureFile))
    {
        return false;
    }
    image.create2dTexture();
    std::cout << "Loaded texture " << textureFile << " (" << image.getTexture() << ")\n";

    createBuffers(model);
    createShadowMaps();
    createScene();

    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    return true;
}

void FogApplication::update()
{
    fw::toggleRelativeMouseMode();

    cameraController.update();

    glm::mat4 viewMatrix = camera.updateViewMatrix();

    for (RenderObject& ro : renderObjects)
    {
        ro.mvpMatrix = camera.getProjectionMatrix() * viewMatrix * ro.transform.getModelMatrix();
    }

    for (RenderObject& ro : lightRenderObjects)
    {
        ro.mvpMatrix = camera.getProjectionMatrix() * viewMatrix * ro.transform.getModelMatrix();
    }
}

void FogApplication::render()
{
    glBindVertexArray(VAO);

    glUseProgram(shadowMapShader.getProgram());
    glViewport(0, 0, shadowMapWidth, shadowMapHeight);

    for (int i = 0; i < numLights; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBuffers[i]);
        glClear(GL_DEPTH_BUFFER_BIT);
        for (size_t j = 0; j < renderObjects.size(); ++j)
        {
            glUniformMatrix4fv(lightSpaceMatrixLocation, 1, 0, glm::value_ptr(lightSpaceMatrices[i]));
            glUniformMatrix4fv(modelMatrixLocation, 1, 0, glm::value_ptr(renderObjects[j].transform.getModelMatrix()));
            glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        }
    }

    glUseProgram(diffuseShader.getProgram());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1024, 768);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image.getTexture());

    glUniform3fv(lightDirectionsLocation, numLights, reinterpret_cast<GLfloat*>(lightDirections.data()));
    glUniform4fv(lightColorsLocation, numLights, reinterpret_cast<GLfloat*>(lightColors.data()));

    for (RenderObject& ro : renderObjects)
    {
        glUniformMatrix4fv(mvpMatrixLocation, 1, 0, glm::value_ptr(ro.mvpMatrix));
        glUniformMatrix4fv(modelMatrixLocation, 1, 0, glm::value_ptr(ro.transform.getModelMatrix()));
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    }

    glUseProgram(simpleShader.getProgram());

    for (size_t i = 0; i < lightRenderObjects.size(); ++i)
    {
        glUniformMatrix4fv(mvpMatrixLocation, 1, 0, glm::value_ptr(lightRenderObjects[i].mvpMatrix));
        glUniform3f(lightColorLocation, lightColors[i].r, lightColors[i].g, lightColors[i].b);
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    }
}

void FogApplication::gui()
{
    fw::displayFps();
    fw::displayVec3("Position %.1f %.1f %.1f", camera.getTransformation().position);
    fw::displayVec3("Rotation %.1f %.1f %.1f", camera.getTransformation().rotation);
}

void FogApplication::createBuffers(const fw::Model& model)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    std::size_t floatSize = sizeof(float);
    GLsizeiptr vertexDataSize = floatSize * 8 * model.getNumVertices();
    numIndices = model.getNumIndices();
    GLsizeiptr indexDataSize = sizeof(unsigned int) * numIndices;

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferStorage(GL_ARRAY_BUFFER, vertexDataSize, 0, GL_MAP_WRITE_BIT);
    float* vertexData = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, vertexDataSize, GL_MAP_WRITE_BIT);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, 0, GL_MAP_WRITE_BIT);
    unsigned int* elementData = (unsigned int*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, indexDataSize, GL_MAP_WRITE_BIT);

    mapBufferData(model, vertexData, elementData);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    GLsizei stride = static_cast<GLsizei>(8 * floatSize);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * floatSize));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(5 * floatSize));
    glEnableVertexAttribArray(2);
}

void FogApplication::createShadowMaps()
{
    shadowMapBuffers.resize(numLights);
    shadowMapTextures.resize(numLights);

    glGenFramebuffers(numLights, shadowMapBuffers.data());
    glGenTextures(numLights, shadowMapTextures.data());

    for (int i = 0; i < numLights; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, shadowMapTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBuffers[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTextures[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
}

void FogApplication::createScene()
{
    renderObjects.resize(11);
    fw::Transformation t;

    float leftWallX = -6.0f;
    float rightWallX = 6.0f;
    float floorY = -2.0f;
    float lowerWallY = -1.0f;
    float middleWallY = 1.5f;
    float upperWallY = 4.0f;
    float wallHeight = 1.5;
    float middleWallHeight = 1.0;
    float thickness = 0.1f;
    float depth = 20.0f;
    float middleWallDepth = 3.0f;

    t.position = glm::vec3(0.0, floorY, 0.0f);
    t.scale = glm::vec3(6.0f, thickness, depth);
    renderObjects[0].transform = t; // Floor

    t.position = glm::vec3(leftWallX, lowerWallY, 0.0f);
    t.scale = glm::vec3(thickness, wallHeight, depth);
    renderObjects[1].transform = t; // Left lower wall

    t.position = glm::vec3(rightWallX, lowerWallY, 0.0f);
    t.scale = glm::vec3(thickness, wallHeight, depth);
    renderObjects[2].transform = t; // Right lower wall

    t.position = glm::vec3(leftWallX, upperWallY, 0.0f);
    t.scale = glm::vec3(thickness, wallHeight, depth);
    renderObjects[3].transform = t; // Left upper wall

    t.position = glm::vec3(rightWallX, upperWallY, 0.0f);
    t.scale = glm::vec3(thickness, wallHeight, depth);
    renderObjects[4].transform = t; // Right upper wall

    t.position = glm::vec3(leftWallX, middleWallY, -10.0f);
    t.scale = glm::vec3(thickness, middleWallHeight, middleWallDepth);
    renderObjects[5].transform = t; // Left middle wall

    t.position = glm::vec3(leftWallX, middleWallY, 0.0f);
    t.scale = glm::vec3(thickness, middleWallHeight, middleWallDepth);
    renderObjects[6].transform = t; // Left middle wall

    t.position = glm::vec3(leftWallX, middleWallY, 10.0f);
    t.scale = glm::vec3(thickness, middleWallHeight, middleWallDepth);
    renderObjects[7].transform = t; // Left middle wall

    t.position = glm::vec3(rightWallX, middleWallY, -15.0f);
    t.scale = glm::vec3(thickness, middleWallHeight, middleWallDepth);
    renderObjects[8].transform = t; // Right middle wall

    t.position = glm::vec3(rightWallX, middleWallY, 0.0f);
    t.scale = glm::vec3(thickness, middleWallHeight, middleWallDepth);
    renderObjects[9].transform = t; // Right middle wall

    t.position = glm::vec3(rightWallX, middleWallY, 15.0f);
    t.scale = glm::vec3(thickness, middleWallHeight, middleWallDepth);
    renderObjects[10].transform = t; // Right middle wall

    for (RenderObject& ro : renderObjects)
    {
        ro.transform.updateModelMatrix();
    }

    lightPositions = {
        {-7.0f, 4.0f, 0.0f},
        {7.0f, 4.0f, 0.0f}};

    lightDirections = {
        glm::normalize(glm::vec3(1.0, -1.0f, 0.0f)),
        glm::normalize(glm::vec3(-1.0, -1.0f, 0.0f))};

    lightColors = {
        {1.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f}};

    lightSpaceMatrices.resize(numLights);
    lightRenderObjects.resize(numLights);

    for (int i = 0; i < numLights; ++i)
    {
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
        glm::mat4 lightView = glm::lookAt(lightPositions[i], lightPositions[i] + lightDirections[i], glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrices[i] = lightProjection * lightView;

        fw::Transformation t;
        t.position = lightPositions[i];
        t.scale = glm::vec3(0.1f);
        lightRenderObjects[i].transform = t;
        lightRenderObjects[i].transform.updateModelMatrix();
    }
}
