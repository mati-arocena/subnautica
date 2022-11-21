#pragma once

#include <memory>
#include "GameObject.h"
#include "btBulletDynamicsCommon.h"
class World : public GameObject
{
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
	std::shared_ptr<btCollisionDispatcher> collisionDispatcher;
public:
	World();

	virtual void update(double deltaTime) override;
};

