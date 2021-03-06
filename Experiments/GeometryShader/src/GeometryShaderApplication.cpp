#include "GeometryShaderApplication.h"

#include <fw/Framework.h>
#include <fw/Input.h>
#include <fw/Common.h>

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace
{
const GLuint projectionMatrixLocation = 0;
const GLuint modelViewMatrixLocation = 1;
const GLuint modelMatrixLocation = 2;
const GLuint timeLocation = 3;

} // namespace

GeometryShaderApplication::GeometryShaderApplication()
{
}

GeometryShaderApplication::~GeometryShaderApplication()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
}

bool GeometryShaderApplication::initialize()
{
    cameraController.setCamera(&camera);
    cameraController.setResetMode(glm::vec3(-1.0f, 1.3f, -2.2f), glm::vec3(-0.5f, -2.6f, 0.0f), SDLK_r);

    std::string path = std::string(ROOT_PATH) + "Experiments/GeometryShader/shaders/explosion";
    if (!shader.createProgram({path + ".vert", path + ".geom", path + ".frag"}))
    {
        return false;
    }
    std::cout << "Loaded shader " << path << " (" << shader.getProgram() << ")\n";

    fw::Model model;
    std::string modelFile = std::string(ASSETS_PATH) + "/Models/monkey.3ds";
    if (!model.loadModel(modelFile))
    {
        return false;
    }
    std::cout << "Loaded model " << modelFile << "\n";

    std::string textureFile = std::string(ASSETS_PATH) + "/Textures/green_square.png";
    if (!image.load(textureFile))
    {
        return false;
    }
    image.create2dTexture();
    std::cout << "Loaded texture " << textureFile << " (" << image.getTexture() << ")\n";

    createBuffer(model);
    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.updateModelMatrix();

    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    return true;
}

void GeometryShaderApplication::update()
{
    cameraController.update();
    fw::toggleRelativeMouseMode();

    const glm::mat4& viewMatrix = camera.updateViewMatrix();
    modelViewMatrix = viewMatrix * transform.getModelMatrix();
}

void GeometryShaderApplication::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader.getProgram());

    glUniformMatrix4fv(projectionMatrixLocation, 1, 0, glm::value_ptr(camera.getProjectionMatrix()));
    glUniformMatrix4fv(modelViewMatrixLocation, 1, 0, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(modelMatrixLocation, 1, 0, glm::value_ptr(transform.getModelMatrix()));
    glUniform1f(timeLocation, fw::Framework::getTimeSinceStart());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image.getTexture());
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

void GeometryShaderApplication::gui()
{
    fw::displayFps();
    fw::displayVec3("Position %.1f %.1f %.1f", camera.getTransformation().position);
    fw::displayVec3("Rotation %.1f %.1f %.1f", camera.getTransformation().rotation);
}

void GeometryShaderApplication::createBuffer(const fw::Model& model)
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
