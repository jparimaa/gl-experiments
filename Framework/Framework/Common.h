#pragma once

#include "Framework.h"
#include "Input.h"
#include "imgui/imgui.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <SDL.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ostream>

namespace fw
{

template <typename T>
inline int executeGenericMain()
{
	fw::Framework framework;
	bool success = framework.initialize();
	if (success) {
		T app;
		success = framework.setApplication(&app);
		if (success) {
			framework.execute();
		}
		framework.uninitialize();
	}

	if (!success) {
		std::cout << "Press any key to exit\n";
		std::cin.get();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

inline void toggleRelativeMouseMode(SDL_Keycode k = SDLK_LSHIFT)
{
	if (fw::Input::isKeyPressed(k)) {
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	if (fw::Input::isKeyReleased(k)) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
}

inline void displayFps()
{
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

inline void displayPosition(const std::string& text, const glm::vec3& pos)
{
	ImGui::Text(text.c_str(), pos.x, pos.y, pos.z);
}

inline GLubyte getRandomColor()
{
	return static_cast<GLubyte>(std::rand() % 255);
}

inline bool hasExtension(const std::string& ext)
{
	GLint numExtensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	for (GLint i = 0; i < numExtensions; ++i) {
		if (ext == std::string((char const*)glGetStringi(GL_EXTENSIONS, i))) {
			return true;
		}
	}
	return false;
}

inline void printExtensions(std::ostream& stream)
{
	GLint numExtensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	for (GLint i = 0; i < numExtensions; ++i) {
		stream << glGetStringi(GL_EXTENSIONS, i) << "\n";
	}
}

} // fw
