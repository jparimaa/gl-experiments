#include "fw/Input.h"

namespace fw
{
std::unordered_map<SDL_Keycode, bool> Input::keys;
std::unordered_map<SDL_Keycode, bool> Input::keysPressed;
std::unordered_map<SDL_Keycode, bool> Input::keysReleased;
int Input::deltaX;
int Input::deltaY;

void Input::setKeyDown(SDL_Keycode k)
{
    if (keys.count(k) && keys.at(k) == false)
    {
        keysPressed[k] = true;
    }
    keys[k] = true;
}

void Input::setKeyUp(SDL_Keycode k)
{
    if (keys.count(k) && keys.at(k) == true)
    {
        keysReleased[k] = true;
    }
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
    return keys.count(k) != 0 && keys.at(k);
}

bool Input::isKeyPressed(SDL_Keycode k)
{
    return keysPressed.count(k) != 0 && keysPressed.at(k);
}

bool Input::isKeyReleased(SDL_Keycode k)
{
    return keysReleased.count(k) != 0 && keysReleased.at(k);
}

int Input::getMouseDeltaX()
{
    return deltaX;
}

int Input::getMouseDeltaY()
{
    return deltaY;
}

void Input::clearPressed()
{
    keysPressed.clear();
}

void Input::clearReleased()
{
    keysReleased.clear();
}

Input::Input()
{
}
} // namespace fw
