#include "Material.h"


Material::Material(std::vector<Texture*> textures, std::shared_ptr<Shader> shader) 
{
	//shader = new Shader("shaders/shader.vert", "shaders/shader.frag");
	this->shader = shader;

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

std::shared_ptr<Shader> Material::getShader()
{
	return shader;
}
