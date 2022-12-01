#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(glm::vec3 color, glm::vec3 direction)
{
	this->color = color;
	this->direction = direction;
}

glm::vec3 DirectionalLight::getColor() const
{
	return color;
}

glm::vec3 DirectionalLight::getDirection() const
{
	return direction;
}

void DirectionalLight::setColor(const glm::vec3& color)
{
	this->color = color;
}

void DirectionalLight::setDirection(const glm::vec3& direction)
{
	this->direction = direction;
}
