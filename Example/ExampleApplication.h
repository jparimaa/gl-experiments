#pragma once

#include <Application.h>

class ExampleApplication : public fw::Application
{
public:
	explicit ExampleApplication();
	virtual ~ExampleApplication();

	virtual void render();

private:
};
