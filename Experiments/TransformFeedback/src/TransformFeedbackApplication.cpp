#include "TransformFeedbackApplication.h"

#include <fw/Framework.h>
#include <fw/Common.h>

#include <imgui/imgui.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

namespace
{
GLuint positionIndex = 0;
GLuint timeLocation = 0;

} // namespace

TransformFeedbackApplication::TransformFeedbackApplication()
{
}

TransformFeedbackApplication::~TransformFeedbackApplication()
{
    glDeleteProgram(transformProgram);
    glDeleteVertexArrays(1, &transformVAO);
    glDeleteBuffers(1, &transformBuffer);

    glDeleteTransformFeedbacks(1, &transformFeedback);

    glDeleteVertexArrays(1, &feedbackVAO);
    glDeleteBuffers(1, &feedbackBuffer);
}

bool TransformFeedbackApplication::initialize()
{
    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    std::string path = std::string(ROOT_PATH) + "Experiments/TransformFeedback/shaders/feedback";
    if (!feedbackShader.createProgram({path + ".vert", path + ".frag"}))
    {
        return false;
    }
    std::cout << "Loaded shader " << path << " (" << feedbackShader.getProgram() << ")\n";

    if (!createFeedbackProgram())
    {
        return false;
    }
    createBuffers();
    initTransformFeedback();

    return true;
}

void TransformFeedbackApplication::update()
{
}

void TransformFeedbackApplication::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_RASTERIZER_DISCARD);

    glUseProgram(transformProgram);
    glBindVertexArray(transformVAO);
    glUniform1f(timeLocation, fw::Framework::getTimeSinceStart());
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback);
    glBeginTransformFeedback(GL_TRIANGLES);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glEndTransformFeedback();
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

    glDisable(GL_RASTERIZER_DISCARD);

    glUseProgram(feedbackShader.getProgram());
    glBindVertexArray(feedbackVAO);
    glDrawTransformFeedback(GL_TRIANGLES, transformFeedback);
}

void TransformFeedbackApplication::gui()
{
}

bool TransformFeedbackApplication::createFeedbackProgram()
{
    std::string shaderCode;
    std::string filename = std::string(ROOT_PATH) + "Experiments/TransformFeedback/shaders//transform.vert";
    std::ifstream file(filename);
    if (file)
    {
        shaderCode.append(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        file.close();
    }
    else
    {
        std::cerr << "ERROR: Could not open file: " << filename << "\n";
        return false;
    }

    const char* shaderCodeCstr = shaderCode.c_str();
    GLuint shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader, 1, &shaderCodeCstr, nullptr);
    glCompileShader(shader);

    if (!fw::isShaderCompiled(shader))
    {
        std::cerr << "ERROR: Shader compilation failed: " << filename << "\n";
    }

    transformProgram = glCreateProgram();
    glAttachShader(transformProgram, shader);
    const GLchar* varyings[] = {"outPosition"}; // Also other outputs could be specified
    glTransformFeedbackVaryings(transformProgram, 1, varyings, GL_INTERLEAVED_ATTRIBS);
    glLinkProgram(transformProgram);

    glDetachShader(transformProgram, shader);
    glDeleteShader(shader);

    if (!fw::isProgramLinked(transformProgram))
    {
        std::cerr << "ERROR: Program linking failed: " << filename << "\n";
        return false;
    }

    return true;
}

void TransformFeedbackApplication::createBuffers()
{
    GLfloat data[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f};

    glGenVertexArrays(1, &transformVAO);
    glBindVertexArray(transformVAO);
    glGenBuffers(1, &transformBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, transformBuffer);
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(data), data, 0);
    glVertexAttribPointer(positionIndex, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionIndex);

    glGenVertexArrays(1, &feedbackVAO);
    glBindVertexArray(feedbackVAO);
    glGenBuffers(1, &feedbackBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, feedbackBuffer);
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(data), 0, 0);
    glVertexAttribPointer(positionIndex, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionIndex);
}

void TransformFeedbackApplication::initTransformFeedback()
{
    glGenTransformFeedbacks(1, &transformFeedback);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedbackBuffer);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}
