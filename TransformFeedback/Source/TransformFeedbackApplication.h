#pragma once

#include <Framework/Application.h>
#include <Framework/Shader.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

class TransformFeedbackApplication : public fw::Application
{
public:
	explicit TransformFeedbackApplication();
	virtual ~TransformFeedbackApplication();
	TransformFeedbackApplication(const TransformFeedbackApplication&) = delete;
	TransformFeedbackApplication(TransformFeedbackApplication&&) = delete;
	TransformFeedbackApplication& operator=(const TransformFeedbackApplication&) = delete;
	TransformFeedbackApplication& operator=(TransformFeedbackApplication&&) = delete;

	virtual bool initialize() final;
	virtual void update() final;
	virtual void render() final;
	virtual void gui() final;

private:
};
