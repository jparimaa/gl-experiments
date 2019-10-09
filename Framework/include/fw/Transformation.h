#pragma once

#include <glm/glm.hpp>

namespace fw
{
class Transformation
{
public:
    static const glm::vec3 FORWARD;
    static const glm::vec3 UP;
    static const glm::vec3 LEFT;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    explicit Transformation();
    ~Transformation();

    void move(const glm::vec3& translation);
    void rotate(const glm::vec3& axis, float amount);

    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    glm::vec3 getLeft() const;

    const glm::mat4x4& Transformation::updateModelMatrix();
    const glm::mat4x4& Transformation::getModelMatrix();

private:
    glm::vec3 forward = FORWARD;
    glm::vec3 up = UP;
    glm::vec3 left = LEFT;
    glm::mat4x4 modelMatrix;
};
} // namespace fw
