#include "CameraController.h"
#include "Transformation.h"
#include "Framework.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <SDL.h>
#include <iostream>

namespace
{

const float TO_RADIANS = 0.0174532925f;
const float ROTATION_LIMIT = 87.0f * TO_RADIANS;

} // anonymous

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

void CameraController::setCamera(Camera * c)
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

void CameraController::update()
{
	if (!camera) {
		std::cerr << "WARNING: Camera is not set for camera controller\n";
		return;
	}

	Transformation& t = camera->getTransformation();
	float speed = movementSpeed * Framework::getFrameTime();

	if (Input::isKeyDown(SDLK_w)) {
		t.move(t.getForward() * speed);
	}
	if (Input::isKeyDown(SDLK_s)) {
		t.move(-t.getForward() * speed);
	}
	if (Input::isKeyDown(SDLK_a)) {
		t.move(t.getLeft() * speed);
	}
	if (Input::isKeyDown(SDLK_d)) {
		t.move(-t.getLeft() * speed);
	}
	if (Input::isKeyDown(SDLK_r)) {
		t.position = glm::vec3(0.0f, 0.0f, 0.0f);
		t.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	t.rotate(Transformation::UP, -static_cast<float>(Input::getMouseDeltaX()) * sensitivity);
	t.rotate(Transformation::LEFT, static_cast<float>(Input::getMouseDeltaY()) * sensitivity);
	if (t.rotation.x > ROTATION_LIMIT) {
		t.rotation.x = ROTATION_LIMIT;
	}
	if (t.rotation.x < -ROTATION_LIMIT) {
		t.rotation.x = -ROTATION_LIMIT;
	}
}

} // fw
