#pragma once

#include "Application.h"
#include <GL/glew.h>
#include <SDL.h>

namespace fw
{

class Framework
{
public:
	explicit Framework(Application* application);
	~Framework();
	Framework(const Framework&) = delete;
	Framework(Framework&&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework& operator=(Framework&&) = delete;

	bool initialize();
	void execute();
	void uninitialize();
	
private:
	Application* app = nullptr;
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;
};

} // fw