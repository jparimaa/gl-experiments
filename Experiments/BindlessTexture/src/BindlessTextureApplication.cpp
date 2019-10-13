#include "BindlessTextureApplication.h"
#include <fw/Framework.h>
#include <fw/Common.h>
#include <iostream>

namespace
{
const int square = 10;
const int numQuads = square * square;
const GLuint textureBufferIndex = 0;

}; // namespace

BindlessTextureApplication::BindlessTextureApplication()
{
}

BindlessTextureApplication::~BindlessTextureApplication()
{
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &textureBuffer);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
}

bool BindlessTextureApplication::initialize()
{
    std::string arbExt = "GL_ARB_bindless_texture";
    bool hasArbExt = fw::hasExtension(arbExt);
    if (!hasArbExt)
    {
        std::cerr << "Extension " << arbExt << " is not available.\n";
        return false;
    }

    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Shader
    std::string shaderPath = std::string(ROOT_PATH) + "Experiments/BindlessTexture/shaders/bindless";
    std::vector<std::string> shaderFiles = {
        shaderPath + ".vert",
        shaderPath + ".frag"};
    if (!shader.createProgram(shaderFiles))
    {
        return false;
    }
    std::cout << "Loaded shader " << shaderPath << " (" << shader.getProgram() << ")\n";

    createTextures();
    createVertexBuffer();

    return true;
}

void BindlessTextureApplication::update()
{
}

void BindlessTextureApplication::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader.getProgram());
    glBindVertexArray(VAO);
    for (int i = 0; i < numQuads; ++i)
    {
        glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, i * 4);
    }
}

void BindlessTextureApplication::gui()
{
}

void BindlessTextureApplication::createTextures()
{
    glUseProgram(shader.getProgram());
    glGenTextures(1, &texture);
    GLenum target = GL_TEXTURE_2D;

    glBindTexture(target, texture);
    glTexStorage2D(target, 1, GL_RGBA8, 1, 1);
    GLubyte data[3] = {fw::getRandomColor(), fw::getRandomColor(), fw::getRandomColor()};
    glTexSubImage2D(target, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(target);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    handle = glGetTextureHandleARB(texture);
    glMakeTextureHandleResidentARB(handle);

    glGenBuffers(1, &textureBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, textureBuffer);
    glBufferStorage(GL_UNIFORM_BUFFER, sizeof(handle), &handle, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, textureBufferIndex, textureBuffer);
}

void BindlessTextureApplication::createVertexBuffer()
{
    const int quadSize = 16;
    const float quad[quadSize] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 0.1f, 0.0f, 1.0f, 0.1f, 0.1f, 1.0f, 1.0f};

    const int vertexDataSize = quadSize * numQuads;
    float vertexData[vertexDataSize];

    for (int quadNum = 0; quadNum < numQuads; ++quadNum)
    {
        int quadStart = quadNum * quadSize;
        float xOffset = -0.99f + static_cast<float>(quadNum % square) * 0.11f;
        float yOffset = -0.99f + static_cast<float>(quadNum / square) * 0.11f;
        for (int vertex = 0; vertex < quadSize; vertex += 4)
        {
            int v = quadStart + vertex;
            vertexData[v + 0] = quad[vertex + 0] + xOffset;
            vertexData[v + 1] = quad[vertex + 1] + yOffset;
            vertexData[v + 2] = quad[vertex + 2];
            vertexData[v + 3] = quad[vertex + 3];
        }
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    GLsizei stride = 4 * sizeof(float);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    const unsigned int indices[] = {0, 1, 2, 1, 3, 2};
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}