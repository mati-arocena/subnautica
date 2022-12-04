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
	std::string name;
	// width, height and nrChannels of image
	int width, height, nrChannels;
	GLuint internalFormat = GL_RGB;
	GLuint format = GL_RGB;
	std::string path;
	bool alpha, minmap;


	Texture(const char* texturePath, const std::string& name, bool alpha, bool minmap);
	Texture(int width, int height, int internalFormat, int format, const std::string& name, int attachment);

	// Use texture (default pos 0)
	void use(int pos) const;
	void use() const;

	void resize(const glm::ivec2& size);
};

