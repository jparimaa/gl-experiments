#include "Transformation.h"
#define GLM_FORCE_RADIANS
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace fw
{

const glm::vec3 Transformation::FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 Transformation::UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Transformation::LEFT = glm::vec3(-1.0f, 0.0f, 0.0f);

Transformation::Transformation()
{
}

Transformation::~Transformation()
{
}

void Transformation::move(const glm::vec3& translation)
{
	position += translation;
}

void Transformation::rotate(const glm::vec3& axis, float amount)
{
	rotation += axis * amount;
}

glm::vec3 Transformation::getForward() const
{
	glm::vec4 v =
		glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) *
		glm::vec4(FORWARD.x, FORWARD.y, FORWARD.z, 0.0f);
	return glm::vec3(v.x, v.y, v.z);
}

glm::vec3 Transformation::getUp() const
{
	glm::vec4 v =
		glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) *
		glm::vec4(UP.x, UP.y, UP.z, 0.0f);
	return glm::vec3(v.x, v.y, v.z);
}

glm::vec3 Transformation::getLeft() const
{
	glm::vec4 v =
		glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) *
		glm::vec4(LEFT.x, LEFT.y, LEFT.z, 0.0f);
	return glm::vec3(v.x, v.y, v.z);
}

const glm::mat4x4& Transformation::updateModelMatrix()
{
	modelMatrix =
		glm::translate(position) *
		glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) *
		glm::scale(scale);
	return modelMatrix;
}

const glm::mat4x4& Transformation::getModelMatrix()
{
	return modelMatrix;
}

} // fw
