#include "Framework.h"
#include <iostream>

namespace
{

void GLAPIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/,
								const GLchar* message, const void* /*userParam*/)
{
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
		return;
	}

	std::cerr << "OpenGL debug callback function\n";
	std::cerr << "    Message: " << message << "\n";

	std::cerr << "    Source: ";
	switch (source) {
	case GL_DEBUG_SOURCE_API:
		std::cerr << "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		std::cerr << "WINDOW_SYSTEM";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		std::cerr << "SHADER_COMPILER";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		std::cerr << "THIRD_PARTY";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		std::cerr << "APPLICATION";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		std::cerr << "OTHER";
		break;
	}
	std::cerr << "\n";

	std::cerr << "    Type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		std::cerr << "TYPE_ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cerr << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cerr << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cerr << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cerr << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_MARKER:
		std::cerr << "MARKER";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		std::cerr << "PUSH_GROUP";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		std::cerr << "POP_GROUP";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cerr << "OTHER";
		break;
	default:
		std::cerr << "UNKNOWN";
		break;
	}
	std::cerr << "\n";

	std::cerr << "    ID: " << id << "\n";
	std::cerr << "    Severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		std::cerr << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cerr << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cerr << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cerr << "NOTIFICATION";
		break;
	default:
		std::cerr << "UNKNOWN";
		break;
	}
	std::cerr << "\n\n";
}

} // anonymous

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
		std::cerr << "ERROR: Failed to initialize SDL2\n";
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
	if (!window) {
		std::cerr << "ERROR: Failed to create SDL2 window\n";
		return false;
	}

	context = SDL_GL_CreateContext(window);

	int value = 0;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
	std::cout << "SDL_GL_CONTEXT_MAJOR_VERSION: " << value << "\n";
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
	std::cout << "SDL_GL_CONTEXT_MINOR_VERSION: " << value << "\n";

	if (glDebugMessageCallback) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, GL_TRUE);
		glDebugMessageCallback(glDebugCallback, nullptr);
	}

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "ERROR: " << glewGetErrorString(err) << "\n";
		return false;
	}

	if (!app) {
		std::cerr << "ERROR: Invalid application pointer\n";
		return false;
	}

	if (!app->initialize()) {
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