#pragma once
#include <vector>
#include "Shader.h"

#define TEXTURE_DIFFUSE "texture_diffuse"
#define TEXTURE_SPECULAR "texture_specular"
#define TEXTURE_NORMAL "texture_normal"

class Material
{
private:
	std::shared_ptr<Shader> shader;
	std::vector<std::shared_ptr<Texture>> textures;

	glm::vec3 diffuseColor;
	glm::vec3 specColor;
	float specularStrenght;
	float specularExponent;

	glm::ivec3 textureFactor; // (diff, spec, norm) Vector que indica si hay textura o no

public:
	Material(std::vector<std::shared_ptr<Texture>> textures, std::shared_ptr<Shader> shader, glm::vec3 diffuseColor = glm::vec3(), glm::vec3 specColor = glm::vec3(), float specularStrenght = 0, float specularExponent = 0);
	~Material();
	void use();
	std::shared_ptr<Shader> getShader();

	std::shared_ptr<Shader> changeShader(std::shared_ptr<Shader> shader);
};

