#include "FogApplication.h"

#include <fw/Framework.h>
#include <fw/Input.h>
#include <fw/Common.h>

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

namespace
{
const GLsizei numLights = 2;
const int shadowMapWidth = 1024;
const int shadowMapHeight = 1024;
const int densityDepth = 128;
const int densityBufferSize = 160 * 90 * densityDepth * sizeof(float) * 4;
const int width = 1024;
const int height = 768;

float hfov = 72.6f;
float vfov = 45.0f;
} // namespace

FogApplication::FogApplication() :
    camera(glm::radians(vfov), 16.0f / 9.0f, 0.5f, 50.0f)
{
}

FogApplication::~FogApplication()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteTextures(1, &densityTexture);
    glDeleteTextures(1, &cumulatveDensityTexture);
    glDeleteTextures(1, &depthTexture);
    glDeleteTextures(1, &framebufferTexture);
    glDeleteRenderbuffers(1, &renderbuffer);
    glDeleteFramebuffers(1, &depthBuffer);
    glDeleteFramebuffers(1, &framebuffer);
}

bool FogApplication::initialize()
{
    fw::printSystemInfo();

    glm::vec3 p = glm::vec3(0.0f, 2.0f, 20.0f);
    camera.getTransformation().position = p;
    cameraController.setCamera(&camera);
    cameraController.setResetMode(p, glm::vec3(0.0f, 0.0f, 0.0f), SDLK_r);
    cameraController.setMovementSpeed(3.0f);

    std::string path = std::string(ROOT_PATH) + "Experiments/VolumetricFog/shaders/diffuse";
    bool status = true;

    status = diffuseShader.createProgram({path + ".vert", path + ".frag"});
    assert(status);
    shadowMapLocations.resize(numLights);
    shadowMapLocations[0] = glGetUniformLocation(diffuseShader.getProgram(), "shadowMaps[0]");
    shadowMapLocations[1] = glGetUniformLocation(diffuseShader.getProgram(), "shadowMaps[1]");

    path = std::string(ROOT_PATH) + "Experiments/VolumetricFog/shaders/shadowMap";
    status = shadowMapShader.createProgram({path + ".vert", path + ".frag"});
    assert(status);

    path = std::string(ROOT_PATH) + "Experiments/VolumetricFog/shaders/simple";
    status = simpleShader.createProgram({path + ".vert", path + ".frag"});
    assert(status);

    path = std::string(ROOT_PATH) + "Experiments/VolumetricFog/shaders/density";
    status = densityShader.createProgram({path + ".comp"});
    assert(status);

    path = std::string(ROOT_PATH) + "Experiments/VolumetricFog/shaders/cumulativeDensity";
    status = cumulativeDensityShader.createProgram({path + ".comp"});
    assert(status);

    path = std::string(ROOT_PATH) + "Experiments/VolumetricFog/shaders/fog";
    status = fogShader.createProgram({path + ".vert", path + ".frag"});
    assert(status);

    fw::Model model;
    std::string modelFile = std::string(ASSETS_PATH) + "Models/cube.obj";
    status = model.loadModel(modelFile);
    assert(status);

    std::string textureFile = std::string(ASSETS_PATH) + "Textures/checker.png";
    status = image.load(textureFile);
    assert(status);
    image.create2dTexture();

    createVertexBuffers(model);
    createShadowMaps();
    createDensityTextures();
    createDepthBuffers();
    createFramebuffer();
    createScene();

    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    return true;
}

void FogApplication::update()
{
    if (fw::Input::isKeyReleased(SDLK_f))
    {
        fogScattering += 0.05f;
    }
    if (fw::Input::isKeyReleased(SDLK_g))
    {
        fogScattering -= 0.05f;
        fogScattering = std::max(0.0f, fogScattering);
    }

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

    // Render shadow maps
    glUseProgram(shadowMapShader.getProgram());
    glViewport(0, 0, shadowMapWidth, shadowMapHeight);

    for (int i = 0; i < numLights; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBuffers[i]);
        glClear(GL_DEPTH_BUFFER_BIT);
        for (size_t j = 0; j < renderObjects.size(); ++j)
        {
            glUniformMatrix4fv(1, 1, 0, glm::value_ptr(renderObjects[j].transform.getModelMatrix()));
            glUniformMatrix4fv(2, 1, 0, glm::value_ptr(lightSpaceMatrices[i]));
            glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        }
    }

    // Render depth map
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glm::mat4 vp = camera.getProjectionMatrix() * camera.getViewMatrix();

    for (size_t j = 0; j < renderObjects.size(); ++j)
    {
        glUniformMatrix4fv(1, 1, 0, glm::value_ptr(renderObjects[j].transform.getModelMatrix()));
        glUniformMatrix4fv(2, 1, 0, glm::value_ptr(vp));
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    }

    // Calculate density
    glUseProgram(densityShader.getProgram());
    glBindImageTexture(0, densityTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glUniformMatrix4fv(1, 1, 0, glm::value_ptr(glm::inverse(camera.getViewMatrix())));
    glUniformMatrix4fv(2, 2, 0, glm::value_ptr(*lightSpaceMatrices.data()));
    glUniform1f(4, camera.getNearClipDistance());
    glUniform1f(5, camera.getFarClipDistance());
    glUniform1f(6, hfov);
    glUniform1f(7, vfov);
    glUniform1f(10, fogScattering);
    glUniform1f(11, fw::Framework::getTimeSinceStart());

    for (size_t i = 0; i < shadowMapTextures.size(); ++i)
    {
        GLint gli = static_cast<GLint>(i);
        glActiveTexture(GL_TEXTURE0 + gli);
        glBindTexture(GL_TEXTURE_2D, shadowMapTextures[i]);
        glUniform1i(8 + gli, gli);
    }

    glUniform4fv(14, numLights, reinterpret_cast<GLfloat*>(lightColors.data()));

    glDispatchCompute(5, 5, densityDepth);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Calculate cumulative density
    glUseProgram(cumulativeDensityShader.getProgram());
    glBindImageTexture(0, densityTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, cumulatveDensityTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glDispatchCompute(5, 5, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Render diffuse lighting with shadows
    glUseProgram(diffuseShader.getProgram());

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image.getTexture());

    for (size_t i = 0; i < shadowMapTextures.size(); ++i)
    {
        GLint gli = static_cast<GLint>(i);
        glActiveTexture(GL_TEXTURE1 + gli);
        glBindTexture(GL_TEXTURE_2D, shadowMapTextures[i]);
        glUniform1i(shadowMapLocations[i], 1 + gli);
    }

    glUniform3fv(10, numLights, reinterpret_cast<GLfloat*>(lightDirections.data()));
    glUniform4fv(14, numLights, reinterpret_cast<GLfloat*>(lightColors.data()));

    for (RenderObject& ro : renderObjects)
    {
        glUniformMatrix4fv(0, 1, 0, glm::value_ptr(ro.mvpMatrix));
        glUniformMatrix4fv(1, 1, 0, glm::value_ptr(ro.transform.getModelMatrix()));
        glUniformMatrix4fv(2, 2, 0, glm::value_ptr(*lightSpaceMatrices.data()));
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    }

    // Render debug lights
    glUseProgram(simpleShader.getProgram());

    for (size_t i = 0; i < lightRenderObjects.size(); ++i)
    {
        glUniformMatrix4fv(0, 1, 0, glm::value_ptr(lightRenderObjects[i].mvpMatrix));
        glUniform3f(2, lightColors[i].r, lightColors[i].g, lightColors[i].b);
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    }

    glUseProgram(simpleShader.getProgram());

    for (size_t i = 0; i < lightRenderObjects.size(); ++i)
    {
        glUniformMatrix4fv(0, 1, 0, glm::value_ptr(lightRenderObjects[i].mvpMatrix));
        glUniform3f(2, lightColors[i].r, lightColors[i].g, lightColors[i].b);
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    }

    // Render fullscreen triangle
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glUseProgram(fogShader.getProgram());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, cumulatveDensityTexture);

    glm::mat4 inverseProj = glm::inverse(camera.getProjectionMatrix());
    glUniformMatrix4fv(1, 1, 0, glm::value_ptr(inverseProj));
    glUniform1f(2, camera.getFarClipDistance());

    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Blit
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glDisable(GL_BLEND);
}

void FogApplication::gui()
{
    fw::displayFps();
    fw::displayVec3("Position %.1f %.1f %.1f", camera.getTransformation().position);
    fw::displayVec3("Rotation %.1f %.1f %.1f", camera.getTransformation().rotation);
}

void FogApplication::createVertexBuffers(const fw::Model& model)
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
        float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBuffers[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTextures[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
}

void FogApplication::createDensityTextures()
{
    glGenTextures(1, &densityTexture);
    glBindTexture(GL_TEXTURE_3D, densityTexture);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, 160, 90, densityDepth);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &cumulatveDensityTexture);
    glBindTexture(GL_TEXTURE_3D, cumulatveDensityTexture);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, 160, 90, densityDepth);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
    glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void FogApplication::createDepthBuffers()
{
    glGenFramebuffers(1, &depthBuffer);
    glGenTextures(1, &depthTexture);

    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
}

void FogApplication::createFramebuffer()
{
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &framebufferTexture);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void FogApplication::createScene()
{
    renderObjects.resize(13);
    fw::Transformation t;

    float leftWallX = -6.0f;
    float rightWallX = 6.0f;
    float floorY = -2.0f;
    float lowerWallY = -1.0f;
    float middleWallY = 1.5f;
    float upperWallY = 6.0f;
    float wallHeight = 1.5;
    float middleWallHeight = 1.0;
    float upperWallHeight = 3.5;
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
    t.scale = glm::vec3(thickness, upperWallHeight, depth);
    renderObjects[3].transform = t; // Left upper wall

    t.position = glm::vec3(rightWallX, upperWallY, 0.0f);
    t.scale = glm::vec3(thickness, upperWallHeight, depth);
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

    t.position = glm::vec3(0.0, 8.0f, 0.0f);
    t.scale = glm::vec3(6.0f, thickness, depth);
    renderObjects[11].transform = t; // Floor

    t.position = glm::vec3(0.0, 0.0f, -20.5f);
    t.scale = glm::vec3(10.0f, 10.0f, 1.0f);
    renderObjects[12].transform = t; // Back wall

    for (RenderObject& ro : renderObjects)
    {
        ro.transform.updateModelMatrix();
    }

    lightPositions = {
        {-15.0f, 2.0f, 0.0f},
        {15.0f, 2.0f, 0.0f}};

    lightDirections = {
        glm::normalize(glm::vec3(1.0, -1.0f, 0.0f)),
        glm::normalize(glm::vec3(-1.0, -1.0f, 0.0f))};

    lightColors = {
        {1.0f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 1.0f, 1.0f}};

    lightSpaceMatrices.resize(numLights);
    lightRenderObjects.resize(numLights);

    for (int i = 0; i < numLights; ++i)
    {
        glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 0.1f, 20.0f);
        glm::mat4 lightView = glm::lookAt(lightPositions[i], lightPositions[i] + lightDirections[i], glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrices[i] = lightProjection * lightView;

        fw::Transformation t;
        t.position = lightPositions[i];
        t.scale = glm::vec3(0.1f);
        lightRenderObjects[i].transform = t;
        lightRenderObjects[i].transform.updateModelMatrix();
    }
}
