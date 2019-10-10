#pragma once

#include <SDL.h>
#include <unordered_map>

namespace fw
{

class Input
{
public:
	static void setKeyDown(SDL_Keycode k);
	static void setKeyUp(SDL_Keycode k);
	static void setMouseDeltaX(int x);
	static void setMouseDeltaY(int y);

	static bool isKeyDown(SDL_Keycode k);
	static bool isKeyPressed(SDL_Keycode k);
	static bool isKeyReleased(SDL_Keycode k);
	static int getMouseDeltaX();
	static int getMouseDeltaY();
	static void clearPressed();
	static void clearReleased();

private:
	static std::unordered_map<SDL_Keycode, bool> keys;
	static std::unordered_map<SDL_Keycode, bool> keysPressed;
	static std::unordered_map<SDL_Keycode, bool> keysReleased;
	static int deltaX;
	static int deltaY;

	Input();
};

} // fw
