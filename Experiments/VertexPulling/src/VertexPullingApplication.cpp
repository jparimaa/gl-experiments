#include "VertexPullingApplication.h"

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
const GLint modelMatrixLocation = 0;
const GLint mvpMatrixLocation = 1;

} // namespace

VertexPullingApplication::VertexPullingApplication()
{
}

VertexPullingApplication::~VertexPullingApplication()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vnBuffer);
    glDeleteBuffers(1, &uvBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteTextures(1, &vnTexBuffer);
    glDeleteTextures(1, &uvTexBuffer);
}

bool VertexPullingApplication::initialize()
{
    fw::printSystemInfo();

    cameraController.setCamera(&camera);
    cameraController.setResetMode(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 3.14f, 0.0f), SDLK_r);

    std::string path = std::string(ROOT_PATH) + "Experiments/VertexPulling/shaders/simple";
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
    image.create2dTexture();
    std::cout << "Loaded texture " << textureFile << " (" << image.getTexture() << ")\n";

    createBuffers(model);

    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    return true;
}

void VertexPullingApplication::update()
{
    fw::toggleRelativeMouseMode();

    cameraController.update();

    objTransformation.rotate(glm::vec3(0.0f, 1.0f, 0.0f), fw::Framework::getFrameTime() * 0.7f);
    objTransformation.updateModelMatrix();

    const glm::mat4& viewMatrix = camera.updateViewMatrix();
    mvpMatrix = camera.getProjectionMatrix() * viewMatrix * objTransformation.getModelMatrix();
}

void VertexPullingApplication::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader.getProgram());

    glUniformMatrix4fv(modelMatrixLocation, 1, 0, glm::value_ptr(objTransformation.getModelMatrix()));
    glUniformMatrix4fv(mvpMatrixLocation, 1, 0, glm::value_ptr(mvpMatrix));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image.getTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, vnTexBuffer);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_BUFFER, uvTexBuffer);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

void VertexPullingApplication::gui()
{
    fw::displayFps();
    fw::displayVec3("Position %.1f %.1f %.1f", camera.getTransformation().position);
    fw::displayVec3("Rotation %.1f %.1f %.1f", camera.getTransformation().rotation);
}

void VertexPullingApplication::createBuffers(const fw::Model& model)
{
    std::vector<float> vertexAndNormal;
    std::vector<float> uv;
    std::vector<unsigned int> indices;
    unsigned int indexOffset = 0;
    for (const auto& mesh : model.getMeshes())
    {
        for (unsigned int i = 0; i < mesh.vertices.size(); ++i)
        {
            vertexAndNormal.push_back(mesh.vertices[i].x);
            vertexAndNormal.push_back(mesh.vertices[i].y);
            vertexAndNormal.push_back(mesh.vertices[i].z);
            vertexAndNormal.push_back(mesh.normals[i].x);
            vertexAndNormal.push_back(mesh.normals[i].y);
            vertexAndNormal.push_back(mesh.normals[i].z);
            uv.push_back(mesh.uvs[i].x);
            uv.push_back(mesh.uvs[i].y);
        }
        for (const auto& index : mesh.indices)
        {
            indices.push_back(index + indexOffset);
        }
        indexOffset += static_cast<unsigned int>(mesh.vertices.size());
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLsizeiptr vertexDataSize = sizeof(float) * 6 * model.getNumVertices();
    glGenBuffers(1, &vnBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vnBuffer);
    glBufferStorage(GL_ARRAY_BUFFER, vertexDataSize, vertexAndNormal.data(), 0);

    GLsizeiptr uvDataSize = sizeof(float) * 3 * model.getNumVertices();
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferStorage(GL_ARRAY_BUFFER, uvDataSize, uv.data(), 0);

    numIndices = model.getNumIndices();
    GLsizeiptr indexDataSize = sizeof(unsigned int) * numIndices;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indices.data(), 0);

    glGenTextures(1, &vnTexBuffer);
    glBindTexture(GL_TEXTURE_BUFFER, vnTexBuffer);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, vnBuffer);

    glGenTextures(1, &uvTexBuffer);
    glBindTexture(GL_TEXTURE_BUFFER, uvTexBuffer);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RG32F, uvBuffer);
    // Could also add indices to tex buffer and render with drawArrays and fetch the index with vertexID
}
