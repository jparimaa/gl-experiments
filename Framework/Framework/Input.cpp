#include "Input.h"
#include <exception>

namespace fw
{

std::unordered_map<SDL_Keycode, bool> Input::keys;
int Input::deltaX;
int Input::deltaY;

void Input::setKeyDown(SDL_Keycode k)
{
	keys[k] = true;
}

void Input::setKeyUp(SDL_Keycode k)
{
	keys[k] = false;
}

void Input::setMouseDeltaX(int x)
{
	deltaX = x;
}

void Input::setMouseDeltaY(int y)
{
	deltaY = y;
}

bool Input::isKeyDown(SDL_Keycode k)
{
	try {
		return keys.at(k);
	} catch (std::exception& /*e*/) {
		return false;
	}	
}

int Input::getMouseDeltaX()
{
	return deltaX;
}

int Input::getMouseDeltaY()
{
	return deltaY;
}

Input::Input()
{
}

} // fw
