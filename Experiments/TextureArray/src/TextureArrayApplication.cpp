#include "TextureArrayApplication.h"
#include <fw/Framework.h>
#include <iostream>
#include <cstdlib>

namespace
{
const int square = 10;
const int numQuads = square * square;
GLint layerLocation = 0;

}; // namespace

TextureArrayApplication::TextureArrayApplication()
{
}

TextureArrayApplication::~TextureArrayApplication()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteTextures(1, &textureArray);
}

bool TextureArrayApplication::initialize()
{
    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Shader
    std::string shaderPath = std::string(ROOT_PATH) + "Experiments/TextureArray/shaders/array";
    std::vector<std::string> shaderFiles = {
        shaderPath + ".vert",
        shaderPath + ".frag"};
    if (shader.createProgram(shaderFiles))
    {
        std::cout << "Loaded shader " << shaderPath << " (" << shader.getProgram() << ")\n";
    }
    else
    {
        std::cerr << "ERROR: Shader creation failed\n";
        return false;
    }

    createVertexBuffer();
    createTextures();

    return true;
}

void TextureArrayApplication::update()
{
}

void TextureArrayApplication::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader.getProgram());
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
    for (int i = 0; i < numQuads; ++i)
    {
        glUniform1i(layerLocation, i);
        glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, i * 4);
        // Could also use instaning and instace_id for layer
    }
}

void TextureArrayApplication::gui()
{
}

void TextureArrayApplication::createVertexBuffer()
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

void TextureArrayApplication::createTextures()
{
    glGenTextures(1, &textureArray);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, 1, 1, numQuads);

    auto randomColor = []() {
        return static_cast<GLubyte>(std::rand() / 255);
    };

    for (unsigned int i = 0; i < numQuads; ++i)
    {
        GLubyte color[3] = {randomColor(), randomColor(), randomColor()};
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);
    }

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
