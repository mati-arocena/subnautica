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
	std::shared_ptr<Mesh> mesh;
	glm::vec3 velocity;

	const int MAX_PARTICLES = 500;

public:

	ParticleGenerator(glm::vec3 position);
	void generateParticles();

	void setPosition(glm::vec3 position);
	void setVelocity(glm::vec3 velocity);

	bool update(double deltaTime);
	void render();

};

