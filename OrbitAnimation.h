#pragma once
#include "GlobalAnimation.h"


class OrbitAnimation :
    public GlobalAnimation
{
private:
	glm::vec3 center;
	float radius;
	float orbitSpeed;
	
	double time = 0.0;
	bool reverse = false;
	
public:
	OrbitAnimation(glm::vec3 center, float radius, float orbitSpeed, bool reverse);
	glm::quat getRotation(double deltaTime) override;
	glm::vec3 getTranslation(double deltaTime) override;
	glm::mat4 getModelMatrix(double deltaTime) override;
	
};

