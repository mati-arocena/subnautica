#include "World.h"

World::World()
{
	std::shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration = std::make_shared<btDefaultCollisionConfiguration>();
	collisionDispatcher = std::make_shared<btCollisionDispatcher>(collisionConfiguration.get());
	std::shared_ptr<btBroadphaseInterface> overlappingPairCache = std::make_shared<btDbvtBroadphase>();
	std::shared_ptr<btSequentialImpulseConstraintSolver> solver = std::make_shared<btSequentialImpulseConstraintSolver>();

	dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(collisionDispatcher.get(), overlappingPairCache.get(), solver.get(), collisionConfiguration.get());

	dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

void World::update(double deltaTime)
{
	dynamicsWorld->stepSimulation(deltaTime, 10);
}
