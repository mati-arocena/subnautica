#include "Particle.h"
#include "Definitions.h"
#include "Material.h"


Particle::Particle(glm::vec3 position, glm::vec3 velocity, glm::vec4 color, float life, float scale, float gravityEffect)
{
	this->position = position;
	this->velocity = velocity;
	this->color = color;
	this->life = life;
	this->scale = scale;
	this->gravityEffect = gravityEffect;
}

glm::vec3 Particle::getPosition()
{
	return glm::vec3();
}

glm::vec3 Particle::getRotation()
{
	return glm::vec3();
}

glm::vec3 Particle::getScale()
{
	return glm::vec3();
}

glm::vec4 Particle::getColor()
{
	return color;
}

bool Particle::update(double deltaTime)
{
	velocity.y += GRAVITY * gravityEffect * deltaTime;
	glm::vec3 change = velocity * (float) deltaTime;
	position = position + change;
	elapsedTime += deltaTime;

	return elapsedTime < life && position.y < 0;
}


glm::mat4 Particle::getModel()
{
	glm::mat4 model;		
	glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));

	model = t * s;

	return model;
}

