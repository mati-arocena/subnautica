#include "Water.h"
#include "GameInstance.h"
#include "Definitions.h"
#include <vector>

glm::vec3 Position;
glm::vec3 Normal;
glm::vec2 TexCoords;

Water::Water() : GameObject()
{
	this->position = glm::vec3(0.f, 0.f, 0.f);
	Vertex v1({ 1.f, 0.f, 1.f }, { 0.f,1.f,0.f }, { 1.f, 1.f });
	Vertex v2({ -1.f, 0.f, 1.f }, { 0.f,1.f,0.f }, { 0.f, 1.f });
	Vertex v3({ -1.f, 0.f, -1.f }, { 0.f,1.f,0.f }, { 0.f, 0.f });
	Vertex v4({ 1.f, 0.f, -1.f }, { 0.f,1.f,0.f }, { 1.f, 0.f });
	std::vector<Vertex> vertices = { v1,v2,v3,v4 };

	std::vector<unsigned> indices = {
			0, 1, 3,
			1, 2, 3
	};

	Material* material = initializeMaterial();

	glm::mat4 model(1.); // TODO: ver despues como hacer esto

	mesh = new Mesh(vertices, indices, material, model);
}


Material* Water::initializeMaterial() 
{
	std::vector<Texture*> textures;

	glGenFramebuffers(1, &reflFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, reflFrameBuffer);
	reflTexture = new Texture(800, 600, GL_RGB, "texture_reflection", GL_COLOR_ATTACHMENT0);
	textures.push_back(reflTexture);
	unbindFrameFuffer();

	glGenFramebuffers(1, &refrFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, refrFrameBuffer);
	refrTexture = new Texture(800, 600, GL_RGB, "texture_refraction", GL_COLOR_ATTACHMENT0);
	textures.push_back(refrTexture);
	unbindFrameFuffer();

	//Texture* texture = new Texture("assets/diffuse.jpg", "texture_diffuse", true, true);
	//textures.push_back(texture);

	Material* material = new Material(textures, GameInstance::getInstance().getShader(WATER_SHADER));
	return material;
}


void Water::unbindFrameFuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 800, 600);
}

void Water::update(double deltaTime)
{
	glEnable(GL_CLIP_DISTANCE0);
	std::shared_ptr<Camera> camera = GameInstance::getInstance().getCamera();
	glm::vec3 camPos = camera->Position;
	float distance = 2 * (camPos.y - this->position.y);

	//camera->SetPosition(glm::vec3(camPos.x, camPos.y - distance, camPos.z));
	camera->Position.y = -camera->Position.y;
	camera->InvertPitch();
	camera->updateViewMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, reflFrameBuffer);
	glViewport(0, 0, 800, 600);

	GameInstance::getInstance().render(this);


	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GameInstance::getInstance().render(this);
	unbindFrameFuffer();

	camera->InvertPitch();
	camera->SetPosition(camPos);
	camera->updateViewMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, refrFrameBuffer);
	glViewport(0, 0, 800, 600);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GameInstance::getInstance().render(this);	
	unbindFrameFuffer();

	glDisable(GL_CLIP_DISTANCE0);
}


void Water::render() {
	mesh->render();
}
