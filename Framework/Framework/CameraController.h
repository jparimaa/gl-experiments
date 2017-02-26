#pragma once

#include "Camera.h"

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
	void update();
	
private:
	Camera* camera = nullptr;
	float movementSpeed;
	float sensitivity;
};

} // fw
