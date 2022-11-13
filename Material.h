#pragma once
#include <vector>
#include "Shader.h"

class Material
{
private:
	Shader* shader;
	std::vector<Texture*> textures;

public:
	Material(std::vector<Texture*> textures);
	~Material();
	void use();
	Shader* getShader();
};

