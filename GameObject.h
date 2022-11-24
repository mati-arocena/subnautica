#pragma once

#include <memory>
#include <vector>

#include "Camera.h"

class Shader;

enum class LOD
{
	LOD0,
	LOD1,
	LOD2,
	NotInFrustum
};

class GameObject
{
	LOD getLOD();
	virtual void renderLOD(LOD levelOfDetail) = 0;

public:
	virtual void update(double deltaTime) = 0;
	void render();
	virtual void render_withShader(std::shared_ptr<Shader> shader) = 0;
	virtual std::vector<std::shared_ptr<btRigidBody>> getCollisionObject() = 0;
	virtual void renderOclussion() = 0;
};

