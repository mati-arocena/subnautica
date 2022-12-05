#include "Light.h"
#include "Shader.h"

Light::Light(glm::vec3 color, glm::vec3 direction) 
{
	this->color = color;
	this->direction = glm::normalize(direction);
	this->position = glm::vec3(.0f, .0f, .0f) - (this->direction * 30.f);
}

glm::vec3 Light::getColor() const
{
	return color;
}

glm::vec3 Light::getDirection() const
{
	return direction;
}

glm::vec3 Light::getPosition() const
{
	return position;
}

void Light::updatePosition(glm::vec3 position)
{
	this->position = position - (this->direction * 30.f);
}

glm::mat4 Light::getLightProjection()
{
	return glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, 0.1f, 100.f);
}

glm::mat4 Light::getLightView()
{
	glm::vec3 up = glm::vec3(.0f, 1.0f, .0f);
	return glm::lookAt(this->position, this->position + (this->direction), up);
}

