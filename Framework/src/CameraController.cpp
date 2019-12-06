#include "fw/CameraController.h"
#include "fw/Transformation.h"
#include "fw/Framework.h"
#include "fw/Input.h"
#include "fw/Globals.h"
#include <iostream>

namespace
{
const float ROTATION_LIMIT = 87.0f * fw::TO_RADIANS;
} // namespace

namespace fw
{
CameraController::CameraController()
{
    setMovementSpeed(1.0f);
    setSensitivity(0.5f);
}

CameraController::~CameraController()
{
}

void CameraController::setCamera(Camera* c)
{
    camera = c;
}

void CameraController::setMovementSpeed(float s)
{
    movementSpeed = s;
}

void CameraController::setSensitivity(float s)
{
    sensitivity = s * TO_RADIANS;
}

void CameraController::setResetMode(const glm::vec3& pos, const glm::vec3& rot, SDL_Keycode key)
{
    resetPosition = pos;
    resetRotation = rot;
    resetKey = key;
}

void CameraController::update()
{
    if (!camera)
    {
        std::cerr << "WARNING: Camera is not set for camera controller\n";
        return;
    }

    Transformation& t = camera->getTransformation();
    float speed = movementSpeed * Framework::getFrameTime();

    if (Input::isKeyDown(SDLK_w))
    {
        t.move(t.getForward() * speed);
    }
    if (Input::isKeyDown(SDLK_s))
    {
        t.move(-t.getForward() * speed);
    }
    if (Input::isKeyDown(SDLK_a))
    {
        t.move(t.getLeft() * speed);
    }
    if (Input::isKeyDown(SDLK_d))
    {
        t.move(-t.getLeft() * speed);
    }
    if (Input::isKeyDown(SDLK_q))
    {
        t.move(-t.getUp() * speed);
    }
    if (Input::isKeyDown(SDLK_e))
    {
        t.move(t.getUp() * speed);
    }

    if (!Input::isKeyDown(SDLK_LSHIFT))
    {
        t.rotate(Transformation::UP, -static_cast<float>(Input::getMouseDeltaX()) * sensitivity);
        t.rotate(Transformation::LEFT, static_cast<float>(Input::getMouseDeltaY()) * sensitivity);
        if (t.rotation.x > ROTATION_LIMIT)
        {
            t.rotation.x = ROTATION_LIMIT;
        }
        if (t.rotation.x < -ROTATION_LIMIT)
        {
            t.rotation.x = -ROTATION_LIMIT;
        }
    }

    if (Input::isKeyReleased(resetKey))
    {
        t.position = resetPosition;
        t.rotation = resetRotation;
    }
}
} // namespace fw
