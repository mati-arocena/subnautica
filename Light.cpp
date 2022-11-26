#include "Light.h"
#include "Shader.h"

PointLight::PointLight(glm::vec3 color, glm::vec3 position) 
{
	this->color = color;
	this->position = position;
}

glm::vec3 PointLight::getColor() const
{
	return color;
}

glm::vec3 PointLight::getPosition() const
{
	return position;
}

