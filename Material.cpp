#include "Material.h"


Material::Material(
	std::vector<std::shared_ptr<Texture>> textures,
	std::shared_ptr<Shader> shader, 
	glm::vec3 diffuseColor,
	glm::vec3 specColor,
	float specularStrenght,
	float specularExponent
)
{
	this->shader = shader;
	this->textureFactor = glm::ivec3(0);

	this->textures = textures;
	for (unsigned int x = 0; x < textures.size(); x++)
	{
		if (textures[x]->name == TEXTURE_DIFFUSE)
		{
			textureFactor.x = 1;
		}
		if (textures[x]->name == TEXTURE_SPECULAR)
		{
			textureFactor.y = 1;
		}
		if (textures[x]->name == TEXTURE_NORMAL)
		{
			textureFactor.z = 1;
		}
		shader->setTexture(textures[x], x);
	}

	this->diffuseColor = diffuseColor;
	this->specColor = specColor;
	this->specularStrenght = specularStrenght;
	this->specularExponent = specularExponent;
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

	shader->setFloat("difuse_color", diffuseColor.r, diffuseColor.g, diffuseColor.b);
	shader->setFloat("specular_color", specColor.r, specColor.g, specColor.b);
	shader->setFloat("specular_strenght", specularStrenght);
	shader->setFloat("specular_exponent", specularExponent);
	shader->setFloat("texture_factor", textureFactor.x, textureFactor.y, textureFactor.z);
}

std::shared_ptr<Shader> Material::getShader()
{
	return shader;
}

std::shared_ptr<Shader> Material::changeShader(std::shared_ptr<Shader> shader)
{
	std::shared_ptr<Shader> previousShrd = this->shader;
	this->shader = shader;
	return previousShrd;
}
