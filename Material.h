#pragma once
#include <vector>
#include "Shader.h"

class Material
{
private:
	std::shared_ptr<Shader> shader;
	std::vector<Texture*> textures;
	float specularStrenght;
public:
	Material(std::vector<Texture*> textures, std::shared_ptr<Shader> shader, float specularStrenght = 0.8);
	~Material();
	void use();
	std::shared_ptr<Shader> getShader();
};

