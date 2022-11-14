#include "Light.h"
#include "Shader.h"

Light::Light(glm::vec3 color, glm::vec3 position) 
{
	this->color = color;
	this->position = position;

	Shader::addLight(this);
}

glm::vec3 Light::getColor() const
{
	return color;
}

glm::vec3 Light::getPosition() const
{
	return position;
}

