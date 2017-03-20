#pragma once

#include "Framework.h"
#include "Input.h"
#include "imgui/imgui.h"
#include <glm/glm.hpp>
#include <SDL.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

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

} // fw
