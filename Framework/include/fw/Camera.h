#pragma once

#include "Transformation.h"
#include <glm/glm.hpp>

namespace fw
{
class Camera
{
public:
    explicit Camera();
    explicit Camera(float fov, float aspectRatio, float ncd, float fcd);
    ~Camera();

    const glm::mat4x4& updateViewMatrix();
    const glm::mat4x4& updateProjectionMatrix();

    const glm::mat4x4& getViewMatrix();
    const glm::mat4x4& getProjectionMatrix();

    Transformation& getTransformation();

    void setAspectRatio(float aspectRatio);

    float getNearClipDistance() const;
    float getFarClipDistance() const;
    float getFOV() const;

private:
    float FOV = 45.0f;
    float ratio = 1.33333f;
    float nearClipDistance = 1.0f;
    float farClipDistance = 100.0f;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    Transformation transformation;
};

} // namespace fw
