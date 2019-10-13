#include "SimpleIndirectApplication.h"
#include <fw/Framework.h>
#include <iostream>

SimpleIndirectApplication::SimpleIndirectApplication()
{
}

SimpleIndirectApplication::~SimpleIndirectApplication()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &commandBuffer);
}

bool SimpleIndirectApplication::initialize()
{
    std::string shaderPath = std::string(ROOT_PATH) + "Experiments/IndirectDrawingSimple/shaders/simple";
    std::vector<std::string> shaderFiles = {
        shaderPath + ".vert",
        shaderPath + ".frag"};
    if (!shader.createProgram(shaderFiles))
    {
        return false;
    }
    std::cout << "Loaded shader " << shaderPath << " (" << shader.getProgram() << ")\n";

    createDataBuffers();

    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    return true;
}

void SimpleIndirectApplication::update()
{
}

void SimpleIndirectApplication::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader.getProgram());

    glClear(GL_COLOR_BUFFER_BIT);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, 2, 0);
}

void SimpleIndirectApplication::gui()
{
}

void SimpleIndirectApplication::createDataBuffers()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    const float quad[] = {
        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f};

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    const unsigned int indices[] = {0, 1, 2, 1, 3, 2};

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    IndirectCommand drawCommand[2];
    drawCommand[0].count = 3;
    drawCommand[0].instanceCount = 1;
    drawCommand[0].firstIndex = 0;
    drawCommand[0].baseVertex = 0;
    drawCommand[0].baseInstance = 0;

    drawCommand[1].count = 3;
    drawCommand[1].instanceCount = 1;
    drawCommand[1].firstIndex = 3;
    drawCommand[1].baseVertex = 0;
    drawCommand[1].baseInstance = 0;

    glGenBuffers(1, &commandBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(drawCommand), drawCommand, GL_STATIC_DRAW);
}
