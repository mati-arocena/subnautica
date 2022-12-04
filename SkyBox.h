#pragma once
#include "GameObject.h"
#include "Shader.h"
#include <string>


static float SIZE = 1.f;

static float VERTICES[108] = {
	-SIZE,  SIZE, -SIZE,
	-SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	 SIZE, -SIZE, -SIZE,
	 SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,

	-SIZE, -SIZE,  SIZE,
	-SIZE, -SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE,  SIZE,
	-SIZE, -SIZE,  SIZE,

	 SIZE, -SIZE, -SIZE,
	 SIZE, -SIZE,  SIZE,
	 SIZE,  SIZE,  SIZE,
	 SIZE,  SIZE,  SIZE,
	 SIZE,  SIZE, -SIZE,
	 SIZE, -SIZE, -SIZE,

	-SIZE, -SIZE,  SIZE,
	-SIZE,  SIZE,  SIZE,
	 SIZE,  SIZE,  SIZE,
	 SIZE,  SIZE,  SIZE,
	 SIZE, -SIZE,  SIZE,
	-SIZE, -SIZE,  SIZE,

	-SIZE,  SIZE, -SIZE,
	 SIZE,  SIZE, -SIZE,
	 SIZE,  SIZE,  SIZE,
	 SIZE,  SIZE,  SIZE,
	-SIZE,  SIZE,  SIZE,
	-SIZE,  SIZE, -SIZE,

	-SIZE, -SIZE, -SIZE,
	-SIZE, -SIZE,  SIZE,
	 SIZE, -SIZE, -SIZE,
	 SIZE, -SIZE, -SIZE,
	-SIZE, -SIZE,  SIZE,
	 SIZE, -SIZE,  SIZE
};

class SkyBox : public GameObject
{
private:
	unsigned int ID;
	std::shared_ptr<Shader> shader;
	VBO* vboLOD1;
	unsigned int vaoLOD1;

public:
	SkyBox();

	void update(double DeltaTime) override;
	void render() override;
	void renderOclussion() override;
	void render_withShader(std::shared_ptr<Shader> shader) override;
};

