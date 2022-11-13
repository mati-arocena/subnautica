#include "Material.h"


Material::Material(std::vector<Texture*> textures) 
{
	shader = new Shader("shaders/shader.vert", "shaders/shader.frag");

	this->textures = textures;
	for (unsigned int x = 0; x < textures.size(); x++)
	{
		shader->setTexture(textures[x], x);
	}
}

Material::~Material()
{
}

void Material::use()
{
	shader->use();

	for (unsigned int x = 0; x < textures.size(); x++)
	{	
		textures[x]->use(x);
	}
}

Shader* Material::getShader()
{
	return shader;
}
