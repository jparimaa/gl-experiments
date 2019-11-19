#include "fw/Camera.h"
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

namespace fw
{
Camera::Camera()
{
    updateViewMatrix();
    updateProjectionMatrix();
}

Camera::~Camera()
{
}

const glm::mat4x4& Camera::updateViewMatrix()
{
    viewMatrix = glm::lookAt(transformation.position,
                             transformation.position + transformation.getForward(),
                             transformation.getUp());
    return viewMatrix;
}

const glm::mat4x4& Camera::updateProjectionMatrix()
{
    projectionMatrix = glm::perspective(FOV, ratio, nearClipDistance, farClipDistance);
    return projectionMatrix;
}

const glm::mat4x4& Camera::getViewMatrix()
{
    return viewMatrix;
}

const glm::mat4x4& Camera::getProjectionMatrix()
{
    return projectionMatrix;
}

Transformation& Camera::getTransformation()
{
    return transformation;
}

void Camera::setAspectRatio(float aspectRatio)
{
    ratio = aspectRatio;
    updateProjectionMatrix();
}

float Camera::getNearClipDistance() const
{
    return nearClipDistance;
}

float Camera::getFarClipDistance() const
{
    return farClipDistance;
}

float Camera::getFOV() const
{
    return FOV;
}
} // namespace fw
