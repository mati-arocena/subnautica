#pragma once
#include "GameObject.h"
#include "Shader.h"
#include <string>


static float SIZE = 80.f;

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
	~SkyBox();

	void update(double DeltaTime);
	void render();
	void renderOclussion();
	void render_withShader(std::shared_ptr<Shader> shader);
	virtual std::vector<std::shared_ptr<btRigidBody>> getCollisionObject() { return std::vector<std::shared_ptr<btRigidBody>>(); };
};

