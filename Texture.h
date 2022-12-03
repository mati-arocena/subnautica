#pragma once

#include <gl/glew.h>
#include <string>
#include <iostream>
#include "stb_image.h"
#include <glm/glm.hpp>

class Texture
{
public:
	// texture id
	unsigned int ID;
	// width, height and nrChannels of image
	int width, height, nrChannels;
	bool alpha, minmap;
	std::string name;
	std::string path;

	GLuint format = GL_RGB;
	GLuint internalFormat = GL_RGB;

	Texture(const char* texturePath, std::string name, bool alpha, bool minmap);
	Texture(int width, int height, int internalFormat, int format, std::string name, int attachment);

	// Use texture (default pos 0)
	void use(int pos) const;
	void use() const;

	void resize(const glm::ivec2& size);
};

