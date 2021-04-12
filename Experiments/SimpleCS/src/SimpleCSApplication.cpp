#include "SimpleCSApplication.h"

#include <fw/Framework.h>
#include <fw/Input.h>
#include <fw/Common.h>

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

namespace
{
const int c_textureSize = 256;
} // namespace

SimpleCSApplication::SimpleCSApplication()
{
}

SimpleCSApplication::~SimpleCSApplication()
{
    glDeleteTextures(1, &m_texture);
}

bool SimpleCSApplication::initialize()
{
    setupGLState();
    createTexture();
    createShaders();

    return true;
}

void SimpleCSApplication::update()
{
}

void SimpleCSApplication::render()
{
    glUseProgram(m_csShader.getProgram());
    glBindImageTexture(0, m_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
    glUniform1f(0, fw::Framework::getTimeSinceStart());
    glDispatchCompute(c_textureSize / 8, c_textureSize / 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_fullscreenShader.getProgram());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void SimpleCSApplication::gui()
{
}

void SimpleCSApplication::setupGLState()
{
    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glDisable(GL_DEPTH_TEST);

    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
}

void SimpleCSApplication::createTexture()
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, c_textureSize, c_textureSize);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    const int totalSize = c_textureSize * c_textureSize * 4;
    std::vector<uint8_t> data(totalSize);
    for (size_t i = 0; i < totalSize; i += 4)
    {
        data[i + 0] = i % 255;
        data[i + 1] = 255 - (i % 255);
        data[i + 2] = 128 + (i % 127);
        data[i + 3] = 255;
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, c_textureSize, c_textureSize, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
}

void SimpleCSApplication::createShaders()
{
    std::string path = std::string(ROOT_PATH) + "Experiments/SimpleCS/shaders/fullscreen";
    bool status = m_fullscreenShader.createProgram({path + ".vert", path + ".frag"});
    assert(status);

    path = std::string(ROOT_PATH) + "Experiments/SimpleCS/shaders/simple";
    status = m_csShader.createProgram({path + ".comp"});
    assert(status);
}
