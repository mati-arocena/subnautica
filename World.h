#pragma once

#include <memory>
#include <vector>

#include "GameObject.h"
#include "btBulletDynamicsCommon.h"
#include <map>
class World : public GameObject
{
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
	std::shared_ptr<btCollisionDispatcher> collisionDispatcher;

	std::map<btRigidBody*, std::shared_ptr<GameObject>> collisionObjects;
	std::vector<std::shared_ptr<GameObject>> objectsInFrustum;

	std::shared_ptr<btPairCachingGhostObject> frustum;

	std::unique_ptr<btGhostPairCallback> ghostPairCallback;
public:
	World();

	virtual void update(double deltaTime) override;
	virtual void render() override;
	virtual void render_withShader(std::shared_ptr<Shader> shader) override;
	virtual std::vector<std::shared_ptr<btRigidBody>> getCollisionObject() override;

	std::vector<std::shared_ptr<GameObject>> getObjectsInFrustum();
	void addToWorld(std::shared_ptr<GameObject> gameObject);
	void addToWorld(const Camera& camera);
};

