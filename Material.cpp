#include "Material.h"



Material::Material(std::vector<Texture> textures) 
{
	this->textures = textures;
}

Material::~Material()
{
}

void Material::use()
{
	shader.use();

	for (unsigned int x = 0; x < textures.size(); x++)
	{
		textures[x].use(x);
	}
}
