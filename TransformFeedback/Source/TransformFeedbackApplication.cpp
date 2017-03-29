#include "TransformFeedbackApplication.h"
#include <Framework/Framework.h>
#include <iostream>
#include <cstdlib>

TransformFeedbackApplication::TransformFeedbackApplication()
{
}

TransformFeedbackApplication::~TransformFeedbackApplication()
{
}

bool TransformFeedbackApplication::initialize()
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	
	return true;
}

void TransformFeedbackApplication::update()
{
}

void TransformFeedbackApplication::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TransformFeedbackApplication::gui()
{
}
