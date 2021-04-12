#pragma once

#include <fw/Application.h>
#include <fw/Shader.h>

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <vector>

class SimpleCSApplication : public fw::Application
{
public:
    explicit SimpleCSApplication();
    virtual ~SimpleCSApplication();

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    fw::Shader m_fullscreenShader;
    fw::Shader m_csShader;

    GLuint m_texture = 0;

    void setupGLState();
    void createTexture();
    void createShaders();
};
