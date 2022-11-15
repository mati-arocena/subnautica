#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"


class Water : public GameObject
{
private:
	Mesh* mesh;
	Texture* refrTexture;
	Texture* reflTexture;
	unsigned int reflFrameBuffer;
	unsigned int refrFrameBuffer;

public:
	Water();
	void update(double deltaTime) override;
	void render() override;

	void unbindFrameFuffer();
	Material* initializeMaterial();

};

