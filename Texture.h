#pragma once

#include <gl/glew.h>
#include <string>
#include <iostream>
#include "stb_image.h"

class Texture
{
public:
	// texture id
	unsigned int ID;
	// width, height and nrChannels of image
	int width, height, nrChannels;
	bool alpha, minmap;
	std::string name;

	Texture(const char* texturePath, std::string name, bool alpha, bool minmap);

	// Use texture (default pos 0)
	void use(int pos) const;
	void use() const;
};

