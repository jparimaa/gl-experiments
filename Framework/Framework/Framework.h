#pragma once

#include "Application.h"
#include <GL/glew.h>
#include <SDL.h>

namespace fw
{

class Framework
{
public:
	static unsigned int getTimeSinceStart();

	explicit Framework();
	~Framework();
	Framework(const Framework&) = delete;
	Framework(Framework&&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework& operator=(Framework&&) = delete;

	bool initialize();
	bool setApplication(Application* application);
	void execute();
	void uninitialize();
	
private:
	static unsigned int timeSinceStart;

	Application* app = nullptr;
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;
};

} // fw