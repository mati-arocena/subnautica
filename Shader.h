#pragma once

#include <gl/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader
{
public:
	// Program id
	unsigned int ID;
	unsigned int textureQty = 0;
	const Texture* textures[16];

	// Reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);

	// Use the shader
	void use() const;

	void addTexture(Texture* texture, const char* name);

	// Set uniform functions
	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setFloat(const std::string& name, float x, float y);
	void setFloat(const std::string& name, float x, float y, float z);
	void setFloat(const std::string& name, float x, float y, float z, float w);

	void setMat4(const std::string& name, glm::mat4& mat);
};
