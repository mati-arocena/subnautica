#include "Material.h"

Material::Material(
	std::vector<Texture*> textures, 
	std::shared_ptr<Shader> shader, 
	glm::vec3 diffuseColor,
	glm::vec3 specColor,
	float specularStrenght,
	float specularExponent
)
	:textures(textures), diffuseColor(diffuseColor), shader(shader), specColor(specColor), specularStrenght(specularStrenght), specularExponent(specularExponent)
{
	this->textureFactor = glm::ivec3(0);

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
}

void Material::use()
{
	shader->use();

	for (unsigned int x = 0; x < textures.size(); x++)
	{	
		shader->setTexture(textures[x], x);
		textures[x]->use(x);
	}

	shader->setFloat("difuse_color", diffuseColor.r, diffuseColor.g, diffuseColor.b);
	shader->setFloat("specular_color", specColor.r, specColor.g, specColor.b);
	shader->setFloat("specular_strenght", specularStrenght);
	shader->setFloat("specular_exponent", specularExponent);
	shader->setFloat("texture_factor", static_cast<float>(textureFactor.x), static_cast<float>(textureFactor.y), static_cast<float>(textureFactor.z));
}

std::shared_ptr<Shader> Material::getShader()
{
	return shader;
}

std::shared_ptr<Shader> Material::changeShader(std::shared_ptr<Shader> newShader)
{
	std::shared_ptr<Shader> previousShrd = this->shader;
	this->shader = std::move(newShader);
	return previousShrd;
}
