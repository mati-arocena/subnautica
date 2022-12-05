#pragma once
#include <vector>
#include "Particle.h"
#include "VBO.h"
#include "Material.h"
#include "Mesh.h"

class ParticleGenerator
{
	std::vector<std::shared_ptr<Particle>> particles;
	std::shared_ptr<VBO> vbo;
	Material* material;
	glm::vec3 position;
	glm::vec3 traslation;
	std::shared_ptr<Mesh> mesh;
	glm::vec3 velocity;
	glm::vec3 center;
	const int MAX_PARTICLES = 500;
	const glm::vec3 rotationAxis = { 0.f, 0.f, -1.f };


public:

	ParticleGenerator(glm::vec3 traslation, glm::vec3 center = {0.f,0.f,0.f});
	void generateParticles();

	void setPosition(glm::vec3 position);
	void setVelocity(glm::vec3 velocity);

	void move(glm::vec3 movement, double deltaTime, glm::vec3 front);
	void rotate(float angle, double deltaTime);

	bool update(double deltaTime);
	void render();

};

