#include "Water.h"
#include <vector>

glm::vec3 Position;
glm::vec3 Normal;
glm::vec2 TexCoords;

Water::Water()
{
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

	mesh = new Mesh(vertices, indices, material);
}


void unbindFrameFuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 800, 600);
}

Material* Water::initializeMaterial() 
{
	Shader* shader = new Shader("shaders/water_shader.vert", "shaders/water_shader.frag");
	std::vector<Texture*> textures;

	//unsigned int reflFrameBuffer;
	//glGenFramebuffers(1, &reflFrameBuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, reflFrameBuffer);
	//Texture* reflTexture = new Texture(800, 600, GL_RGB, "texture_reflection", GL_COLOR_ATTACHMENT0);
	//textures.push_back(reflTexture);

	//unsigned int refrFrameBuffer;
	//glGenFramebuffers(1, &refrFrameBuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, refrFrameBuffer);
	//Texture* refrTexture = new Texture(800, 600, GL_RGB, "texture_refraction", GL_COLOR_ATTACHMENT0);
	//textures.push_back(refrTexture);

	Texture* texture = new Texture("assets/diffuse.jpg", "texture_diffuse", true, true);
	textures.push_back(texture);

	Material* material = new Material(textures, shader);
	return material;
}



void Water::draw(Camera* camera)
{
	mesh->Draw(camera);
}
