#pragma once

#include <Framework/Application.h>
#include <Framework/Shader.h>
#include <GL/glew.h>

struct IndirectCommand
{
	GLuint count = 0;
	GLuint instanceCount = 0;
	GLuint firstIndex = 0;
	GLuint baseVertex = 0;
	GLuint baseInstance = 0;
};

class SimpleIndirectApplication : public fw::Application
{
public:
	explicit SimpleIndirectApplication();
	virtual ~SimpleIndirectApplication();
	SimpleIndirectApplication(const SimpleIndirectApplication&) = delete;
	SimpleIndirectApplication(SimpleIndirectApplication&&) = delete;
	SimpleIndirectApplication& operator=(const SimpleIndirectApplication&) = delete;
	SimpleIndirectApplication& operator=(SimpleIndirectApplication&&) = delete;

	virtual bool initialize() final;
	virtual void update() final;
	virtual void render() final;
	virtual void gui() final;

private:
	fw::Shader shader;

	GLuint VAO = 0;
	GLuint vertexBuffer = 0;
	GLuint indexBuffer = 0;
	GLuint commandBuffer = 0;

	void createDataBuffers();
};
