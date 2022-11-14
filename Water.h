#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"


class Water
{
private:
	Mesh* mesh;

public:
	Water();
	void draw(Camera* camera);
	Material* initializeMaterial();

};

