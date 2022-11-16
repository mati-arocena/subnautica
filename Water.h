#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"


class Water
{
private:
	glm::vec3 position;
	Mesh* mesh;
	Texture* refrTexture;
	Texture* reflTexture;
	unsigned int reflFrameBuffer;
	unsigned int refrFrameBuffer;

public:
	Water();
	void draw(Camera* camera);
	void unbindFrameFuffer();
	void update(std::vector<Model*> models, Camera* camera);
	Material* initializeMaterial();

};

