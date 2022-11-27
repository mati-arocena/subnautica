#pragma once

#include <gl/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "Texture.h"
#include "Light.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader
{
public:

	// Program id
	unsigned int ID;
	// unsigned int textureQty = 0;
	// const Texture* textures[16];

	// Reads and builds the shader
	Shader();
	Shader(std::string vertexPath, std::string fragmentPath);

	// Use the shader
	void use() const;

	// void addTexture(Texture* texture, const char* name);

	// Set uniform functions
	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setFloat(const std::string& name, float x, float y);
	void setFloat(const std::string& name, float x, float y, float z);
	void setFloat(const std::string& name, float x, float y, float z, float w);

	void setMat4(const std::string& name, glm::mat4& mat);
	void setTexture(std::shared_ptr<Texture> texture, int pos);
	void prerender(std::shared_ptr<Camera> camera, std::shared_ptr<Light> light);
	void lightSpaceTransform(std::shared_ptr<Light> light);
};
