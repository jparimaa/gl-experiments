#include "DSAApplication.h"

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
const GLuint vertexBufferBindingIndex = 0;
const GLuint positionAttribute = 0;
const GLuint uvAttribute = 1;
const GLuint normalAttribute = 2;
const GLint uniformBufferBinding = 0;
const GLint textureBinding = 1;
const GLint timeLocation = 0;
const GLsizeiptr matrixBufferSize = 2 * sizeof(glm::mat4);
GLfloat clearColor[4] = {0.0f, 0.0f, 0.3f, 0.0f};
GLfloat clearDepth = 1.0f;

} // namespace

DSAApplication::DSAApplication()
{
}

DSAApplication::~DSAApplication()
{
    glDeleteTextures(1, &texture);
    glDeleteSamplers(1, &sampler);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &uniformBuffer);
}

bool DSAApplication::initialize()
{
    fw::printSystemInfo();

    cameraController.setCamera(&camera);
    cameraController.setResetMode(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 3.14f, 0.0f), SDLK_r);

    std::string path = std::string(ROOT_PATH) + "Experiments/DSA/shaders/simple";
    if (!shader.createProgram({path + ".vert", path + ".frag"}))
    {
        return false;
    }
    std::cout << "Loaded shader " << path << " (" << shader.getProgram() << ")\n";

    fw::Model model;
    std::string modelFile = std::string(ASSETS_PATH) + "Models/monkey.3ds";
    if (!model.loadModel(modelFile))
    {
        return false;
    }
    std::cout << "Loaded model " << modelFile << "\n";

    std::string textureFile = std::string(ASSETS_PATH) + "Textures/green_square.png";
    if (!image.load(textureFile))
    {
        return false;
    }
    std::cout << "Loaded image " << textureFile << "\n";

    createTexture(image);
    createSampler();
    createBuffers(model);

    glEnable(GL_DEPTH_TEST);

    return true;
}

void DSAApplication::update()
{
    fw::toggleRelativeMouseMode();

    cameraController.update();

    objTransformation.rotate(glm::vec3(0.0f, 1.0f, 0.0f), fw::Framework::getFrameTime() * 0.7f);
    objTransformation.updateModelMatrix();

    const glm::mat4& viewMatrix = camera.updateViewMatrix();
    mvpMatrix = camera.getProjectionMatrix() * viewMatrix * objTransformation.getModelMatrix();
}

void DSAApplication::render()
{
    glClearNamedFramebufferfv(0, GL_COLOR, 0, clearColor);
    glClearNamedFramebufferfv(0, GL_DEPTH, 0, &clearDepth);
    glUseProgram(shader.getProgram());

    glm::mat4* data = (glm::mat4*)glMapNamedBufferRange(uniformBuffer, 0, matrixBufferSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    data[0] = objTransformation.getModelMatrix();
    data[1] = mvpMatrix;
    glUnmapNamedBuffer(uniformBuffer);
    glBindBufferRange(GL_UNIFORM_BUFFER, uniformBufferBinding, uniformBuffer, 0, matrixBufferSize);

    glProgramUniform1f(shader.getProgram(), timeLocation, fw::Framework::getTimeSinceStart());

    glBindSamplers(textureBinding, 1, &sampler);
    glBindTextureUnit(textureBinding, texture);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

void DSAApplication::gui()
{
    fw::displayFps();
    fw::displayVec3("Position %.1f %.1f %.1f", camera.getTransformation().position);
    fw::displayVec3("Rotation %.1f %.1f %.1f", camera.getTransformation().rotation);
}

void DSAApplication::createTexture(const fw::Image& image)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    GLsizei numMipmaps = static_cast<GLsizei>(std::floor(log2(static_cast<double>(std::max(image.getWidth(), image.getHeight())))));
    glTextureStorage2D(texture, numMipmaps, GL_RGBA8, image.getWidth(), image.getHeight());
    glTextureSubImage2D(texture, 0, 0, 0, image.getWidth(), image.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, image.getData());
    glGenerateTextureMipmap(texture);
}

void DSAApplication::createSampler()
{
    glCreateSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

void DSAApplication::createBuffers(const fw::Model& model)
{
    std::vector<float> vertexData;
    std::vector<unsigned int> indices;
    unsigned int indexOffset = 0;
    for (const auto& mesh : model.getMeshes())
    {
        for (unsigned int i = 0; i < mesh.vertices.size(); ++i)
        {
            vertexData.push_back(mesh.vertices[i].x);
            vertexData.push_back(mesh.vertices[i].y);
            vertexData.push_back(mesh.vertices[i].z);
            vertexData.push_back(mesh.uvs[i].x);
            vertexData.push_back(mesh.uvs[i].y);
            vertexData.push_back(mesh.normals[i].x);
            vertexData.push_back(mesh.normals[i].y);
            vertexData.push_back(mesh.normals[i].z);
        }
        for (const auto& index : mesh.indices)
        {
            indices.push_back(index + indexOffset);
        }
        indexOffset += static_cast<unsigned int>(mesh.vertices.size());
    }

    glCreateBuffers(1, &vertexBuffer);
    GLsizeiptr vertexDataSize = sizeof(float) * 8 * model.getNumVertices();
    glNamedBufferStorage(vertexBuffer, vertexDataSize, vertexData.data(), 0);

    glCreateBuffers(1, &indexBuffer);
    numIndices = model.getNumIndices();
    GLsizeiptr indexDataSize = sizeof(unsigned int) * numIndices;
    glNamedBufferStorage(indexBuffer, indexDataSize, indices.data(), 0);

    glCreateBuffers(1, &uniformBuffer);
    glNamedBufferStorage(uniformBuffer, matrixBufferSize, nullptr, GL_MAP_WRITE_BIT);

    glCreateVertexArrays(1, &VAO);

    glVertexArrayAttribBinding(VAO, positionAttribute, vertexBufferBindingIndex);
    glVertexArrayAttribFormat(VAO, positionAttribute, 3, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(VAO, positionAttribute);

    glVertexArrayAttribBinding(VAO, uvAttribute, vertexBufferBindingIndex);
    glVertexArrayAttribFormat(VAO, uvAttribute, 2, GL_FLOAT, GL_FALSE, (GLuint)(3 * sizeof(float)));
    glEnableVertexArrayAttrib(VAO, uvAttribute);

    glVertexArrayAttribBinding(VAO, normalAttribute, vertexBufferBindingIndex);
    glVertexArrayAttribFormat(VAO, normalAttribute, 3, GL_FLOAT, GL_FALSE, (GLuint)(5 * sizeof(float)));
    glEnableVertexArrayAttrib(VAO, normalAttribute);

    glVertexArrayVertexBuffer(VAO, vertexBufferBindingIndex, vertexBuffer, 0, (GLuint)(8 * sizeof(float)));
    glVertexArrayElementBuffer(VAO, indexBuffer);
}
