#pragma once

#include "Camera.h"
#include <glm/glm.hpp>
#include <SDL.h>

namespace fw
{

class CameraController
{
public:
	explicit CameraController();
	~CameraController();
	CameraController(const CameraController&) = delete;
	CameraController(CameraController&&) = delete;
	CameraController& operator=(const CameraController&) = delete;
	CameraController& operator=(CameraController&&) = delete;

	void setCamera(Camera* c);
	void setMovementSpeed(float s);
	void setSensitivity(float s);
	void setResetMode(const glm::vec3& pos, const glm::vec3& rot, SDL_Keycode key);
	void update();
	
private:
	Camera* camera = nullptr;
	float movementSpeed;
	float sensitivity;
	glm::vec3 resetPosition;
	glm::vec3 resetRotation;
	SDL_Keycode resetKey = -1;
};

} // fw
