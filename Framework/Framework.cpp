#include "Framework.h"
#include <iostream>

namespace fw
{

Framework::Framework(Application* application) :
	app(application)
{
}

Framework::~Framework()
{
}

bool Framework::initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cerr << "ERROR: Failed to initialize SDL2.\n";
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
	if (!window) {
		std::cerr << "ERROR: Failed to create SDL2 window.\n";
		return false;
	}

	context = SDL_GL_CreateContext(window);

	int value = 0;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
	std::cout << "SDL_GL_CONTEXT_MAJOR_VERSION: " << value << std::endl;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
	std::cout << "SDL_GL_CONTEXT_MINOR_VERSION: " << value << std::endl;

	if (!app) {
		std::cerr << "ERROR: Invalid application pointer.\n";
		return false;
	}

	return true;
}

void Framework::execute()
{
	bool loop = true;

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	while (loop) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				loop = false;
			}
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					loop = false;
					break;
				}
			}
		}
		
		app->render();
		SDL_GL_SwapWindow(window);
	}
}

void Framework::uninitialize()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

} // fw