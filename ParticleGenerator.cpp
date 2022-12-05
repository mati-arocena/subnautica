#include "ParticleGenerator.h"
#include "GameInstance.h"
#include "Definitions.h"



ParticleGenerator::ParticleGenerator(glm::vec3 position)
{
	std::vector<std::shared_ptr<Texture>> textures;
	std::shared_ptr<Texture> texture = std::make_shared<Texture>("assets/caustics.jpg", "particle_texture", true, true);
	textures.push_back(texture);
	material =  new Material (textures, GameInstance::getInstance().getShader(PARTICLE_SHADER));
	this->position = position;
	vbo = std::make_shared<VBO>();
	this->velocity = glm::vec3(0.f, 1.f, 0.5f);

	Vertex v1({ 1.f, 1.f, 0.f }, { 0.f,0.f,1.f }, { 1.f, 1.0f });
	Vertex v2({ 1.f, -1.f, 0.f }, { 0.f,0.f,1.f }, { 0.f, 1.0f });
	Vertex v3({ -1.f, -1.f, 0.f }, { 1.f,0.f,1.f }, { 0.f, 0.f });
	Vertex v4({ -1.f, 1.f, 0.f }, { 0.f,0.f,1.f }, { 1.0f, 0.f });
	std::vector<Vertex> vertices = { v1,v2,v3,v4 };

	std::vector<unsigned> indices = {
			0, 1, 3,
			1, 2, 3
	};

	glm::mat4 model(1.); // TODO: ver despues como hacer esto

	this->mesh = std::make_shared<Mesh>(vertices, indices, material, model, glm::vec3(-1.f, -1.f, -3.f), glm::vec3(1.f, 1.f, 3.f));
}


void ParticleGenerator::setPosition(glm::vec3 position)
{
	this->position = position;
}

void ParticleGenerator::setVelocity(glm::vec3 velocity)
{
	this->velocity = velocity;
}

void ParticleGenerator::generateParticles()
{
	unsigned new_particles = 3;
	for (int i = 0; i < 3 && particles.size() < MAX_PARTICLES; i++)
	{
		float posX = (rand() % 100) / 100.f;
		float posY = (rand() % 100) / 100.f;
		float posZ = (rand() % 100) / 100.f;
		float velX = (rand() % 100) / 1000.f;
		float velY = (rand() % 100) / 500.f;
		float velZ = (rand() % 100) / 1000.f;
		float size = 0.002f + (rand() % 10) / 1000.f;

		float life = 1.f + (rand() % 100) / 100.f;

		glm::vec3 newVel = velocity * glm::vec3(velX, velY, velZ);
		glm::vec3 newPos = this->position + glm::vec3(posX, posY, posZ);

		std::shared_ptr<Particle> particle = std::make_shared<Particle>(newPos, newVel, glm::vec4(1.f, 1.f, 1.f, 1.f), life, size, 0.0f);
		particles.push_back(particle);
	}

}

bool ParticleGenerator::update(double deltaTime)
{
	generateParticles();
	std::vector<int> remove;

	for (int i = 0; i < particles.size(); i++)
	{
		if (!particles[i]->update(deltaTime))
		{
			remove.push_back(i);
		}
	}

	for (int i = remove.size() - 1; i >= 0; i--)
	{
		particles.erase(particles.begin() + remove[i]);
	}

	return false;
}

void ParticleGenerator::render()
{

	for (std::shared_ptr<Particle> particle : particles)
	{
		material->use();
		glm::mat4 model = particle->getModel();
		material->getShader()->use();
		material->getShader()->setFloat("color", particle->getColor().x, particle->getColor().y, particle->getColor().z, particle->getColor().w);
		this->mesh->setModel(model);
		this->mesh->render();
	}
}
