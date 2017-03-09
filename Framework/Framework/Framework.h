#pragma once

#include "Application.h"
#include <GL/glew.h>
#include <SDL.h>

namespace fw
{

class Framework
{
public:
	static float getTimeSinceStart();
	static float getFrameTime();

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
	static float timeSinceStart;
	static float frameTime;

	bool running = true;
	Application* app = nullptr;
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;

	void handleEvents();
};

} // fw
