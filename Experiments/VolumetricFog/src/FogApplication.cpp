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
const GLint modelMatrixLocation = 1;
const GLint lightPositionsLocation = 10;
const GLint lightColorsLocation = 14;
const GLsizei numLights = 4;
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

    std::string path = std::string(ROOT_PATH) + "Experiments/VolumetricFog/shaders/simple";
    if (!shader.createProgram({path + ".vert", path + ".frag"}))
    {
        return false;
    }
    std::cout << "Loaded shader " << path << " (" << shader.getProgram() << ")\n";

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
}

void FogApplication::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader.getProgram());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image.getTexture());

    glBindVertexArray(VAO);
    glUniform3fv(lightPositionsLocation, numLights, reinterpret_cast<GLfloat*>(lightPositions.data()));
    glUniform4fv(lightColorsLocation, numLights, reinterpret_cast<GLfloat*>(lightColors.data()));

    for (RenderObject& ro : renderObjects)
    {
        glUniformMatrix4fv(mvpMatrixLocation, 1, 0, glm::value_ptr(ro.mvpMatrix));
        glUniformMatrix4fv(modelMatrixLocation, 1, 0, glm::value_ptr(ro.transform.getModelMatrix()));
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
        {-7.0f, 1.0f, -10.0f},
        {-7.0f, 1.0f, 5.0f},
        {7.0f, 1.0f, -10.0f},
        {7.0f, 1.0f, 5.0f}};

    lightColors = {
        {1.0f, 0.0f, 0.0f, 5.0f},
        {0.0f, 1.0f, 0.0f, 5.0f},
        {0.0f, 0.0f, 1.0f, 5.0f},
        {1.0f, 0.0f, 1.0f, 5.0f}};
}
