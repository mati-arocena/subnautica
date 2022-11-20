#include "Material.h"


Material::Material(std::vector<Texture*> textures, std::shared_ptr<Shader> shader, float specularStrenght)
{
	this->shader = shader;

	this->textures = textures;
	for (unsigned int x = 0; x < textures.size(); x++)
	{
		shader->setTexture(textures[x], x);
	}

	this->specularStrenght = specularStrenght;
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

	shader->setFloat("specular_strenght", specularStrenght);
}

std::shared_ptr<Shader> Material::getShader()
{
	return shader;
}
