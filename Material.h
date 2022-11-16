#pragma once
#include <vector>
#include "Shader.h"

class Material
{
private:
	std::shared_ptr<Shader> shader;
	std::vector<Texture*> textures;

public:
	Material(std::vector<Texture*> textures, std::shared_ptr<Shader> shader);
	~Material();
	void use();
	std::shared_ptr<Shader> getShader();
};

