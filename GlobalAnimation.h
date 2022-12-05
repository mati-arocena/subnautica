#pragma once

#include <glm/glm.hpp>

class GlobalAnimation
{

public:

	virtual glm::mat4 getModelMatrix(double deltaTime) = 0;
	virtual glm::vec3 getTranslation(double deltaTime) = 0;
	virtual glm::quat getRotation(double deltaTime) = 0;
};

