#include "TextureProjectionApplication.h"

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
const GLint projectorMatrixLocation = 1;
} // namespace

TextureProjectionApplication::TextureProjectionApplication()
{
}

TextureProjectionApplication::~TextureProjectionApplication()
{
}

bool TextureProjectionApplication::initialize()
{
    fw::printSystemInfo();

    cameraController.setCamera(&camera);
    cameraController.setResetMode(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 3.14f, 0.0f), SDLK_r);

    std::string path = std::string(ROOT_PATH) + "Experiments/TextureProjection/shaders/projection";
    if (!shader.createProgram({path + ".vert", path + ".frag"}))
    {
        return false;
    }
    std::cout << "Loaded shader " << path << " (" << shader.getProgram() << ")\n";

    fw::Model cubeModel;
    std::string cubeModelFile = std::string(ASSETS_PATH) + "Models/cube.obj";
    if (!cubeModel.loadModel(cubeModelFile))
    {
        return false;
    }
    std::cout << "Loaded model " << cubeModelFile << "\n";

    std::string checkerTextureFile = std::string(ASSETS_PATH) + "Textures/checker.png";
    if (!checkerImage.load(checkerTextureFile))
    {
        return false;
    }
    checkerImage.create2dTexture();
    std::cout << "Loaded texture " << checkerTextureFile << " (" << checkerImage.getTexture() << ")\n";

    std::string greyTextureFile = std::string(ASSETS_PATH) + "Textures/green_square.png";
    if (!greyImage.load(greyTextureFile))
    {
        return false;
    }
    greyImage.create2dTexture();
    std::cout << "Loaded texture " << greyTextureFile << " (" << greyImage.getTexture() << ")\n";

    std::string projectionTextureFile = std::string(ASSETS_PATH) + "Textures/smile.jpg";
    if (!projectionImage.load(projectionTextureFile))
    {
        return false;
    }
    projectionImage.create2dTexture();
    std::cout << "Loaded texture " << projectionTextureFile << " (" << projectionImage.getTexture() << ")\n";

    createBuffers(cubeModel, cube);

    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    floorTrans.objTransformation.position = glm::vec3(0.0f, -0.5f, 0.0f);
    floorTrans.objTransformation.scale = glm::vec3(3.0f, 0.1f, 3.0f);
    floorTrans.objTransformation.updateModelMatrix();

    camera.getTransformation().position = glm::vec3(0.0f, 1.0f, 5.0f);
    camera.getTransformation().rotation = glm::vec3(-0.2f, 0.0f, 0.0f);

    projector.getTransformation().position = glm::vec3(5.0f, 5.0f, 5.0f);
    projector.getTransformation().rotation = glm::vec3(-0.67f, 0.77f, 0.0f);
    projector.updateViewMatrix();

    return true;
}

void TextureProjectionApplication::update()
{
    if (fw::Input::isKeyPressed(SDLK_p))
    {
        camera.getTransformation().position = projector.getTransformation().position;
        camera.getTransformation().rotation = projector.getTransformation().rotation;
    }

    fw::toggleRelativeMouseMode();

    cameraController.update();

    cubeTrans.objTransformation.rotate(glm::vec3(0.0f, 1.0f, 0.0f), fw::Framework::getFrameTime() * 0.7f);
    cubeTrans.objTransformation.updateModelMatrix();

    camera.updateViewMatrix();

    cubeTrans.mvpMatrix = camera.getProjectionMatrix() * camera.getViewMatrix() * cubeTrans.objTransformation.getModelMatrix();
    floorTrans.mvpMatrix = camera.getProjectionMatrix() * camera.getViewMatrix() * floorTrans.objTransformation.getModelMatrix();

    cubeTrans.projectionMatrix = projector.getProjectionMatrix() * projector.getViewMatrix() * cubeTrans.objTransformation.getModelMatrix();
    floorTrans.projectionMatrix = projector.getProjectionMatrix() * projector.getViewMatrix() * floorTrans.objTransformation.getModelMatrix();
}

void TextureProjectionApplication::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader.getProgram());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glUniformMatrix4fv(mvpMatrixLocation, 1, 0, glm::value_ptr(cubeTrans.mvpMatrix));
    glUniformMatrix4fv(projectorMatrixLocation, 1, 0, glm::value_ptr(cubeTrans.projectionMatrix));

    // Cube
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, greyImage.getTexture());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, projectionImage.getTexture());

    glBindVertexArray(cube.VAO);
    glDrawElements(GL_TRIANGLES, cube.numIndices, GL_UNSIGNED_INT, 0);

    // Floor
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, checkerImage.getTexture());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, projectionImage.getTexture());

    glUniformMatrix4fv(mvpMatrixLocation, 1, 0, glm::value_ptr(floorTrans.mvpMatrix));
    glUniformMatrix4fv(projectorMatrixLocation, 1, 0, glm::value_ptr(floorTrans.projectionMatrix));

    glBindVertexArray(cube.VAO);
    glDrawElements(GL_TRIANGLES, cube.numIndices, GL_UNSIGNED_INT, 0);
}

void TextureProjectionApplication::gui()
{
}

void TextureProjectionApplication::createBuffers(const fw::Model& model, RenderObject& renderObject)
{
    glGenVertexArrays(1, &renderObject.VAO);
    glBindVertexArray(renderObject.VAO);

    std::size_t floatSize = sizeof(float);
    GLsizeiptr vertexDataSize = floatSize * 8 * model.getNumVertices();
    renderObject.numIndices = model.getNumIndices();
    GLsizeiptr indexDataSize = sizeof(unsigned int) * renderObject.numIndices;

    glGenBuffers(1, &renderObject.vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, renderObject.vertexBuffer);
    glBufferStorage(GL_ARRAY_BUFFER, vertexDataSize, 0, GL_MAP_WRITE_BIT);
    float* vertexData = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, vertexDataSize, GL_MAP_WRITE_BIT);

    glGenBuffers(1, &renderObject.indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderObject.indexBuffer);
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

TextureProjectionApplication::RenderObject::~RenderObject()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
}
