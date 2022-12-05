#include "Water.h"
#include "GameInstance.h"
#include "Definitions.h"
#include <vector>
#include "ConfigManager.h"

glm::vec3 Position;
glm::vec3 Normal;
glm::vec2 TexCoords;

Water::Water() : GameObject()
{
	this->position = glm::vec3(0.f, 0.f, 0.f);
	Vertex v1({ 1000.f, 0.f, 1000.f }, { 0.f,1000.f,0.f }, { 500.f, 500.f });
	Vertex v2({ -1000.f, 0.f, 1000.f }, { 0.f,1000.f,0.f }, { 0.f, 500.f });
	Vertex v3({ -1000.f, 0.f, -1000.f }, { 0.f,1000.f,0.f }, { 0.f, 0.f });
	Vertex v4({ 1000.f, 0.f, -1000.f }, { 0.f,1000.f,0.f }, { 500.f, 0.f });
	std::vector<Vertex> vertices = { v1,v2,v3,v4 };

	std::vector<unsigned> indices = {
			0, 1, 3,
			1, 2, 3
	};


	waterReflectionResolution = ConfigManager::getInstance().getWaterReflectionResolution();

	Material* material = initializeMaterial();

	
	glm::mat4 model(1.); // TODO: ver despues como hacer esto


	mesh = new Mesh(vertices, indices, material, model, glm::vec3(-1000.f, -1000.f, -3.f), glm::vec3(1000.f, 1000.f, 3.f));
}


Material* Water::initializeMaterial() 
{
	std::vector<std::shared_ptr<Texture>> textures;

	glGenFramebuffers(1, &reflFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, reflFrameBuffer);
	reflTexture = std::make_shared<Texture>(waterReflectionResolution.x, waterReflectionResolution.y, GL_RGB, GL_RGB, "texture_reflection", GL_COLOR_ATTACHMENT0);
	textures.push_back(reflTexture);

	reflectionDepthTexture = std::make_shared<Texture>(waterReflectionResolution.x, waterReflectionResolution.y, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, "texture_reflection_depth", GL_DEPTH_ATTACHMENT);
	textures.push_back(reflectionDepthTexture);

	unbindFrameFuffer();

	glGenFramebuffers(1, &refrFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, refrFrameBuffer);
	refrTexture = std::make_shared<Texture>(waterReflectionResolution.x, waterReflectionResolution.y, GL_RGB, GL_RGB, "texture_refraction", GL_COLOR_ATTACHMENT0);
	textures.push_back(refrTexture);

	refractionDepthTexture = std::make_shared<Texture>(waterReflectionResolution.x, waterReflectionResolution.y, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, "texture_refraction_depth", GL_DEPTH_ATTACHMENT);
	textures.push_back(refractionDepthTexture);
	unbindFrameFuffer();

	dudv_texture = std::make_shared<Texture>("assets/waterDUDV.png", "dudv_map", true, true);
	textures.push_back(dudv_texture);

	dudv_texture = std::make_shared<Texture>("assets/waterNormalMap.png", "normal_map", true, true);
	textures.push_back(dudv_texture);

	std::shared_ptr<Texture> occlusion_texture = std::make_shared<Texture>("assets/waterOclussion.jpg", "occlusion_map", true, true);
	textures.push_back(occlusion_texture);

	auto occlusionShader = GameInstance::getInstance().getShader(WATER_SHADER_OCCLUSION);
	occlusionShader->setTexture(dudv_texture,4);
	occlusionShader->setTexture(occlusion_texture,6);

	Material* material = new Material(textures, GameInstance::getInstance().getShader(WATER_SHADER));
	return material;
}

void Water::unbindFrameFuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glm::ivec2 windowSize = ConfigManager::getInstance().getWindowSize();
	glViewport(0, 0, windowSize.x, windowSize.y);
}

void Water::update(double deltaTime)
{
	if (waterReflectionResolution != ConfigManager::getInstance().getWaterReflectionResolution())
	{
		waterReflectionResolution = ConfigManager::getInstance().getWaterReflectionResolution();
		reflTexture->resize(waterReflectionResolution);
		reflectionDepthTexture->resize(waterReflectionResolution);
		refrTexture->resize(waterReflectionResolution);
		refractionDepthTexture->resize(waterReflectionResolution);
	}
	
	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_BLEND);
	std::shared_ptr<Camera> camera = GameInstance::getInstance().getCamera();
	glm::vec3 camPos = camera->Position;
	float camDir = camPos.y >= this->position.y ? 1.f : -1.f;
	auto shader = GameInstance::getInstance().getShader(WATER_SHADER);
	
	shader->use();
	shader->setFloat("camera_position", camPos.x, camPos.y, camPos.z);
	shader->setFloat("refract_exp", camPos.y >= this->position.y ? 5.f : 0.1f);
	shader->setFloat("normal_direction", 0, camPos.y >= this->position.y ? 1.f : -1.f, 0.f);


	float distance = 2 * (camPos.y - this->position.y);
	camera->startReflections(camPos.y - distance);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, reflFrameBuffer);
	glViewport(0, 0, waterReflectionResolution.x, waterReflectionResolution.y);

	GameInstance::getInstance().render(this, glm::vec4(0.f, camDir, 0.f, this->position.y * camDir * -1));
	unbindFrameFuffer();

	camera->stopReflections(camPos);


	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, refrFrameBuffer);
	glViewport(0, 0, waterReflectionResolution.x, waterReflectionResolution.y);

	glm::vec3 clearColor = ConfigManager::getInstance().getClearColor();
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GameInstance::getInstance().render(this, glm::vec4(0.f, camDir * -1, 0.f, this->position.y * camDir));
	unbindFrameFuffer();

	glDisable(GL_CLIP_DISTANCE0);
	glDisable(GL_BLEND);
}


void Water::render() {

	auto shader = GameInstance::getInstance().getShader(WATER_SHADER);
	std::shared_ptr<Camera> camera = GameInstance::getInstance().getCamera();

	shader->prerender(camera, GameInstance::getInstance().getPointLight());
	mesh->render();
}

void Water::renderOclussion()
{
	auto shader = GameInstance::getInstance().getShader(WATER_SHADER_OCCLUSION);
	std::shared_ptr<Camera> camera = GameInstance::getInstance().getCamera();

	shader->prerender(camera, GameInstance::getInstance().getPointLight());

	std::shared_ptr<Shader> previousShdr = mesh->material->changeShader(shader);
	mesh->render();
	mesh->material->changeShader(previousShdr);
}

void Water::render_withShader(std::shared_ptr<Shader> shader)
{
	mesh->render_withShader(shader);
}

