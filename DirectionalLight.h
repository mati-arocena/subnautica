#pragma once
#include <glm/glm.hpp>
class DirectionalLight
{
	glm::vec3 color;
	glm::vec3 direction;
	

public:
	DirectionalLight(glm::vec3 color, glm::vec3 direction);
	glm::vec3 getColor() const;
	glm::vec3 getDirection() const;
	void setColor(glm::vec3 color);
	void setDirection(glm::vec3 direction);
	
};
	

