#include "OrbitAnimation.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


OrbitAnimation::OrbitAnimation(glm::vec3 center, float radius, float orbitSpeed, bool reverse)
{
	this->center = center;
	this->radius = radius;
	this->orbitSpeed = orbitSpeed;
	this->reverse = reverse;
}

glm::quat OrbitAnimation::getRotation(double deltaTime)
{
	time += deltaTime * orbitSpeed;
	if (reverse)
	{
		return glm::angleAxis((float)time, glm::vec3(0, 1, 0));
	}
	else
	{
		return glm::angleAxis((float)time, glm::vec3(0, -1, 0));
	}
}

glm::vec3 OrbitAnimation::getTranslation(double deltaTime)
{
	return center + glm::vec3(0, 0, radius);
}


glm::mat4 OrbitAnimation::getModelMatrix(double deltaTime)
{
	time += deltaTime * orbitSpeed;

	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, center);
	if (reverse)
	{
		model = glm::rotate(model, (float)time, glm::vec3(0, 1, 0));
	}
	else
	{
		model = glm::rotate(model, (float)time, glm::vec3(0, -1, 0));
	}
	model = glm::translate(model, glm::vec3(radius, 0.f, 0.f));

	return model;
	
}


